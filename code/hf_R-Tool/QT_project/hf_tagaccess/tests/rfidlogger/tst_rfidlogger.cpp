#include <QtTest>

#include "rfidlogger.h"
#include "rfidlogdispatcher.h"

#include <QFile>
#include <QTemporaryDir>

class RfidLoggerTest : public QObject
{
    Q_OBJECT

private slots:
    void formatsStructuredLine();
    void masksSensitiveFields();
    void keepsFullDataOnlyWhenEnabled();
    void redactsLocalFileByDefault();
    void writesFullLocalDataOnlyWhenEnabled();
};

void RfidLoggerTest::formatsStructuredLine()
{
    const QDateTime time(QDate(2026, 8, 3), QTime(13, 0, 0, 123));
    QMap<QString, QString> fields;
    fields.insert(QStringLiteral("uid"), QStringLiteral("E004"));
    const QString line = RfidLogger::format(RfidLogLevel::Info, QStringLiteral("READ"), 2,
                                            QStringLiteral("payload decoded"),
                                            fields, time);
    QCOMPARE(line, QStringLiteral("[2026-08-03 13:00:00.123][INFO][READ][2] payload decoded uid=E004"));
}

void RfidLoggerTest::masksSensitiveFields()
{
    const QString name = QString::fromUtf8("\xE5\xBC\xA0\xE4\xB8\x89");
    QCOMPARE(RfidLogger::maskedName(name), QString::fromUtf8("\xE5\xBC\xA0***"));
    QCOMPARE(RfidLogger::maskedMedicalRecord("MR-123456"), QStringLiteral("***3456"));
    QCOMPARE(RfidLogger::maskedMedicalRecord("1234"), QStringLiteral("1234"));
}

void RfidLoggerTest::keepsFullDataOnlyWhenEnabled()
{
    const QByteArray payload = QByteArray::fromHex("01020304");
    QCOMPARE(RfidLogger::payloadHex(payload, false), QStringLiteral("[SENSITIVE_HEX]"));
    QCOMPARE(RfidLogger::payloadHex(payload, true), QStringLiteral("01 02 03 04"));
}

void RfidLoggerTest::redactsLocalFileByDefault()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString path = directory.filePath(QStringLiteral("rfid.log"));
    RfidLogDispatcher dispatcher;
    dispatcher.setFileLoggingEnabled(true, path);
    QMap<QString, QString> fields;
    fields.insert(QStringLiteral("name"), QStringLiteral("Alice"));
    fields.insert(QStringLiteral("medicalRecord"), QStringLiteral("MR-123456"));
    fields.insert(QStringLiteral("payloadHex"), QStringLiteral("01 02 03"));
    dispatcher.log(RfidLogLevel::Info, QStringLiteral("WRITE"), 1,
                   QStringLiteral("payload written"), fields);

    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString line = QString::fromUtf8(file.readAll());
    QVERIFY(line.contains(QStringLiteral("name=A***")));
    QVERIFY(line.contains(QStringLiteral("medicalRecord=***3456")));
    QVERIFY(line.contains(QStringLiteral("payloadHex=[SENSITIVE_HEX]")));
    QVERIFY(!line.contains(QStringLiteral("Alice")));
}

void RfidLoggerTest::writesFullLocalDataOnlyWhenEnabled()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString path = directory.filePath(QStringLiteral("rfid.log"));
    RfidLogDispatcher dispatcher;
    dispatcher.setFileLoggingEnabled(true, path);
    dispatcher.setSensitiveDebugEnabled(true);
    QMap<QString, QString> fields;
    fields.insert(QStringLiteral("name"), QStringLiteral("Alice"));
    fields.insert(QStringLiteral("medicalRecord"), QStringLiteral("MR-123456"));
    fields.insert(QStringLiteral("payloadHex"), QStringLiteral("01 02 03"));
    dispatcher.log(RfidLogLevel::Info, QStringLiteral("WRITE"), 1,
                   QStringLiteral("payload written"), fields);

    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString line = QString::fromUtf8(file.readAll());
    QVERIFY(line.contains(QStringLiteral("name=Alice")));
    QVERIFY(line.contains(QStringLiteral("medicalRecord=MR-123456")));
    QVERIFY(line.contains(QStringLiteral("payloadHex=01 02 03")));
}

QTEST_APPLESS_MAIN(RfidLoggerTest)

#include "tst_rfidlogger.moc"
