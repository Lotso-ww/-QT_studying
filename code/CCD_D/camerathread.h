#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include <QThread>
#include <QImage>
#include <mutex>
#include <memory>
#include <peak/peak.hpp>
#include <peak_ipl/peak_ipl.hpp>

class CameraThread : public QThread
{
    Q_OBJECT
public:
    explicit CameraThread(QObject *parent = nullptr);
    ~CameraThread() override;

    // 控制线程启动和停止的接口
    void startCapture();
    void stopCapture();

signals:
    // 当底层抓到一张图时，通过这个信号把 QImage 发送给 UI 界面显示
    void imageReady(const QImage &image);
    // 发生错误时发送信号给 UI
    void errorOccurred(const QString &errorMsg);

protected:
    // QThread 的核心，子线程实际运行的死循环函数
    void run() override;

private:
    bool m_isCapturing;       // 采集状态标志位
    std::mutex m_stopMutex;   // 保护标志位的线程锁

    // IDS 相机的设备指针
    std::shared_ptr<peak::core::Device> m_device;
    std::shared_ptr<peak::core::DataStream> m_dataStream;
};

#endif // CAMERATHREAD_H
