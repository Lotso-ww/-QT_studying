#include "rfidtagservice.h"

#include "tagpayloadcodec.h"

#include <QHash>
#include <QSet>

namespace {
QString observationKey(const InventoryObservation &observation)
{
    return observation.uid + QLatin1Char('|') + QString::number(observation.airProtocol)
            + QLatin1Char('|') + QString::number(observation.antenna);
}

void setFailure(RfidOperationResult *failure, RfidErrorKind errorKind, const QString &message)
{
    if (!failure)
        return;
    failure->success = false;
    failure->errorKind = errorKind;
    failure->message = message;
}

RfidOperationResult deviceFailureResult(const RfidDeviceResult &deviceResult, RfidErrorKind errorKind)
{
    RfidOperationResult result;
    result.errorKind = errorKind;
    result.sdkErrorCode = deviceResult.sdkErrorCode;
    result.message = deviceResult.message;
    return result;
}

int blocksForBytes(int byteCount, int blockSize)
{
    return (byteCount + blockSize - 1) / blockSize;
}

RfidOperationResult rawDataReadResult(const QByteArray &raw, const QString &reason)
{
    RfidOperationResult result;
    result.success = true;
    result.rawData = raw;
    result.message = QStringLiteral("Tag data was read as raw data. %1").arg(reason);
    return result;
}
}

StableInventoryResult RfidTagService::evaluateInventoryWindow(const QVector<InventoryObservation> &observations)
{
    StableInventoryResult result;
    if (observations.isEmpty()) {
        result.state = TagStabilityState::TagTimeout;
        result.errorKind = RfidErrorKind::InventoryTimeout;
        result.message = QStringLiteral("No ISO15693 tag was found in the inventory window.");
        return result;
    }

    QHash<QString, int> appearances;
    QHash<QString, InventoryObservation> latestObservation;
    QSet<QString> uniqueUids;
    for (const InventoryObservation &observation : observations) {
        if (observation.deviceError) {
            result.state = TagStabilityState::DeviceError;
            result.errorKind = RfidErrorKind::ReadFailed;
            result.sdkErrorCode = observation.sdkErrorCode;
            result.message = QStringLiteral("Reader error during inventory.");
            return result;
        }
        if (observation.uid.isEmpty())
            continue;
        const QString key = observationKey(observation);
        ++appearances[key];
        latestObservation.insert(key, observation);
        uniqueUids.insert(observation.uid);
    }

    if (uniqueUids.size() > 1) {
        result.state = TagStabilityState::TagNotUnique;
        result.errorKind = RfidErrorKind::TagNotUnique;
        result.message = QStringLiteral("More than one tag UID was found in the inventory window.");
        return result;
    }
    if (appearances.isEmpty()) {
        result.state = TagStabilityState::TagTimeout;
        result.errorKind = RfidErrorKind::InventoryTimeout;
        result.message = QStringLiteral("Inventory returned no valid tag UID.");
        return result;
    }
    if (appearances.size() != 1) {
        result.state = TagStabilityState::TagDetected;
        result.message = QStringLiteral("The tag was observed with inconsistent antenna or protocol information.");
        return result;
    }

    const QString key = appearances.constBegin().key();
    result.appearanceCount = appearances.value(key);
    result.tag = latestObservation.value(key);
    if (result.appearanceCount >= StableAppearanceCount) {
        result.state = TagStabilityState::TagStable;
        result.message = QStringLiteral("A single tag was observed consistently.");
    } else {
        result.state = TagStabilityState::TagDetected;
        result.message = QStringLiteral("Tag detected; waiting for another matching observation.");
    }
    return result;
}

bool RfidTagService::createWritePlan(const QByteArray &payload, const TagSystemInfo &systemInfo,
                                     BlockWritePlan *plan, RfidOperationResult *failure)
{
    if (!plan || payload.isEmpty()) {
        setFailure(failure, RfidErrorKind::InvalidPayload, QStringLiteral("Payload and write plan are required."));
        return false;
    }
    if (systemInfo.blockSize <= 0 || systemInfo.blockCount <= 0) {
        setFailure(failure, RfidErrorKind::ReadFailed,
                   QStringLiteral("Tag system information does not contain a valid block size and count."));
        return false;
    }

    QByteArray padded;
    QString error;
    if (!TagPayloadCodec::padForBlocks(payload, systemInfo.blockSize, &padded, &error)) {
        setFailure(failure, RfidErrorKind::InvalidPayload, error);
        return false;
    }
    const qint64 capacity = static_cast<qint64>(systemInfo.blockSize) * systemInfo.blockCount;
    if (padded.size() > capacity) {
        setFailure(failure, RfidErrorKind::CapacityExceeded,
                   QStringLiteral("Payload exceeds the tag capacity."));
        return false;
    }

    plan->paddedData = padded;
    plan->blockCount = padded.size() / systemInfo.blockSize;
    return true;
}

