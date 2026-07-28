#include "dbmanager.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QBuffer>
#include <QDateTime>
#include <QFileInfo>

QSqlDatabase DbManager::m_db;
bool DbManager::m_inited = false;
qint64 DbManager::m_currentSessionId = 0;

bool DbManager::init(const QString &dbPath)
{
    if (m_inited) return true;

    // 清理可能残留的旧连接 (上次崩溃/异常退出后)
    const QString connName = QLatin1String(QSqlDatabase::defaultConnection);
    if (QSqlDatabase::contains(connName)) {
        qWarning() << "发现残留数据库连接, 先清理";
        QSqlDatabase::removeDatabase(connName);
    }

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbPath);

    if (!m_db.open()) {
        qWarning() << "数据库打开失败:" << m_db.lastError().text();
        return false;
    }

    QSqlQuery q(m_db);
    q.exec("PRAGMA foreign_keys = ON");
    q.exec("PRAGMA busy_timeout = 5000");

    // 清理上次崩溃残留的 -wal 数据
    if (!q.exec("PRAGMA wal_checkpoint(TRUNCATE)")) {
        qWarning() << "WAL checkpoint 失败:" << q.lastError().text()
                   << " (可能是旧库非WAL模式, 忽略)";
    }

    // 启用 WAL 模式: 读写不互锁
    if (!q.exec("PRAGMA journal_mode = WAL")) {
        qWarning() << "设置 WAL 模式失败:" << q.lastError().text()
                   << " 回退到默认 DELETE 模式";
    }
    q.exec("PRAGMA wal_autocheckpoint = 1000");

    if (!ensureTables()) {
        qWarning() << "建表失败";
        return false;
    }

    q.exec("PRAGMA wal_checkpoint(TRUNCATE)");

    m_inited = true;
    qDebug() << "数据库已就绪:" << dbPath;
    return true;
}

void DbManager::close()
{
    if (!m_inited) return;

    if (m_db.isOpen()) {
        try { m_db.commit(); } catch (...) {}

        try {
            QSqlQuery q(m_db);
            q.exec("PRAGMA wal_checkpoint(TRUNCATE)");
        } catch (...) {
            qWarning() << "close 时 wal_checkpoint 异常 (忽略)";
        }
    }

    // close → 释放 QSqlDatabase 对象 → removeDatabase
    QString connName = m_db.connectionName();
    m_db.close();
    m_db = QSqlDatabase();             // 释放本对象的引用
    QSqlDatabase::removeDatabase(connName);
    m_inited = false;
}

bool DbManager::ensureTables()
{
    const QString sqlSession =
        "CREATE TABLE IF NOT EXISTS capture_session ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  start_time TEXT NOT NULL,"
        "  end_time TEXT,"
        "  note TEXT,"
        "  created_at TEXT NOT NULL DEFAULT (datetime('now','localtime'))"
        ")";

    const QString sqlLog =
        "CREATE TABLE IF NOT EXISTS capture_log ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  session_id INTEGER,"                                   // 可空, 关联 capture_session.id
        "  capture_time TEXT NOT NULL,"
        "  capture_mode INTEGER NOT NULL DEFAULT 0,"
        "  capture_index INTEGER NOT NULL DEFAULT 0,"
        "  image_path TEXT NOT NULL,"
        "  created_at TEXT NOT NULL DEFAULT (datetime('now','localtime'))"
        ")";

    const QString sqlImg =
        "CREATE TABLE IF NOT EXISTS image_data ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  capture_log_id INTEGER NOT NULL,"
        "  image_name TEXT NOT NULL,"
        "  image_path TEXT NOT NULL,"
        "  image_data BLOB NOT NULL,"
        "  image_size INTEGER NOT NULL DEFAULT 0,"
        "  capture_mode INTEGER NOT NULL DEFAULT 0,"
        "  capture_time TEXT NOT NULL,"
        "  session_id INTEGER,"                                   // 冗余, 便于按会话查图
        "  created_at TEXT NOT NULL DEFAULT (datetime('now','localtime')),"
        "  FOREIGN KEY (capture_log_id) REFERENCES capture_log(id) ON DELETE CASCADE"
        ")";

    // 旧库升级: 缺 session_id 列则自动补上
    QSqlQuery q(m_db);

    for (const QString &s : {sqlSession, sqlLog, sqlImg}) {
        if (!q.exec(s)) {
            qWarning() << "建表失败:" << q.lastError().text() << "\nSQL:" << s;
            return false;
        }
    }

    q.exec("PRAGMA table_info(capture_log)");
    bool hasSessionInLog = false;
    while (q.next()) {
        if (q.value(1).toString() == "session_id") { hasSessionInLog = true; break; }
    }
    if (!hasSessionInLog) {
        if (!q.exec("ALTER TABLE capture_log ADD COLUMN session_id INTEGER")) {
            qWarning() << "升级 capture_log 失败:" << q.lastError().text();
        }
    }

    q.exec("PRAGMA table_info(image_data)");
    bool hasSessionInImg = false;
    while (q.next()) {
        if (q.value(1).toString() == "session_id") { hasSessionInImg = true; break; }
    }
    if (!hasSessionInImg) {
        if (!q.exec("ALTER TABLE image_data ADD COLUMN session_id INTEGER")) {
            qWarning() << "升级 image_data 失败:" << q.lastError().text();
        }
    }

    for (const QString &s : {
        "CREATE INDEX IF NOT EXISTS idx_log_session   ON capture_log (session_id)",
        "CREATE INDEX IF NOT EXISTS idx_img_session   ON image_data (session_id)",
        "CREATE INDEX IF NOT EXISTS idx_capture_time  ON image_data (capture_time)",
        "CREATE INDEX IF NOT EXISTS idx_capture_mode  ON image_data (capture_mode)",
        "CREATE INDEX IF NOT EXISTS idx_image_name    ON image_data (image_name)",
        "CREATE INDEX IF NOT EXISTS idx_capture_log_id ON image_data (capture_log_id)",
    }) {
        if (!q.exec(s)) {
            qWarning() << "建索引失败:" << q.lastError().text();
        }
    }
    return true;
}

