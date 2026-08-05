#include <QtTest>

#include "rfidtagservice.h"
#include "tagpayloadcodec.h"

class FakeDeviceAdapter : public RfidDeviceAdapter
{
public:
    FakeDeviceAdapter()
    {
        systemInfo.blockSize = 4;
        systemInfo.blockCount = 32;
        TagPayload payload;
        payload.dishNumber = 1;
        payload.inseminationTime = QDateTime(QDate(2023, 1, 1), QTime(0, 0));
        payload.femaleName = QStringLiteral("A");
        payload.medicalRecordNumber = "INITIAL";
        QByteArray raw;
        TagPayloadCodec::encode(payload, &raw);
        QByteArray padded;
        TagPayloadCodec::padForBlocks(raw, systemInfo.blockSize, &padded);
        storage = QByteArray(systemInfo.blockSize * systemInfo.blockCount, '\0');
        storage.replace(0, padded.size(), padded);
    }

    TagSystemInfo systemInfo;
    QByteArray storage;
    bool corruptAfterWrite = false;
    bool reportWriteFailureAfterStoring = false;
    int readCallCount = 0;

    RfidDeviceResult setAccessAntenna(quint32) override { return success(); }
    RfidDeviceResult connectIso15693(quint32, const QString &) override { return success(); }
    RfidDeviceResult disconnectTag() override { return success(); }
    RfidDeviceResult getSystemInfo(TagSystemInfo *info) override
    {
        *info = systemInfo;
        return success();
    }
    RfidDeviceResult readBlocks(int basicIndex, int blockCount, int blockSize, QByteArray *data) override
    {
        ++readCallCount;
        const int offset = basicIndex * blockSize;
        const int size = blockCount * blockSize;
        if (offset < 0 || size <= 0 || storage.size() < offset + size)
            return failure();
        *data = storage.mid(offset, size);
        return success();
    }
    RfidDeviceResult writeBlocks(int basicIndex, int blockCount, int blockSize, const QByteArray &data) override
    {
        if (basicIndex != 0 || data.size() != blockCount * blockSize)
            return failure();
        storage.replace(0, data.size(), data);
        if (corruptAfterWrite && storage.size() > TagPayloadCodec::HeaderLength)
            storage[TagPayloadCodec::HeaderLength] = 'X';
        if (reportWriteFailureAfterStoring)
            return failure();
        return success();
    }

private:
    static RfidDeviceResult success()
    {
        RfidDeviceResult result;
        result.success = true;
        return result;
    }
    static RfidDeviceResult failure()
    {
        RfidDeviceResult result;
        result.message = QStringLiteral("Unexpected fake device operation.");
        return result;
    }
};

class RfidTagServiceTest : public QObject
{
    Q_OBJECT

private slots:
    void identifiesStableTag();
    void rejectsMultipleTags();
    void rejectsInconsistentObservations();
    void createsBlockWritePlan();
    void rejectsInsufficientCapacity();
    void requiresInitializedReader();
    void readsUnrecognizedFormatAsRawData();
    void readsLegacyContentLengthPayload();
    void readsOnlyBusinessPayloadBlocks();
    void writesAndVerifiesPayload();
    void writesToBlankTag();
    void confirmsWriteAfterTransportFailure();
    void detectsVerificationMismatch();
};

void RfidTagServiceTest::identifiesStableTag()
{
    InventoryObservation tag;
    tag.uid = QStringLiteral("E004010203040506");
    tag.airProtocol = 2;
    tag.tagType = 100;
    tag.antenna = 1;
    tag.rssi = 42;

    const StableInventoryResult result = RfidTagService::evaluateInventoryWindow({tag, tag});
    QCOMPARE(static_cast<int>(result.state), static_cast<int>(TagStabilityState::TagStable));
    QCOMPARE(result.appearanceCount, 2);
    QCOMPARE(result.tag.uid, tag.uid);
}

