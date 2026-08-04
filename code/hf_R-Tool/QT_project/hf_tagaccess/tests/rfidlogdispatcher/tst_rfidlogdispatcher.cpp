#include <QtTest>

#include "rfidlogdispatcher.h"

#include <QFile>
#include <QTemporaryDir>

class RfidLogDispatcherTest : public QObject
{
    Q_OBJECT

private slots:
    void alwaysEmitsFullUiLine();
    void keepsFileLoggingDisabledByDefault();
    void redactsSensitiveFieldsInLocalFile();
};

void RfidLogDispatcherTest::alwaysEmitsFullUiLine()
{
    RfidLogDispatcher dispatcher;
    QSignalSpy entries(&dispatcher, &RfidLogDispatcher::entryAdded);
    dispatcher.log(RfidLogLevel::Info, QStringLiteral("READ"), 1,
                   QStringLiteral("payload read"),
                   {{QStringLiteral("name"), QStringLiteral("Alice")},
                    {QStringLiteral("payloadHex"), QStringLiteral("01 02")}});

    QCOMPARE(entries.count(), 1);
    const QString line = entries.at(0).at(0).toString();
    QVERIFY(line.contains(QStringLiteral("name=Alice")));
    QVERIFY(line.contains(QStringLiteral("payloadHex=01 02")));
}

void RfidLogDispatcherTest::keepsFileLoggingDisabledByDefault()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString path = directory.filePath(QStringLiteral("rfid.log"));
    RfidLogDispatcher dispatcher;
    dispatcher.log(RfidLogLevel::Info, QStringLiteral("INIT"), 0,
                   QStringLiteral("reader ready"));

    QVERIFY(!QFile::exists(path));
    QVERIFY(!dispatcher.isFileLoggingEnabled());
    QVERIFY(!dispatcher.isSensitiveDebugEnabled());
}

void RfidLogDispatcherTest::redactsSensitiveFieldsInLocalFile()
{
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString path = directory.filePath(QStringLiteral("rfid.log"));
    RfidLogDispatcher dispatcher;
    dispatcher.setFileLoggingEnabled(true, path);
    dispatcher.log(RfidLogLevel::Info, QStringLiteral("WRITE"), 1,
                   QStringLiteral("payload written"),
                   {{QStringLiteral("name"), QStringLiteral("Alice")},
                    {QStringLiteral("medicalRecord"), QStringLiteral("MR-123456")},
                    {QStringLiteral("payloadHex"), QStringLiteral("01 02 03")}});

    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    const QString line = QString::fromUtf8(file.readAll());
    QVERIFY(line.contains(QStringLiteral("name=A***")));
    QVERIFY(line.contains(QStringLiteral("medicalRecord=***3456")));
    QVERIFY(line.contains(QStringLiteral("payloadHex=[SENSITIVE_HEX]")));
}

QTEST_APPLESS_MAIN(RfidLogDispatcherTest)

#include "tst_rfidlogdispatcher.moc"
