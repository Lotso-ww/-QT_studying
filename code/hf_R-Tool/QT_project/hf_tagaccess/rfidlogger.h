#ifndef RFIDLOGGER_H
#define RFIDLOGGER_H

#include <QDateTime>
#include <QMap>
#include <QString>

enum class RfidLogLevel {
    Info,
    Warn,
    Error,
};

class RfidLogger
{
public:
    static QString format(RfidLogLevel level, const QString &stage, int attempt,
                          const QString &message, const QMap<QString, QString> &fields = {},
                          const QDateTime &timestamp = QDateTime());
    static QString maskedName(const QString &name);
    static QString maskedMedicalRecord(const QByteArray &record);
    static QString payloadHex(const QByteArray &payload, bool sensitiveDebugEnabled);
};

#endif // RFIDLOGGER_H
