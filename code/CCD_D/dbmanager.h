#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QImage>
#include <QSqlDatabase>
#include <QList>
#include <QVariantList>

enum CaptureMode : int {
    ModeSingle = 0,
    ModeAsync  = 1,
};

struct SessionInfo {
    qint64    id;
    QString   startTime;
    int       singleCount;
    int       asyncCount;
    QString   note;
};

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

    static qint64 createSession(const QString& note = QString());

    static qint64 insertCapture(const QImage &img, const QString &imagePath, int mode);

    static int nextCaptureIndex(int mode);

    static QList<SessionInfo> listSessions();

    static QList<ImageRow> listImagesBySession(qint64 sessionId);

    static QList<ImageRow> listCurrentSessionImages();

    static QImage loadImageById(qint64 imageId);

    static qint64 currentSessionId();
    static void setCurrentSessionId(qint64 id);

private:
    static QSqlDatabase m_db;
    static bool m_inited;
    static qint64 m_currentSessionId;

    static bool ensureTables();
    static QByteArray encodePng(const QImage &img);
};

#endif // DBMANAGER_H