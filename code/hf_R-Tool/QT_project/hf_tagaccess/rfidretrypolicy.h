#ifndef RFIDRETRYPOLICY_H
#define RFIDRETRYPOLICY_H

#include "rfidtagtypes.h"

class RfidRetryPolicy
{
public:
    static const int MaxAttempts = 3;

    static bool isRetryable(RfidErrorKind errorKind);
    static bool shouldRetry(RfidErrorKind errorKind, int failedAttempt);
    static int delayBeforeRetryMs(int failedAttempt);
};

#endif // RFIDRETRYPOLICY_H
