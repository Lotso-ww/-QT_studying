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

bool isMedicalRecordCharacter(char value)
{
    return (value >= 'A' && value <= 'Z') || (value >= 'a' && value <= 'z')
            || (value >= '0' && value <= '9') || value == '_' || value == '-';
}

bool hasLegacyMedicalRecordTail(const QByteArray &raw, int wholePayloadLength)
{
    const int tailLength = TagPayloadCodec::HeaderFieldLength;
    if (wholePayloadLength < 0 || raw.size() < wholePayloadLength + tailLength)
        return false;
    for (int index = 0; index < tailLength; ++index) {
        if (!isMedicalRecordCharacter(raw.at(wholePayloadLength + index)))
            return false;
    }
    return true;
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

    const qint64 capacity = static_cast<qint64>(systemInfo.blockSize) * systemInfo.blockCount;
    auto readRange = [&](int basicIndex, int blockCount, QByteArray *data) {
        return device->readBlocks(basicIndex, blockCount, systemInfo.blockSize, data);
    };

    // The length byte is in block 0. Reading the entire tag block by block made
    // a normal business read (and the write verification that follows it) slow
    // enough for a stationary tag session to time out on some RD5200 readers.
    QByteArray raw;
    deviceResult = readRange(0, 1, &raw);
    if (!deviceResult.success) {
        device->disconnectTag();
        return deviceFailureResult(deviceResult, RfidErrorKind::ReadFailed);
    }

    const int lengthField = raw.size() > 1 ? static_cast<quint8>(raw.at(1)) : 0;
    const int wholePayloadLength = lengthField;
    const int legacyPayloadLength = lengthField + TagPayloadCodec::HeaderFieldLength;
    const bool wholeLengthPossible = wholePayloadLength >= TagPayloadCodec::HeaderLength
            && wholePayloadLength <= capacity;
    const bool legacyLengthPossible = lengthField >= TagPayloadCodec::HeaderLength
            - TagPayloadCodec::HeaderFieldLength && legacyPayloadLength <= capacity;

    if (wholeLengthPossible || legacyLengthPossible) {
        const int longestCandidate = qMax(wholeLengthPossible ? wholePayloadLength : 0,
                                          legacyLengthPossible ? legacyPayloadLength : 0);
        const int blocksToRead = blocksForBytes(longestCandidate, systemInfo.blockSize);
        if (blocksToRead > 1) {
            QByteArray remainingData;
            deviceResult = readRange(1, blocksToRead - 1, &remainingData);
            if (!deviceResult.success) {
                device->disconnectTag();
                RfidOperationResult failure = deviceFailureResult(deviceResult, RfidErrorKind::ReadFailed);
                failure.rawData = raw;
                return failure;
            }
            raw.append(remainingData);
        }
    } else {
        // Preserve the existing raw-data fallback for tags that do not use the
        // business format, while avoiding that expensive full read for valid data.
        QByteArray remainingData;
        if (systemInfo.blockCount > 1)
            deviceResult = readRange(1, systemInfo.blockCount - 1, &remainingData);
        if (!deviceResult.success) {
            device->disconnectTag();
            RfidOperationResult failure = deviceFailureResult(deviceResult, RfidErrorKind::ReadFailed);
            failure.rawData = raw;
            return failure;
        }
        raw.append(remainingData);
    }

    if (raw == QByteArray(raw.size(), '\0')) {
        device->disconnectTag();
        RfidOperationResult result;
        result.rawData = raw;
        result.errorKind = RfidErrorKind::UnsupportedFormat;
        result.message = QStringLiteral("The tag is blank. Write business data to initialize it.");
        return result;
    }

    const bool legacyLengthHasRemainingRecordBytes = legacyPayloadLength <= raw.size()
            && hasLegacyMedicalRecordTail(raw, wholePayloadLength);
    const int totalPayloadLength = legacyLengthHasRemainingRecordBytes
            ? legacyPayloadLength : wholePayloadLength;
    if (totalPayloadLength < TagPayloadCodec::HeaderLength || totalPayloadLength > capacity) {
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
    // Clear the two bytes immediately after the declared payload whenever the
    // tag has room. Older firmware stored a content-only length and those stale
    // bytes could otherwise make a new full-length payload look like the legacy
    // layout during verification.
    QByteArray dataToWrite = encoded;
    const qint64 paddedCapacity = static_cast<qint64>(systemInfo.blockSize) * systemInfo.blockCount;
    if (dataToWrite.size() + TagPayloadCodec::HeaderFieldLength <= paddedCapacity)
        dataToWrite.append(QByteArray(TagPayloadCodec::HeaderFieldLength, '\0'));

    BlockWritePlan plan;
    RfidOperationResult failure;
    if (!createWritePlan(dataToWrite, systemInfo, &plan, &failure)) {
        device->disconnectTag();
        return failure;
    }
    deviceResult = device->writeBlocks(0, plan.blockCount, systemInfo.blockSize, plan.paddedData);
    device->disconnectTag();
    if (!deviceResult.success) {
        // A transport error can arrive after the reader has committed the write.
        // Read the tag once before reporting failure so a retry never blindly
        // writes the same payload over a successfully completed operation.
        RfidOperationResult confirmation = readPayload(device, tag);
        if (confirmation.success && confirmation.rawData.startsWith(encoded)) {
            confirmation.rawData = encoded;
            confirmation.payload = payload;
            confirmation.message = QStringLiteral("Tag payload was written and confirmed after a write transport error.");
            return confirmation;
        }
        return deviceFailureResult(deviceResult, RfidErrorKind::WriteFailed);
    }

    RfidOperationResult verification = readPayload(device, tag);
    if (!verification.success)
        return verification;
    if (!verification.rawData.startsWith(encoded)) {
        verification.success = false;
        verification.errorKind = RfidErrorKind::VerifyFailed;
        verification.message = QStringLiteral("Written payload does not match the verification read. expected=%1 actual=%2")
                .arg(TagPayloadCodec::toHex(encoded), TagPayloadCodec::toHex(verification.rawData));
        return verification;
    }
    verification.rawData = encoded;
    verification.payload = payload;
    verification.message = QStringLiteral("Tag payload written and verified successfully.");
    return verification;
}
