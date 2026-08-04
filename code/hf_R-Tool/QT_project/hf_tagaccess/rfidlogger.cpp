#include "rfidlogger.h"

namespace {
QString levelName(RfidLogLevel level)
{
    switch (level) {
    case RfidLogLevel::Info: return QStringLiteral("INFO");
    case RfidLogLevel::Warn: return QStringLiteral("WARN");
    case RfidLogLevel::Error: return QStringLiteral("ERROR");
    }
    return QStringLiteral("INFO");
}
}

QString RfidLogger::format(RfidLogLevel level, const QString &stage, int attempt,
                           const QString &message, const QMap<QString, QString> &fields,
                           const QDateTime &timestamp)
{
    const QDateTime time = timestamp.isValid() ? timestamp : QDateTime::currentDateTime();
    QString line = QStringLiteral("[%1][%2][%3][%4] %5")
            .arg(time.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz")))
            .arg(levelName(level)).arg(stage).arg(attempt).arg(message);
    for (auto it = fields.constBegin(); it != fields.constEnd(); ++it)
        line += QStringLiteral(" %1=%2").arg(it.key(), it.value());
    return line;
}

QString RfidLogger::maskedName(const QString &name)
{
    if (name.isEmpty())
        return QString();
    return name.left(1) + QStringLiteral("***");
}

QString RfidLogger::maskedMedicalRecord(const QByteArray &record)
{
    const QString value = QString::fromLatin1(record);
    if (value.size() <= 4)
        return value;
    return QStringLiteral("***") + value.right(4);
}

QString RfidLogger::payloadHex(const QByteArray &payload, bool sensitiveDebugEnabled)
{
    return sensitiveDebugEnabled ? QString::fromLatin1(payload.toHex(' ').toUpper())
                                 : QStringLiteral("[SENSITIVE_HEX]");
}