void RfidTagServiceTest::rejectsMultipleTags()
{
    InventoryObservation first;
    first.uid = QStringLiteral("E004010203040506");
    first.airProtocol = 2;
    first.antenna = 1;
    InventoryObservation second = first;
    second.uid = QStringLiteral("E004010203040507");

    const StableInventoryResult result = RfidTagService::evaluateInventoryWindow({first, second});
    QCOMPARE(static_cast<int>(result.state), static_cast<int>(TagStabilityState::TagNotUnique));
    QCOMPARE(static_cast<int>(result.errorKind), static_cast<int>(RfidErrorKind::TagNotUnique));
}

void RfidTagServiceTest::rejectsInconsistentObservations()
{
    InventoryObservation first;
    first.uid = QStringLiteral("E004010203040506");
    first.airProtocol = 2;
    first.antenna = 1;
    InventoryObservation second = first;
    second.antenna = 2;

    const StableInventoryResult result = RfidTagService::evaluateInventoryWindow({first, second});
    QCOMPARE(static_cast<int>(result.state), static_cast<int>(TagStabilityState::TagDetected));
}

void RfidTagServiceTest::createsBlockWritePlan()
{
    TagSystemInfo info;
    info.blockSize = 4;
    info.blockCount = 2;
    BlockWritePlan plan;
    RfidOperationResult failure;

    QVERIFY(RfidTagService::createWritePlan(QByteArray::fromHex("0102030405"), info, &plan, &failure));
    QCOMPARE(plan.blockCount, 2);
    QCOMPARE(plan.paddedData, QByteArray::fromHex("0102030405000000"));
}

void RfidTagServiceTest::rejectsInsufficientCapacity()
{
    TagSystemInfo info;
    info.blockSize = 4;
    info.blockCount = 1;
    BlockWritePlan plan;
    RfidOperationResult failure;

    QVERIFY(!RfidTagService::createWritePlan(QByteArray::fromHex("0102030405"), info, &plan, &failure));
    QCOMPARE(static_cast<int>(failure.errorKind), static_cast<int>(RfidErrorKind::CapacityExceeded));
}

void RfidTagServiceTest::requiresInitializedReader()
{
    InventoryObservation tag;
    tag.uid = QStringLiteral("E004010203040506");
    const RfidOperationResult result = RfidTagService::readPayload(nullptr, tag);
    QVERIFY(!result.success);
    QCOMPARE(static_cast<int>(result.errorKind), static_cast<int>(RfidErrorKind::ReaderNotReady));
}

void RfidTagServiceTest::readsUnrecognizedFormatAsRawData()
{
    InventoryObservation tag;
    tag.uid = QStringLiteral("E004010203040506");
    tag.tagType = 100;
    tag.antenna = 1;
    FakeDeviceAdapter device;
    device.storage[0] = char(0x02);
    device.storage[1] = char(0x0e);
    device.storage[3] = char(0xfa);

    const RfidOperationResult result = RfidTagService::readPayload(&device, tag);
    QVERIFY2(result.success, qPrintable(result.message));
    QVERIFY(!result.payloadDecoded);
    QCOMPARE(result.rawData, device.storage);
}

void RfidTagServiceTest::readsLegacyContentLengthPayload()
{
    InventoryObservation tag;
    tag.uid = QStringLiteral("E004010203040506");
    tag.tagType = 100;
    tag.antenna = 1;
    FakeDeviceAdapter device;
    device.storage.fill('\0');
    const QByteArray legacy = QByteArray::fromHex("011801230131174320202020D5C5C8FD32363738343533313031");
    device.storage.replace(0, legacy.size(), legacy);

    const RfidOperationResult result = RfidTagService::readPayload(&device, tag);
    QVERIFY2(result.success, qPrintable(result.message));
    QVERIFY(result.payloadDecoded);
    QCOMPARE(result.payload.medicalRecordNumber, QByteArray("2678453101"));
}

