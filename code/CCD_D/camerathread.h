#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include <QThread>
#include <QImage>
#include <QString>
#include <QSqlDatabase>
#include <memory>
#include <atomic>
#include <peak/peak.hpp>
#include <peak_ipl/peak_ipl.hpp>

// 落盘+写库工作线程: 独立 QThread, 不阻塞主线程渲染
// - 抓帧线程通过 imageToSave 信号把要存的图丢过来 (QueuedConnection)
// - 单张抓拍通过 doSaveSingle 投递 (invokeMethod QueuedConnection)
// - 本对象在自己的线程做: PNG编码(只一次) → 写文件 → 写数据库
// - 使用独立的 QSqlDatabase 连接 (WAL模式支持并发读写)
class SaveWorker : public QObject
{
    Q_OBJECT
public:
    explicit SaveWorker(QObject *parent = nullptr);

    void setupSave(int limit, const QString &saveDir);
    void cancelSave();      // 停止保存: 设标志, 后续 doSave 直接跳过

public slots:
    void initDb(const QString &dbPath, qint64 sessionId);
    void closeDb();
    void clearQueue();  // 在 worker 线程清空待处理事件 (必须在 worker 线程调用)
    void doSave(const QImage &img, const QString &path);       // 异步模式
    void doSaveSingle(const QImage &img, const QString &path); // 单张模式

signals:
    void imageSaved(qint64 logId, const QString &path);
    void saveFailed(const QString &reason);
    void saveLimitReached(int savedCount, const QString &saveDir);

private:
    qint64 insertCapture(const QString &imagePath, int mode, const QByteArray &pngBytes);
    void   doSaveInternal(const QImage &img, const QString &path, int mode);

    QSqlDatabase m_db;
    qint64       m_sessionId      = 0;
    int          m_saveLimit      = 0;
    int          m_completedCount = 0;
    QString      m_saveDir;
    std::atomic<bool> m_cancelSave{false};   // 停止保存标志, doSave 入口检查
};

class CameraThread : public QThread
{
    Q_OBJECT
public:
    explicit CameraThread(std::shared_ptr<peak::core::DataStream> dataStream,
                          std::shared_ptr<peak::core::NodeMap> nodeMap,
                          QObject *parent = nullptr);
    ~CameraThread() override;

    void stop();

    // 持续保存控制
    void enableSave(const QString& saveDir, int limit);
    void stopSave();
    int  savedCount() const { return m_savedCount.load(); }
    bool isSaving() const { return m_saveEnabled.load(); }

signals:
    void imageReady(const QImage &img);
    void imageToSave(const QImage &img, const QString &path);

protected:
    void run() override;

private:
    std::shared_ptr<peak::core::DataStream> m_dataStream;
    std::shared_ptr<peak::core::NodeMap>    m_nodeMapRemoteDevice;
    std::atomic<bool> m_isCapturing;

    std::atomic<bool> m_saveEnabled;
    std::atomic<int>  m_savedCount;
    std::atomic<int>  m_saveLimit;
    QString           m_saveDir;
};

#endif // CAMERATHREAD_H
