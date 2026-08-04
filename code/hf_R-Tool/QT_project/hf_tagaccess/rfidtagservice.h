#ifndef RFIDTAGSERVICE_H
#define RFIDTAGSERVICE_H

#include "rfidtagtypes.h"
#include "rfiddeviceadapter.h"

#include <QVector>

enum class TagStabilityState {
    WaitingForTag,
    TagDetected,
    TagStable,
    TagNotUnique,
    TagTimeout,
    DeviceError,
};

struct InventoryObservation
{
    QString uid;
    quint32 airProtocol = 0;
    quint32 tagType = 0;
    quint32 antenna = 0;
    quint16 rssi = 0;
    bool deviceError = false;
    int sdkErrorCode = 0;
};

struct StableInventoryResult
{
    TagStabilityState state = TagStabilityState::WaitingForTag;
    RfidErrorKind errorKind = RfidErrorKind::None;
    int sdkErrorCode = 0;
    int appearanceCount = 0;
    InventoryObservation tag;
    QString message;
};

struct BlockWritePlan
{
    int blockCount = 0;
    QByteArray paddedData;
};

class RfidTagService
{
public:
    static const int StableAppearanceCount = 1;

    static StableInventoryResult evaluateInventoryWindow(const QVector<InventoryObservation> &observations);
    static bool createWritePlan(const QByteArray &payload, const TagSystemInfo &systemInfo,
                                BlockWritePlan *plan, RfidOperationResult *failure = nullptr);
    static RfidOperationResult readPayload(RfidDeviceAdapter *device, const InventoryObservation &tag);
    static RfidOperationResult writePayload(RfidDeviceAdapter *device, const InventoryObservation &tag,
                                            const TagPayload &payload);
};

#endif // RFIDTAGSERVICE_H