// 遇到 "database is locked" 时: 切 DELETE 模式强制合并 WAL, 再切回 WAL
static bool tryRecoverFromLock(QSqlDatabase &db)
{
    qWarning() << "尝试 WAL 恢复...";
    QSqlQuery q(db);
    bool ok = q.exec("PRAGMA journal_mode = DELETE");
    if (!ok) {
        qWarning() << "切 DELETE 模式失败:" << q.lastError().text();
        return false;
    }
    q.exec("PRAGMA journal_mode = WAL");
    q.exec("PRAGMA wal_checkpoint(TRUNCATE)");
    qDebug() << "WAL 恢复完成";
    return true;
}

qint64 DbManager::createSession(const QString &note)
{
    if (!m_inited) return 0;

    for (int attempt = 0; attempt < 2; ++attempt)
    {
        QSqlQuery q(m_db);
        q.prepare("INSERT INTO capture_session (start_time, note) VALUES (?, ?)");
        q.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz"));
        q.addBindValue(note);
        if (q.exec()) {
            m_currentSessionId = q.lastInsertId().toLongLong();
            qDebug() << "新会话已建立, session_id =" << m_currentSessionId;
            return m_currentSessionId;
        }
        // 第一次失败 → 尝试 WAL 恢复 + 重试
        const QString err = q.lastError().databaseText();
        qWarning() << "创建会话失败 (尝试" << (attempt+1) << "/2):" << err;
        if (err.contains("locked", Qt::CaseInsensitive) && attempt == 0) {
            tryRecoverFromLock(m_db);
            continue;
        }
        break;
    }
    return 0;
}

qint64 DbManager::currentSessionId()      { return m_currentSessionId; }
void  DbManager::setCurrentSessionId(qint64 id) { m_currentSessionId = id; }

int DbManager::nextCaptureIndex(int mode)
{
    QSqlQuery q(m_db);
    // 按"当前会话 + 模式"算序号, 每次运行从 1 开始
    q.prepare("SELECT COALESCE(MAX(capture_index), 0) FROM capture_log "
              "WHERE capture_mode = ? AND session_id = ?");
    q.addBindValue(mode);
    q.addBindValue(m_currentSessionId);
    if (!q.exec() || !q.next()) {
        qWarning() << "查询 capture_index 失败:" << q.lastError().text();
        return 1;
    }
    return q.value(0).toInt() + 1;
}

QByteArray DbManager::encodePng(const QImage &img)
{
    QByteArray bytes;
    QBuffer buf(&bytes);
    buf.open(QIODevice::WriteOnly);
    img.save(&buf, "PNG");
    return bytes;
}

