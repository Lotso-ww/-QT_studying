#ifndef RFIDTAGTYPES_H
#define RFIDTAGTYPES_H

#include <QByteArray>
#include <QDateTime>
#include <QMetaType>
#include <QString>

struct TagPayload
{
    quint8 formatVersion = 1;
    quint8 dishNumber = 0;
    QDateTime inseminationTime;
    QString femaleName;
    QByteArray medicalRecordNumber;
};

enum class RfidErrorKind {
    None,
    InputInvalid,
    ReaderNotReady,
    InventoryTimeout,
    TagNotUnique,
    TagConnectFailed,
    ReadFailed,
    WriteFailed,
    VerifyFailed,
    UnsupportedFormat,
    InvalidPayload,
    CapacityExceeded,
    Cancelled,
};

struct RfidOperationResult
{
    bool success = false;
    // A read may succeed at the transport level even when its bytes use a legacy layout.
    bool payloadDecoded = false;
    RfidErrorKind errorKind = RfidErrorKind::None;
    int sdkErrorCode = 0;
    int attemptCount = 0;
    QString message;
    QByteArray rawData;
    TagPayload payload;
};

struct TagSystemInfo
{
    int blockSize = 0;
    int blockCount = 0;
};

struct RfidDeviceResult
{
    bool success = false;
    int sdkErrorCode = 0;
    QString message;
};

Q_DECLARE_METATYPE(RfidOperationResult)

#endif // RFIDTAGTYPES_H
