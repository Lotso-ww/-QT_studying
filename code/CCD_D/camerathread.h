#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include <QThread>
#include <QImage>
#include <memory>
#include <atomic>              // 用于线程安全的布尔值
#include <peak/peak.hpp>
#include <peak_ipl/peak_ipl.hpp>

class CameraThread : public QThread
{
    Q_OBJECT
public:
    // 构造函数：把主线程里已经建好的数据流通道 (水管) 和控制节点 (开关) 传给子线程
    explicit CameraThread(std::shared_ptr<peak::core::DataStream> dataStream,
                          std::shared_ptr<peak::core::NodeMap> nodeMap,
                          QObject *parent = nullptr);
    ~CameraThread();

    // 控制线程停止的紧急刹车
    void stop();

signals:
    // 核心跨线程通信枢纽：子线程抓好图后，通过这个信号把 QImage 当作“炮弹”发射出去
    void imageReady(const QImage &img);

protected:
    // 整个子线程的“流水线车间”，死循环全写在这里面
    void run() override;

private:
    std::shared_ptr<peak::core::DataStream> m_dataStream;
    std::shared_ptr<peak::core::NodeMap> m_nodeMapRemoteDevice;

    // 使用 std::atomic 保证多线程读写安全，防止主线程点“停止”时子线程没反应
    std::atomic<bool> m_isCapturing;
};

#endif // CAMERATHREAD_H