qint64 DbManager::insertCapture(const QImage &img, const QString &imagePath, int mode)
{
    if (!m_inited) {
        qWarning() << "数据库未初始化, 跳过写入";
        return 0;
    }
    if (img.isNull()) return 0;

    const QByteArray pngBytes = encodePng(img);
    const QFileInfo fi(imagePath);
    const QString fileName = fi.fileName();
    const QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
    const int idx = nextCaptureIndex(mode);

    m_db.transaction();
    QSqlQuery q(m_db);

    // capture_log
    q.prepare("INSERT INTO capture_log "
              "(session_id, capture_time, capture_mode, capture_index, image_path) "
              "VALUES (?,?,?,?,?)");
    q.addBindValue(m_currentSessionId);
    q.addBindValue(now);
    q.addBindValue(mode);
    q.addBindValue(idx);
    q.addBindValue(imagePath);
    if (!q.exec()) {
        qWarning() << "capture_log 插入失败:" << q.lastError().text();
        m_db.rollback();
        return 0;
    }
    const qint64 logId = q.lastInsertId().toLongLong();

    // image_data
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
    q.addBindValue(m_currentSessionId);
    if (!q.exec()) {
        qWarning() << "image_data 插入失败:" << q.lastError().text();
        m_db.rollback();
        return 0;
    }

    m_db.commit();
    return logId;
}

QList<SessionInfo> DbManager::listSessions()
{
    QList<SessionInfo> list;
    if (!m_inited) return list;

    QSqlQuery q(m_db);
    q.prepare(
        "SELECT s.id, s.start_time, s.note, "
        "  (SELECT COUNT(*) FROM capture_log l WHERE l.session_id = s.id AND l.capture_mode = 0) AS single_cnt, "
        "  (SELECT COUNT(*) FROM capture_log l WHERE l.session_id = s.id AND l.capture_mode = 1) AS async_cnt "
        "FROM capture_session s "
        "ORDER BY s.id DESC");
    if (!q.exec()) {
        qWarning() << "查询会话列表失败:" << q.lastError().text();
        return list;
    }
    while (q.next()) {
        SessionInfo s;
        s.id          = q.value(0).toLongLong();
        s.startTime   = q.value(1).toString();
        s.singleCount = q.value(3).toInt();
        s.asyncCount  = q.value(4).toInt();
        s.note        = q.value(2).toString();
        list.append(s);
    }
    return list;
}

QList<ImageRow> DbManager::listImagesBySession(qint64 sessionId)
{
    QList<ImageRow> list;
    if (!m_inited) return list;

    QSqlQuery q(m_db);
    if (sessionId <= 0) {
        q.prepare("SELECT id, capture_log_id, COALESCE(session_id,0), image_name, image_path, "
                  "       capture_mode, capture_time FROM image_data ORDER BY id ASC");
    } else {
        q.prepare("SELECT id, capture_log_id, COALESCE(session_id,0), image_name, image_path, "
                  "       capture_mode, capture_time FROM image_data WHERE session_id = ? ORDER BY id ASC");
        q.addBindValue(sessionId);
    }
    if (!q.exec()) {
        qWarning() << "查询图像列表失败:" << q.lastError().text();
        return list;
    }
    while (q.next()) {
        ImageRow r;
        r.id          = q.value(0).toLongLong();
        r.logId       = q.value(1).toLongLong();
        r.sessionId   = q.value(2).toLongLong();
        r.name        = q.value(3).toString();
        r.path        = q.value(4).toString();
        r.mode        = q.value(5).toInt();
        r.captureTime = q.value(6).toString();
        list.append(r);
    }
    return list;
}

QList<ImageRow> DbManager::listCurrentSessionImages()
{
    return listImagesBySession(m_currentSessionId);
}

QImage DbManager::loadImageById(qint64 imageId)
{
    QImage img;
    if (!m_inited) return img;

    QSqlQuery q(m_db);
    q.prepare("SELECT image_data FROM image_data WHERE id = ?");
    q.addBindValue(imageId);
    if (!q.exec() || !q.next()) {
        qWarning() << "从库读图失败 id=" << imageId << ":" << q.lastError().text();
        return img;
    }
    const QByteArray pngBytes = q.value(0).toByteArray();
    img.loadFromData(pngBytes, "PNG");
    return img;
}