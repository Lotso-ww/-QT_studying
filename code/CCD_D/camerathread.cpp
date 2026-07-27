#include "camerathread.h"
#include "dbmanager.h"
#include <QDebug>
#include <QDateTime>
#include <QDir>
#include <QImage>
#include <QBuffer>
#include <QFile>
#include <QFileInfo>
#include <QSqlQuery>
#include <QSqlError>
#include <QCoreApplication>
#include <QEvent>

// ==================== SaveWorker ====================

SaveWorker::SaveWorker(QObject *parent) : QObject(parent) {}

void SaveWorker::setupSave(int limit, const QString &saveDir)
{
    m_saveLimit      = limit;
    m_saveDir        = saveDir;
    m_completedCount = 0;
    m_cancelSave     = false;             // 新一轮保存, 清除取消标志
}

void SaveWorker::cancelSave()
{
    m_cancelSave = true;
}

void SaveWorker::initDb(const QString &dbPath, qint64 sessionId)
{
    m_sessionId = sessionId;
    m_db = QSqlDatabase::addDatabase("QSQLITE", "saveWorkerConn");
    m_db.setDatabaseName(dbPath);
    if (!m_db.open()) {
        qWarning() << "SaveWorker: 数据库打开失败:" << m_db.lastError().text();
        return;
    }
    QSqlQuery q(m_db);
    q.exec("PRAGMA foreign_keys = ON");
    q.exec("PRAGMA busy_timeout = 5000");
    q.exec("PRAGMA journal_mode = WAL");
    qDebug() << "SaveWorker: 数据库连接已建立, session_id =" << m_sessionId;
}

void SaveWorker::closeDb()
{
    try {
        if (!m_db.isOpen()) return;
        m_db.close();
        QString connName = m_db.connectionName();
        m_db = QSqlDatabase();
        QSqlDatabase::removeDatabase(connName);
        qDebug() << "SaveWorker: 数据库连接已关闭";
    } catch (...) {
        qWarning() << "SaveWorker: closeDb 异常 (忽略)";
    }
}

void SaveWorker::clearQueue()
{
    // 必须在 worker 线程调用! 清除本线程事件队列里所有待处理的 doSave 调用
    QCoreApplication::removePostedEvents(this, QEvent::MetaCall);
}

void SaveWorker::doSaveInternal(const QImage &img, const QString &path, int mode)
{
    if (m_cancelSave.load()) return;      // 二次检查: 防止在 doSave 入口和此处之间被取消

    if (img.isNull()) {
        emit saveFailed("图像为空");
        return;
    }

    // PNG 只编码一次, 同时用于文件和数据库
    QByteArray pngBytes;
    QBuffer buf(&pngBytes);
    buf.open(QIODevice::WriteOnly);
    img.save(&buf, "PNG");

    // 写文件
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly) || file.write(pngBytes) < 0) {
        emit saveFailed(QString("保存失败: %1").arg(path));
        return;
    }
    file.close();

    // 写数据库 (本线程独立连接)
    qint64 logId = insertCapture(path, mode, pngBytes);
    emit imageSaved(logId, path);
}

void SaveWorker::doSave(const QImage &img, const QString &path)
{
    if (m_cancelSave.load()) return;      // 已取消, 跳过

    doSaveInternal(img, path, ModeAsync);

    m_completedCount++;
    if (m_saveLimit > 0 && m_completedCount >= m_saveLimit) {
        emit saveLimitReached(m_completedCount, m_saveDir);
        m_saveLimit = 0;
    }
}

void SaveWorker::doSaveSingle(const QImage &img, const QString &path)
{
    if (m_cancelSave.load()) return;      // 单张也检查取消标志
    doSaveInternal(img, path, ModeSingle);
}

qint64 SaveWorker::insertCapture(const QString &imagePath, int mode, const QByteArray &pngBytes)
{
    if (!m_db.isOpen()) {
        qWarning() << "SaveWorker: DB not open, 跳过写库";
        return 0;
    }

    try {
        QFileInfo fi(imagePath);
        QString fileName  = fi.fileName();
        QString now      = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");

        QSqlQuery q(m_db);

        q.prepare("SELECT COALESCE(MAX(capture_index), 0) FROM capture_log "
                  "WHERE capture_mode = ? AND session_id = ?");
        q.addBindValue(mode);
        q.addBindValue(m_sessionId);
        if (!q.exec() || !q.next()) return 0;
        int idx = q.value(0).toInt() + 1;

        m_db.transaction();

        q.prepare("INSERT INTO capture_log "
                  "(session_id, capture_time, capture_mode, capture_index, image_path) "
                  "VALUES (?,?,?,?,?)");
        q.addBindValue(m_sessionId);
        q.addBindValue(now);
        q.addBindValue(mode);
        q.addBindValue(idx);
        q.addBindValue(imagePath);
        if (!q.exec()) { m_db.rollback(); return 0; }
        qint64 logId = q.lastInsertId().toLongLong();

        q.prepare("INSERT INTO image_data "
                  "(capture_log_id, image_name, image_path, image_data, image_size, "
                  " capture_mode, capture_time, session_id) "
                  "VALUES (?,?,?,?,?,?,?,?)");
        q.addBindValue(logId);
        q.addBindValue(fileName);
        q.addBindValue(imagePath);
        q.addBindValue(pngBytes);
        q.addBindValue(qint64(pngBytes.size()));
        q.addBindValue(mode);
        q.addBindValue(now);
        q.addBindValue(m_sessionId);
        if (!q.exec()) { m_db.rollback(); return 0; }

        m_db.commit();
        return logId;
    } catch (...) {
        qWarning() << "SaveWorker: insertCapture 异常 (忽略)";
        try { m_db.rollback(); } catch (...) {}
        return 0;
    }
}

// ==================== CameraThread ====================

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

        // 官方建议 ≥5 个缓冲, 减少 WaitForFinishedBuffer 饥饿
        for (int i = 0; i < 5; ++i)
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

                // 关键: 先 QueueBuffer 还缓冲给 SDK, 再 emit
                // 这样释放后下一帧的缓冲早就回来了, 不会因为 emit(QueuedConnection) 的临时深拷贝拖延下一帧
                m_dataStream->QueueBuffer(buffer);

                // 渲染限流 ~30FPS
                qint64 now = QDateTime::currentMSecsSinceEpoch();
                if (now - lastRenderTime > 33) {
                    emit imageReady(qImg.copy());
                    lastRenderTime = now;
                }

                // 持续保存到上限 — 不再阻塞! 直接发 signal 给 SaveWorker
                if (m_saveEnabled && m_savedCount < m_saveLimit) {
                    int idx = m_savedCount.fetch_add(1) + 1;
                    QString ts = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss_zzz");
                    QString path = m_saveDir +
                        QString("/ccd_live_%1_%2.png").arg(ts).arg(idx, 4, 10, QChar('0'));

                    // 把图丢给 SaveWorker (QueuedConnection), 不阻塞抓帧线程
                    emit imageToSave(qImg.copy(), path);

                    if (m_savedCount >= m_saveLimit) {
                        m_saveEnabled = false;
                        // 不在这里 emit saveLimitReached!
                        // 由 SaveWorker 在实际落盘完成后触发
                    }
                }

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