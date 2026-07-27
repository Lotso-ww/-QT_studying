#include "camerathread.h"
#include <QDebug>
#include <QDateTime>
#include <QDir>
#include <QImage>

CameraThread::CameraThread(std::shared_ptr<peak::core::DataStream> dataStream,
                           std::shared_ptr<peak::core::NodeMap> nodeMap,
                           QObject *parent)
    : QThread(parent)
    , m_dataStream(dataStream)
    , m_nodeMapRemoteDevice(nodeMap)
    , m_isCapturing(false)
    , m_saveEnabled(false)
    , m_savedCount(0)
    , m_saveLimit(0)
{
}

CameraThread::~CameraThread()
{
    stop();
    wait();
}

void CameraThread::stop()
{
    m_isCapturing = false;
    if (m_dataStream)
        m_dataStream->KillWait();
}

void CameraThread::enableSave(const QString& saveDir, int limit)
{
    m_saveDir    = saveDir;
    m_saveLimit  = (limit < 1) ? 1 : limit;
    m_savedCount = 0;
    m_saveEnabled = true;
}

void CameraThread::stopSave()
{
    m_saveEnabled = false;
}

void CameraThread::run()
{
    if (!m_dataStream || !m_nodeMapRemoteDevice)
        return;

    m_isCapturing = true;
    qint64 lastRenderTime = 0;

    try {
        auto payloadSize = m_nodeMapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("PayloadSize")->Value();

        for (int i = 0; i < 3; ++i)
            m_dataStream->AllocAndAnnounceBuffer(static_cast<size_t>(payloadSize), nullptr);

        m_dataStream->StartAcquisition();
        m_nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStart")->Execute();

        for (const auto& buf : m_dataStream->AnnouncedBuffers())
            m_dataStream->QueueBuffer(buf);

        while (m_isCapturing) {
            try {
                auto buffer = m_dataStream->WaitForFinishedBuffer(1000);

                peak::ipl::Image iplRaw(
                    peak::ipl::PixelFormat(static_cast<peak::ipl::PixelFormatName>(buffer->PixelFormat())),
                    static_cast<uint8_t*>(buffer->BasePtr()),
                    static_cast<size_t>(buffer->Size()),
                    static_cast<size_t>(buffer->Width()),
                    static_cast<size_t>(buffer->Height()));

                auto iplRGB = iplRaw.ConvertTo(peak::ipl::PixelFormat(peak::ipl::PixelFormatName::RGB8));

                const int w = static_cast<int>(iplRGB.Width());
                const int h = static_cast<int>(iplRGB.Height());

                QImage qImg(static_cast<uchar*>(iplRGB.PixelPointer(0, 0)),
                            w, h, w * 3, QImage::Format_RGB888);

                // 渲染限流 ~30FPS
                qint64 now = QDateTime::currentMSecsSinceEpoch();
                if (now - lastRenderTime > 33) {
                    emit imageReady(qImg.copy());
                    lastRenderTime = now;
                }

                // 持续保存到上限
                if (m_saveEnabled && m_savedCount < m_saveLimit) {
                    int idx = m_savedCount.fetch_add(1) + 1;
                    QString ts = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss_zzz");
                    QString path = m_saveDir +
                        QString("/ccd_live_%1_%2.png").arg(ts).arg(idx, 4, 10, QChar('0'));

                    if (qImg.save(path)) {
                        // 落盘成功 -> 通知主线程写数据库 (QSqlDatabase 只能在创建它的线程使用)
                        emit imageSaved(qImg.copy(), path);
                    } else {
                        emit saveFailed(QString("保存失败: %1").arg(path));
                    }

                    if (m_savedCount >= m_saveLimit) {
                        m_saveEnabled = false;
                        emit saveLimitReached(m_savedCount.load(), m_saveDir);
                    }
                }

                m_dataStream->QueueBuffer(buffer);

            } catch (const peak::core::TimeoutException&) {
                continue;
            } catch (const peak::core::AbortedException&) {
                break;
            } catch (const std::exception& e) {
                qDebug() << "抓图循环异常:" << e.what();
                break;
            }
        }

        // 清理
        try {
            m_nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStop")->Execute();
        } catch (const std::exception& e) {
            qDebug() << "AcquisitionStop 异常:" << e.what();
        }

        try {
            m_dataStream->KillWait();
            m_dataStream->StopAcquisition();
            m_dataStream->Flush(peak::core::DataStreamFlushMode::DiscardAll);
            for (const auto& buf : m_dataStream->AnnouncedBuffers())
                m_dataStream->RevokeBuffer(buf);
        } catch (const std::exception& e) {
            qDebug() << "数据流清理异常:" << e.what();
        }

    } catch (const std::exception& e) {
        qDebug() << "子线程致命异常:" << e.what();
    }

    m_saveEnabled = false;
}