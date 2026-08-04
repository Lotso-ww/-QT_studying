#include "rfidlogdispatcher.h"

#include <QFile>
#include <QDebug>
#include <QTextStream>

RfidLogDispatcher::RfidLogDispatcher(QObject *parent)
    : QObject(parent)
{
}

void RfidLogDispatcher::setFileLoggingEnabled(bool enabled, const QString &filePath)
{
    m_fileLoggingEnabled = enabled;
    if (!filePath.isEmpty())
        m_filePath = filePath;
}

void RfidLogDispatcher::setSensitiveDebugEnabled(bool enabled)
{
    m_sensitiveDebugEnabled = enabled;
}

bool RfidLogDispatcher::isFileLoggingEnabled() const
{
    return m_fileLoggingEnabled;
}

bool RfidLogDispatcher::isSensitiveDebugEnabled() const
{
    return m_sensitiveDebugEnabled;
}

void RfidLogDispatcher::log(RfidLogLevel level, const QString &stage, int attempt,
                            const QString &message, const QMap<QString, QString> &fields)
{
    const QString line = RfidLogger::format(level, stage, attempt, message, fields);
    qInfo().noquote() << line;
    emit entryAdded(line);
    if (!m_fileLoggingEnabled)
        return;
    if (m_filePath.isEmpty()) {
        emit fileWriteFailed(QStringLiteral("Local log file path is empty."));
        return;
    }

    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        emit fileWriteFailed(QStringLiteral("Cannot open local log file."));
        return;
    }
    QTextStream stream(&file);
    stream << RfidLogger::format(level, stage, attempt, message, redactForFile(fields)) << '\n';
}

QMap<QString, QString> RfidLogDispatcher::redactForFile(const QMap<QString, QString> &fields) const
{
    if (m_sensitiveDebugEnabled)
        return fields;
    QMap<QString, QString> redacted = fields;
    if (redacted.contains(QStringLiteral("name")))
        redacted[QStringLiteral("name")] = RfidLogger::maskedName(redacted.value(QStringLiteral("name")));
    if (redacted.contains(QStringLiteral("medicalRecord"))) {
        redacted[QStringLiteral("medicalRecord")] = RfidLogger::maskedMedicalRecord(
                redacted.value(QStringLiteral("medicalRecord")).toLatin1());
    }
    if (redacted.contains(QStringLiteral("payloadHex")))
        redacted[QStringLiteral("payloadHex")] = QStringLiteral("[SENSITIVE_HEX]");
    return redacted;
}
