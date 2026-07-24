#include "camerathread.h"
#include <QDebug>
#include <QDateTime>

// 构造函数：接管水管 (DataStream) 和控制节点 (NodeMap)
CameraThread::CameraThread(std::shared_ptr<peak::core::DataStream> dataStream,
                           std::shared_ptr<peak::core::NodeMap> nodeMap,
                           QObject *parent)
    : QThread(parent),
    m_dataStream(dataStream),
    m_nodeMapRemoteDevice(nodeMap),
    m_isCapturing(false)
{
}

// 析构函数：安全退出
CameraThread::~CameraThread()
{
    stop();
    wait(); // 阻塞主线程一瞬间，确保子线程死循环真正停下，防止内存崩溃
}

// 紧急刹车
void CameraThread::stop()
{
    m_isCapturing = false;
    if (m_dataStream)
    {
        m_dataStream->KillWait();
    }
}



// 核心流水线车间
void CameraThread::run()
{
    if (!m_dataStream || !m_nodeMapRemoteDevice) {
        return; // 防御性编程：设备为空则直接退出
    }

    m_isCapturing = true;
    qint64 lastRenderTime = 0; // 新增：用于记录上一次发图的时间

    try {
        // ==========================================
        // 1. 准备阶段：分配内存与启动设备
        // ==========================================
        auto payloadSize = m_nodeMapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("PayloadSize")->Value();

        // 申请 3 个 Buffer，建立“多重缓冲池”，让画面极致丝滑
        for(int i = 0; i < 3; ++i) {
            m_dataStream->AllocAndAnnounceBuffer(static_cast<size_t>(payloadSize), nullptr);
        }

        // 打开水管，开始曝光
        m_dataStream->StartAcquisition();
        m_nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStart")->Execute();

        // 将所有准备好的空桶排队塞给相机
        for (const auto& buf : m_dataStream->AnnouncedBuffers()) {
            m_dataStream->QueueBuffer(buf);
        }

        // ==========================================
        // 2. 核心死循环：疯狂抓图，并控制发图频率
        // ==========================================
        while (m_isCapturing) {
            try {
                // 等待相机装满一个桶，超时设为 1000ms
                auto buffer = m_dataStream->WaitForFinishedBuffer(1000);

                peak::ipl::Image iplRawImage(
                    peak::ipl::PixelFormat(static_cast<peak::ipl::PixelFormatName>(buffer->PixelFormat())),
                    static_cast<uint8_t*>(buffer->BasePtr()),
                    static_cast<size_t>(buffer->Size()),
                    static_cast<size_t>(buffer->Width()),
                    static_cast<size_t>(buffer->Height())
                    );

                auto iplRGBImage = iplRawImage.ConvertTo(peak::ipl::PixelFormat(peak::ipl::PixelFormatName::RGB8));

                // 【核心修复 1】限流机制：计算距离上一次给界面发图过去了多久
                qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
                if (currentTime - lastRenderTime > 33) { // 33ms 约等于 30 FPS，防止撑死主线程信号队列
                    QImage qImg(static_cast<uchar*>(iplRGBImage.PixelPointer(0, 0)),
                                static_cast<int>(iplRGBImage.Width()),
                                static_cast<int>(iplRGBImage.Height()),
                                QImage::Format_RGB888);

                    // 必须用 copy() 进行深拷贝！然后像炮弹一样发射给主界面
                    emit imageReady(qImg.copy());
                    lastRenderTime = currentTime; // 更新发图时间
                }

                // 【细节】不管有没有发给界面，这个空水桶都必须立刻还给相机循环利用
                m_dataStream->QueueBuffer(buffer);

            } catch (const peak::core::TimeoutException&) {
                // 如果等了 1000ms 没图，忽略报错，继续等
                continue;
            } catch (const peak::core::AbortedException&) {
                // 收到 KillWait 的紧急刹车信号，平稳退出循环
                break;
            } catch (const std::exception& e) {
                qDebug() << "抓图循环异常:" << e.what();
                break;
            }
        }

        // ==========================================
        // 3. 打扫战场：对标官方标准的独立异常隔离护甲
        // ==========================================

        // 3.1 先让相机的物理节点停止抓图 (关掉开关)
        try {
            m_nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStop")->Execute();
        } catch (const std::exception& e) {
            qDebug() << "AcquisitionStop 异常，但已被安全拦截:" << e.what();
        }

        // 3.2 再清理数据流通道 (收拾水管和水桶)
        try {
            m_dataStream->KillWait(); // 再次确保底层没有残留的等待阻塞
            m_dataStream->StopAcquisition();
            m_dataStream->Flush(peak::core::DataStreamFlushMode::DiscardAll);

            // 销毁所有缓冲池内存
            for (const auto& buf : m_dataStream->AnnouncedBuffers()) {
                m_dataStream->RevokeBuffer(buf);
            }
        } catch (const std::exception& e) {
            qDebug() << "数据流清理异常，但已被安全拦截:" << e.what();
        }

    } catch (const std::exception& e) {
        qDebug() << "子线程致命异常:" << e.what();
    }
}
