#include "camerathread.h"
#include <QDebug>

CameraThread::CameraThread(QObject *parent) : QThread(parent), m_isCapturing(false)
{
    try
    {
        // 1. 刷新并获取设备管理器里的设备列表
        auto& deviceManager = peak::DeviceManager::Instance();
        deviceManager.Update();

        if (deviceManager.Devices().empty())
        {
            qDebug() << "错误: 没有找到任何可用相机！请检查连线。";
            return;
        }

        // 2. 打开列表里的第一台相机
        m_device = deviceManager.Devices().at(0)->OpenDevice(peak::core::DeviceAccessType::Control);
        qDebug() << "成功连接相机: " << QString::fromStdString(m_device->ModelName());

        // 3. 打开数据流 (DataStream) 准备接收图像
        m_dataStream = m_device->DataStreams().at(0)->OpenDataStream();

        // 4. 获取相机的图像载荷大小，并向系统申请内存缓冲区 (Buffer)
        auto nodemapRemoteDevice = m_device->RemoteDevice()->NodeMaps().at(0);
        int64_t payloadSize = nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("PayloadSize")->Value();
        int bufferCount = m_dataStream->NumBuffersAnnouncedMinRequired();

        // 将申请到的内存丢进数据流的队列里供底层轮转使用
        for (int i = 0; i < bufferCount; ++i) {
            auto buffer = m_dataStream->AllocAndAnnounceBuffer(payloadSize, nullptr);
            m_dataStream->QueueBuffer(buffer);
        }

    }
    catch (const std::exception& e)
    {
        qDebug() << "相机初始化异常:" << e.what();
    }
}

CameraThread::~CameraThread()
{
    stopCapture();
    wait(); // 确保安全退出子线程循环
}

void CameraThread::startCapture()
{
    if (m_device && m_dataStream && !m_isCapturing)
    {
        m_isCapturing = true;

        // 启动数据流通道
        m_dataStream->StartAcquisition();

        // 向相机硬件发送"开始采集"指令
        auto nodemap = m_device->RemoteDevice()->NodeMaps().at(0);
        nodemap->FindNode<peak::core::nodes::CommandNode>("AcquisitionStart")->Execute();
        nodemap->FindNode<peak::core::nodes::CommandNode>("AcquisitionStart")->WaitUntilDone();

        start(); // 启动 QThread 专属的 run() 函数，进入死循环
    }
}

void CameraThread::stopCapture()
{
    if (m_isCapturing) {
        m_isCapturing = false;

        if (m_device && m_dataStream)
        {
            // 向硬件发送"停止采集"指令
            try
            {
                auto nodemap = m_device->RemoteDevice()->NodeMaps().at(0);
                nodemap->FindNode<peak::core::nodes::CommandNode>("AcquisitionStop")->Execute();
                nodemap->FindNode<peak::core::nodes::CommandNode>("AcquisitionStop")->WaitUntilDone();
            }
            catch(...) {}

            // 停止数据流并清空所有未处理的缓冲区
            m_dataStream->StopAcquisition(peak::core::AcquisitionStopMode::Default);
            m_dataStream->Flush(peak::core::DataStreamFlushMode::DiscardAll);

            // 注销并释放内存，防止内存泄漏
            for (const auto& buffer : m_dataStream->AnnouncedBuffers())
            {
                m_dataStream->RevokeBuffer(buffer);
            }
        }
    }
}

void CameraThread::run()
{
    while (m_isCapturing)
    {
        try
        {
            // 在子线程中阻塞等待相机传回一张图像 (超时时间设为 1000ms)
            auto buffer = m_dataStream->WaitForFinishedBuffer(1000);

            // ==========================================
            // 这里已经拿到图像原始内存指针了！
            // 下一步我们会在这里把裸数据转成 QImage 并发给 UI
            qDebug() << "成功抓取到一帧图像！";
            // ==========================================

            // 图像处理完后，必须把 buffer 重新放回队列，供相机下次使用
            m_dataStream->QueueBuffer(buffer);

        } catch (const peak::core::TimeoutException&)
        {
            // 超时属于正常现象（比如触发没给到位），继续下一轮轮询即可
            continue;
        }
        catch (const std::exception& e)
        {
            emit errorOccurred(QString("抓图异常: %1").arg(e.what()));
            break;
        }
    }
}
