#include <QtTest>

#include "tagpayloadcodec.h"

class TagPayloadCodecTest : public QObject
{
    Q_OBJECT

private slots:
    void encodesSpecifiedBcdTime();
    void encodesAndDecodesPayload();
    void rejectsInvalidInput();
    void rejectsMalformedPayload();
    void padsToBlockSize();
};

void TagPayloadCodecTest::encodesSpecifiedBcdTime()
{
    QByteArray encoded;
    QString error;
    const QDateTime value(QDate(2023, 1, 31), QTime(17, 43));

    QVERIFY2(TagPayloadCodec::encodeBcdTime(value, &encoded, &error), qPrintable(error));
    QCOMPARE(encoded, QByteArray::fromHex("2301311743"));

    QDateTime decoded;
    QVERIFY2(TagPayloadCodec::decodeBcdTime(encoded, &decoded, &error), qPrintable(error));
    QCOMPARE(decoded, value);
}

void TagPayloadCodecTest::encodesAndDecodesPayload()
{
    TagPayload input;
    input.dishNumber = 1;
    input.inseminationTime = QDateTime(QDate(2023, 1, 31), QTime(17, 43));
    input.femaleName = QString::fromUtf8("\xE5\xBC\xA0\xE4\xB8\x89");
    input.medicalRecordNumber = "2678453101";

    QByteArray encoded;
    QString error;
    QVERIFY2(TagPayloadCodec::encode(input, &encoded, &error), qPrintable(error));
    QCOMPARE(encoded, QByteArray::fromHex("011801230131174320202020D5C5C8FD32363738343533313031"));

    TagPayload decoded;
    QVERIFY2(TagPayloadCodec::decode(encoded, &decoded, &error), qPrintable(error));
    QCOMPARE(decoded.formatVersion, input.formatVersion);
    QCOMPARE(decoded.dishNumber, input.dishNumber);
    QCOMPARE(decoded.inseminationTime, input.inseminationTime);
    QCOMPARE(decoded.femaleName, input.femaleName);
    QCOMPARE(decoded.medicalRecordNumber, input.medicalRecordNumber);

    encoded[0] = char(0x05);
    QVERIFY2(TagPayloadCodec::decode(encoded, &decoded, &error), qPrintable(error));
    QCOMPARE(decoded.formatVersion, quint8(0x05));
    QCOMPARE(decoded.dishNumber, input.dishNumber);
    QCOMPARE(decoded.inseminationTime, input.inseminationTime);
    QCOMPARE(decoded.femaleName, input.femaleName);
    QCOMPARE(decoded.medicalRecordNumber, input.medicalRecordNumber);

    input.formatVersion = 0x05;
    QVERIFY2(TagPayloadCodec::encode(input, &encoded, &error), qPrintable(error));
    QCOMPARE(static_cast<quint8>(encoded.at(0)), quint8(0x05));
}

void TagPayloadCodecTest::rejectsInvalidInput()
{
    TagPayload input;
    input.dishNumber = 1;
    input.inseminationTime = QDateTime(QDate(2023, 1, 31), QTime(17, 43));
    input.femaleName = QStringLiteral("123456789");
    input.medicalRecordNumber = "MR-001";

    QByteArray encoded;
    QVERIFY(!TagPayloadCodec::encode(input, &encoded));

    input.femaleName = QStringLiteral("A");
    input.medicalRecordNumber.clear();
    QVERIFY(!TagPayloadCodec::encode(input, &encoded));

    input.medicalRecordNumber = "MR 001";
    QVERIFY(!TagPayloadCodec::encode(input, &encoded));

    input.medicalRecordNumber = QByteArray(65, 'A');
    QVERIFY(!TagPayloadCodec::encode(input, &encoded));
}

void TagPayloadCodecTest::rejectsMalformedPayload()
{
    TagPayload output;
    QByteArray malformed = QByteArray::fromHex("011801230131174320202020D5C5C8FD32363738343533313031");

    malformed[1] = char(0x19);
    QVERIFY(!TagPayloadCodec::decode(malformed, &output));

    malformed = QByteArray::fromHex("01180123FA31174320202020D5C5C8FD32363738343533313031");
    QVERIFY(!TagPayloadCodec::decode(malformed, &output));

    malformed = QByteArray::fromHex("011801230131174320202020D5C5C8FD32363738");
    QVERIFY(!TagPayloadCodec::decode(malformed, &output));
}

void TagPayloadCodecTest::padsToBlockSize()
{
    QByteArray padded;
    QString error;
    QVERIFY2(TagPayloadCodec::padForBlocks(QByteArray::fromHex("0102030405"), 4, &padded, &error), qPrintable(error));
    QCOMPARE(padded, QByteArray::fromHex("0102030405000000"));
    QVERIFY(!TagPayloadCodec::padForBlocks(QByteArray("x"), 0, &padded));
}

QTEST_APPLESS_MAIN(TagPayloadCodecTest)

#include "tst_tagpayloadcodec.moc"
