#include <QtTest>

#include "rfidoperationrunner.h"

class RfidOperationRunnerTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void retriesAsynchronously();
    void stopsAfterThirdAttempt();
    void canCancelPendingRetry();
};

void RfidOperationRunnerTest::initTestCase()
{
    qRegisterMetaType<RfidOperationResult>();
}

void RfidOperationRunnerTest::retriesAsynchronously()
{
    RfidOperationRunner runner;
    QSignalSpy finishedSpy(&runner, &RfidOperationRunner::finished);
    QSignalSpy retrySpy(&runner, &RfidOperationRunner::retryScheduled);
    int calls = 0;
    runner.start([&calls]() {
        ++calls;
        RfidOperationResult result;
        if (calls == 3) {
            result.success = true;
            result.errorKind = RfidErrorKind::None;
        } else {
            result.errorKind = RfidErrorKind::ReadFailed;
            result.message = QStringLiteral("temporary failure");
        }
        return result;
    });

    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(calls, 3);
    QCOMPARE(retrySpy.count(), 2);
    const RfidOperationResult result = qvariant_cast<RfidOperationResult>(finishedSpy.at(0).at(0));
    QVERIFY(result.success);
    QCOMPARE(result.attemptCount, 3);
}

void RfidOperationRunnerTest::stopsAfterThirdAttempt()
{
    RfidOperationRunner runner;
    QSignalSpy finishedSpy(&runner, &RfidOperationRunner::finished);
    int calls = 0;
    runner.start([&calls]() {
        ++calls;
        RfidOperationResult result;
        result.errorKind = RfidErrorKind::WriteFailed;
        return result;
    });

    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(calls, 3);
    const RfidOperationResult result = qvariant_cast<RfidOperationResult>(finishedSpy.at(0).at(0));
    QVERIFY(!result.success);
    QCOMPARE(result.attemptCount, 3);
}

void RfidOperationRunnerTest::canCancelPendingRetry()
{
    RfidOperationRunner runner;
    QSignalSpy finishedSpy(&runner, &RfidOperationRunner::finished);
    int calls = 0;
    runner.start([&calls]() {
        ++calls;
        RfidOperationResult result;
        result.errorKind = RfidErrorKind::VerifyFailed;
        return result;
    });
    QVERIFY(runner.isRunning());
    runner.cancel();

    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(calls, 1);
    const RfidOperationResult result = qvariant_cast<RfidOperationResult>(finishedSpy.at(0).at(0));
    QCOMPARE(static_cast<int>(result.errorKind), static_cast<int>(RfidErrorKind::Cancelled));
}

QTEST_GUILESS_MAIN(RfidOperationRunnerTest)

#include "tst_rfidoperationrunner.moc"