RfidOperationResult RfidTagService::readPayload(RfidDeviceAdapter *device, const InventoryObservation &tag)
{
    if (!device) {
        RfidOperationResult result;
        result.errorKind = RfidErrorKind::ReaderNotReady;
        result.message = QStringLiteral("Reader is not initialized.");
        return result;
    }
    if (tag.uid.isEmpty()) {
        RfidOperationResult result;
        result.errorKind = RfidErrorKind::InventoryTimeout;
        result.message = QStringLiteral("A stable tag is required before reading.");
        return result;
    }

    RfidDeviceResult deviceResult = device->setAccessAntenna(tag.antenna);
    if (!deviceResult.success)
        return deviceFailureResult(deviceResult, RfidErrorKind::TagConnectFailed);

    deviceResult = device->connectIso15693(tag.tagType, tag.uid);
    if (!deviceResult.success)
        return deviceFailureResult(deviceResult, RfidErrorKind::TagConnectFailed);

    TagSystemInfo systemInfo;
    deviceResult = device->getSystemInfo(&systemInfo);
    if (!deviceResult.success) {
        device->disconnectTag();
        return deviceFailureResult(deviceResult, RfidErrorKind::ReadFailed);
    }

    QByteArray raw;
    for (int block = 0; block < systemInfo.blockCount; ++block) {
        QByteArray blockData;
        deviceResult = device->readBlocks(block, 1, systemInfo.blockSize, &blockData);
        if (!deviceResult.success) {
            device->disconnectTag();
            RfidOperationResult failure = deviceFailureResult(deviceResult, RfidErrorKind::ReadFailed);
            failure.rawData = raw;
            return failure;
        }
        raw.append(blockData);
    }

    if (raw == QByteArray(raw.size(), '\0')) {
        device->disconnectTag();
        RfidOperationResult result;
        result.rawData = raw;
        result.errorKind = RfidErrorKind::UnsupportedFormat;
        result.message = QStringLiteral("The tag is blank. Write business data to initialize it.");
        return result;
    }

    const int contentLength = raw.size() > 1 ? static_cast<quint8>(raw.at(1)) : 0;
    const int totalPayloadLength = contentLength + 2;
    const qint64 capacity = static_cast<qint64>(systemInfo.blockSize) * systemInfo.blockCount;
    if (contentLength < TagPayloadCodec::HeaderLength - 2 || totalPayloadLength > capacity) {
        device->disconnectTag();
        return rawDataReadResult(raw,
                                 QStringLiteral("The stored length does not match the current business format."));
    }
    device->disconnectTag();

    RfidOperationResult result;
    result.rawData = raw.left(totalPayloadLength);
    QString error;
    if (!TagPayloadCodec::decode(result.rawData, &result.payload, &error)) {
        return rawDataReadResult(raw, error);
    }
    result.success = true;
    result.payloadDecoded = true;
    result.message = QStringLiteral("Tag payload read successfully.");
    return result;
}

RfidOperationResult RfidTagService::writePayload(RfidDeviceAdapter *device, const InventoryObservation &tag,
                                                   const TagPayload &payload)
{
    QByteArray encoded;
    QString error;
    if (!TagPayloadCodec::encode(payload, &encoded, &error)) {
        RfidOperationResult result;
        result.errorKind = RfidErrorKind::InputInvalid;
        result.message = error;
        return result;
    }
    if (!device) {
        RfidOperationResult result;
        result.errorKind = RfidErrorKind::ReaderNotReady;
        result.message = QStringLiteral("Reader is not initialized.");
        return result;
    }

    RfidDeviceResult deviceResult = device->setAccessAntenna(tag.antenna);
    if (!deviceResult.success)
        return deviceFailureResult(deviceResult, RfidErrorKind::TagConnectFailed);
    deviceResult = device->connectIso15693(tag.tagType, tag.uid);
    if (!deviceResult.success)
        return deviceFailureResult(deviceResult, RfidErrorKind::TagConnectFailed);

    TagSystemInfo systemInfo;
    deviceResult = device->getSystemInfo(&systemInfo);
    if (!deviceResult.success) {
        device->disconnectTag();
        return deviceFailureResult(deviceResult, RfidErrorKind::ReadFailed);
    }
    BlockWritePlan plan;
    RfidOperationResult failure;
    if (!createWritePlan(encoded, systemInfo, &plan, &failure)) {
        device->disconnectTag();
        return failure;
    }
    deviceResult = device->writeBlocks(0, plan.blockCount, systemInfo.blockSize, plan.paddedData);
    device->disconnectTag();
    if (!deviceResult.success)
        return deviceFailureResult(deviceResult, RfidErrorKind::WriteFailed);

    RfidOperationResult verification = readPayload(device, tag);
    if (!verification.success)
        return verification;
    if (verification.rawData != encoded) {
        verification.success = false;
        verification.errorKind = RfidErrorKind::VerifyFailed;
        verification.message = QStringLiteral("Written payload does not match the verification read.");
        return verification;
    }
    verification.payload = payload;
    verification.message = QStringLiteral("Tag payload written and verified successfully.");
    return verification;
}
