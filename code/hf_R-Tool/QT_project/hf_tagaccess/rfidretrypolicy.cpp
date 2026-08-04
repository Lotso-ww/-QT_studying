#include "rfidretrypolicy.h"

bool RfidRetryPolicy::isRetryable(RfidErrorKind errorKind)
{
    switch (errorKind) {
    case RfidErrorKind::ReaderNotReady:
    case RfidErrorKind::InventoryTimeout:
    case RfidErrorKind::TagNotUnique:
    case RfidErrorKind::TagConnectFailed:
    case RfidErrorKind::ReadFailed:
    case RfidErrorKind::WriteFailed:
    case RfidErrorKind::VerifyFailed:
        return true;
    case RfidErrorKind::None:
    case RfidErrorKind::InputInvalid:
    case RfidErrorKind::UnsupportedFormat:
    case RfidErrorKind::InvalidPayload:
    case RfidErrorKind::CapacityExceeded:
    case RfidErrorKind::Cancelled:
        return false;
    }
    return false;
}

bool RfidRetryPolicy::shouldRetry(RfidErrorKind errorKind, int failedAttempt)
{
    return failedAttempt >= 1 && failedAttempt < MaxAttempts && isRetryable(errorKind);
}

int RfidRetryPolicy::delayBeforeRetryMs(int failedAttempt)
{
    if (failedAttempt == 1)
        return 300;
    if (failedAttempt == 2)
        return 500;
    return 0;
}
