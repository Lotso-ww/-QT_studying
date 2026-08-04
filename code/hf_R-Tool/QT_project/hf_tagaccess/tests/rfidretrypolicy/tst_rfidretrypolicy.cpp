#include <QtTest>

#include "rfidretrypolicy.h"

class RfidRetryPolicyTest : public QObject
{
    Q_OBJECT

private slots:
    void retryableErrors();
    void nonRetryableErrors();
    void stopsAtThirdAttempt();
    void usesSpecifiedDelays();
};

void RfidRetryPolicyTest::retryableErrors()
{
    QVERIFY(RfidRetryPolicy::isRetryable(RfidErrorKind::ReadFailed));
    QVERIFY(RfidRetryPolicy::isRetryable(RfidErrorKind::WriteFailed));
    QVERIFY(RfidRetryPolicy::isRetryable(RfidErrorKind::VerifyFailed));
    QVERIFY(RfidRetryPolicy::shouldRetry(RfidErrorKind::TagNotUnique, 1));
    QVERIFY(RfidRetryPolicy::shouldRetry(RfidErrorKind::TagConnectFailed, 2));
}

void RfidRetryPolicyTest::nonRetryableErrors()
{
    QVERIFY(!RfidRetryPolicy::isRetryable(RfidErrorKind::InputInvalid));
    QVERIFY(!RfidRetryPolicy::isRetryable(RfidErrorKind::InvalidPayload));
    QVERIFY(!RfidRetryPolicy::isRetryable(RfidErrorKind::CapacityExceeded));
    QVERIFY(!RfidRetryPolicy::shouldRetry(RfidErrorKind::InputInvalid, 1));
}

void RfidRetryPolicyTest::stopsAtThirdAttempt()
{
    QVERIFY(!RfidRetryPolicy::shouldRetry(RfidErrorKind::ReadFailed, 3));
    QVERIFY(!RfidRetryPolicy::shouldRetry(RfidErrorKind::ReadFailed, 0));
    QCOMPARE(RfidRetryPolicy::MaxAttempts, 3);
}

void RfidRetryPolicyTest::usesSpecifiedDelays()
{
    QCOMPARE(RfidRetryPolicy::delayBeforeRetryMs(1), 300);
    QCOMPARE(RfidRetryPolicy::delayBeforeRetryMs(2), 500);
    QCOMPARE(RfidRetryPolicy::delayBeforeRetryMs(3), 0);
}

QTEST_APPLESS_MAIN(RfidRetryPolicyTest)

#include "tst_rfidretrypolicy.moc"
