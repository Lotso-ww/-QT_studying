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

class SaveWorker : public QObject
{
    Q_OBJECT
public:
    explicit SaveWorker(QObject *parent = nullptr);

    void setupSave(int limit, const QString &saveDir);
    void cancelSave();

public slots:
    void initDb(const QString &dbPath, qint64 sessionId);
    void closeDb();
    void clearQueue();
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
    std::atomic<bool> m_cancelSave{false};
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