void RfidTagServiceTest::readsOnlyBusinessPayloadBlocks()
{
    InventoryObservation tag;
    tag.uid = QStringLiteral("E004010203040506");
    tag.tagType = 100;
    tag.antenna = 1;
    FakeDeviceAdapter device;
    device.storage.fill('\0');

    TagPayload payload;
    payload.dishNumber = 1;
    payload.inseminationTime = QDateTime(QDate(2026, 8, 5), QTime(11, 7));
    payload.femaleName = QStringLiteral("苏燕");
    payload.medicalRecordNumber = "11103";
    QByteArray encoded;
    QVERIFY(TagPayloadCodec::encode(payload, &encoded));
    device.storage.replace(0, encoded.size(), encoded);
    device.readCallCount = 0;

    const RfidOperationResult result = RfidTagService::readPayload(&device, tag);
    QVERIFY2(result.success, qPrintable(result.message));
    QCOMPARE(result.rawData, encoded);
    QCOMPARE(device.readCallCount, 2);
}

void RfidTagServiceTest::writesAndVerifiesPayload()
{
    InventoryObservation tag;
    tag.uid = QStringLiteral("E004010203040506");
    tag.tagType = 100;
    tag.antenna = 1;
    TagPayload payload;
    payload.dishNumber = 1;
    payload.inseminationTime = QDateTime(QDate(2023, 1, 31), QTime(17, 43));
    payload.femaleName = QStringLiteral("A");
    payload.medicalRecordNumber = "MR-001";
    FakeDeviceAdapter device;

    const RfidOperationResult result = RfidTagService::writePayload(&device, tag, payload);
    QVERIFY2(result.success, qPrintable(result.message));
    QCOMPARE(result.payload.medicalRecordNumber, payload.medicalRecordNumber);
}

void RfidTagServiceTest::writesToBlankTag()
{
    InventoryObservation tag;
    tag.uid = QStringLiteral("E004010203040506");
    tag.tagType = 100;
    tag.antenna = 1;
    TagPayload payload;
    payload.dishNumber = 2;
    payload.inseminationTime = QDateTime(QDate(2024, 2, 29), QTime(8, 30));
    payload.femaleName = QStringLiteral("A");
    payload.medicalRecordNumber = "NEW-001";
    FakeDeviceAdapter device;
    device.storage.fill('\0');

    const RfidOperationResult result = RfidTagService::writePayload(&device, tag, payload);
    QVERIFY2(result.success, qPrintable(result.message));
    QCOMPARE(result.payload.medicalRecordNumber, payload.medicalRecordNumber);
}

void RfidTagServiceTest::confirmsWriteAfterTransportFailure()
{
    InventoryObservation tag;
    tag.uid = QStringLiteral("E004010203040506");
    tag.tagType = 100;
    tag.antenna = 1;
    TagPayload payload;
    payload.dishNumber = 3;
    payload.inseminationTime = QDateTime(QDate(2024, 3, 1), QTime(9, 15));
    payload.femaleName = QStringLiteral("A");
    payload.medicalRecordNumber = "SAFE-001";
    FakeDeviceAdapter device;
    device.reportWriteFailureAfterStoring = true;

    const RfidOperationResult result = RfidTagService::writePayload(&device, tag, payload);
    QVERIFY2(result.success, qPrintable(result.message));
    QCOMPARE(result.payload.medicalRecordNumber, payload.medicalRecordNumber);
}

void RfidTagServiceTest::detectsVerificationMismatch()
{
    InventoryObservation tag;
    tag.uid = QStringLiteral("E004010203040506");
    tag.tagType = 100;
    tag.antenna = 1;
    TagPayload payload;
    payload.dishNumber = 1;
    payload.inseminationTime = QDateTime(QDate(2023, 1, 31), QTime(17, 43));
    payload.femaleName = QStringLiteral("A");
    payload.medicalRecordNumber = "MR-001";
    FakeDeviceAdapter device;
    device.corruptAfterWrite = true;

    const RfidOperationResult result = RfidTagService::writePayload(&device, tag, payload);
    QVERIFY(!result.success);
    QCOMPARE(static_cast<int>(result.errorKind), static_cast<int>(RfidErrorKind::VerifyFailed));
}

QTEST_APPLESS_MAIN(RfidTagServiceTest)

#include "tst_rfidtagservice.moc"
