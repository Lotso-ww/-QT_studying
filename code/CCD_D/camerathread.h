#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include <QThread>
#include <QImage>
#include <QString>
#include <memory>
#include <atomic>
#include <peak/peak.hpp>
#include <peak_ipl/peak_ipl.hpp>

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
    // 异步保存已落盘成功, 通知主线程写库 (QImage + 落盘路径)
    void imageSaved(const QImage &img, const QString &path);
    void saveLimitReached(int savedCount, const QString& saveDir);
    void saveFailed(const QString& reason);

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