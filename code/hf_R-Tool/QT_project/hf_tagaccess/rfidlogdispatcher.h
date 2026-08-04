#ifndef RFIDLOGDISPATCHER_H
#define RFIDLOGDISPATCHER_H

#include "rfidlogger.h"

#include <QObject>

class RfidLogDispatcher : public QObject
{
    Q_OBJECT

public:
    explicit RfidLogDispatcher(QObject *parent = nullptr);

    void setFileLoggingEnabled(bool enabled, const QString &filePath = QString());
    void setSensitiveDebugEnabled(bool enabled);
    bool isFileLoggingEnabled() const;
    bool isSensitiveDebugEnabled() const;
    void log(RfidLogLevel level, const QString &stage, int attempt, const QString &message,
             const QMap<QString, QString> &fields = {});

signals:
    void entryAdded(const QString &line);
    void fileWriteFailed(const QString &message);

private:
    QMap<QString, QString> redactForFile(const QMap<QString, QString> &fields) const;

    bool m_fileLoggingEnabled = false;
    bool m_sensitiveDebugEnabled = false;
    QString m_filePath;
};

#endif // RFIDLOGDISPATCHER_H
