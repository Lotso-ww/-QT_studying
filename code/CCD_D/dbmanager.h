#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QImage>
#include <QSqlDatabase>
#include <QList>
#include <QVariantList>

// 拍照模式
enum CaptureMode : int {
    ModeSingle = 0,   // 单张同步
    ModeAsync  = 1,   // 回调异步
};

// 会话信息 (供 Tab2 列表展示)
struct SessionInfo {
    qint64    id;
    QString   startTime;     // 启动时间
    int       singleCount;    // 该会话下单张同步张数
    int       asyncCount;     // 异步回调张数
    QString   note;          // 备注
};

// 单条图像记录 (供 Tab2 浏览用, 不含 BLOB)
struct ImageRow {
    qint64    id;
    qint64    logId;
    qint64    sessionId;
    QString   name;
    QString   path;
    int       mode;
    QString   captureTime;
};

class DbManager
{
public:
    static bool init(const QString& dbPath);
    static void close();

    // 创建一次新会话, 返回 session_id > 0
    static qint64 createSession(const QString& note = QString());

    // 写入一次拍照 (绑定到 currentSessionId)
    static qint64 insertCapture(const QImage &img, const QString &imagePath, int mode);

    static int nextCaptureIndex(int mode);

    // ========== 查询 API (供 Tab2 使用) ==========

    // 列出所有历史会话
    static QList<SessionInfo> listSessions();

    // 列出某次会话下的所有图像 (按时间排序). sessionId<=0 表示全部会话
    static QList<ImageRow> listImagesBySession(qint64 sessionId);

    // 列出当前会话的所有图像
    static QList<ImageRow> listCurrentSessionImages();

    // 按 image_data.id 直接取 PNG BLOB 并解码为 QImage (方案B: 直接从库读图)
    static QImage loadImageById(qint64 imageId);

    // 当前会话信息
    static qint64 currentSessionId();
    static void setCurrentSessionId(qint64 id);

private:
    static QSqlDatabase m_db;
    static bool m_inited;
    static qint64 m_currentSessionId;   // 本次程序运行对应的会话 ID

    static bool ensureTables();
    static QByteArray encodePng(const QImage &img);
};

#endif // DBMANAGER_H