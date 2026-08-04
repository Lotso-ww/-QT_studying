#include "rfidbusinessworker.h"

#include "rfiddeviceadapter.h"
#include "rfidoperationrunner.h"

#include <QEventLoop>
#include <QTimer>

namespace {
RfidOperationResult runWithRetry(RfidBusinessWorker *worker, RfidOperationRunner **runnerSlot,
                                 const RfidOperationRunner::Operation &operation,
                                 bool retryEnabled = true)
{
    RfidOperationRunner runner;
    *runnerSlot = &runner;
    RfidOperationResult result;
    QEventLoop loop;
    QObject::connect(&runner, &RfidOperationRunner::attemptStarted, worker,
                     &RfidBusinessWorker::attemptStarted);
    QObject::connect(&runner, &RfidOperationRunner::retryScheduled, worker,
                     &RfidBusinessWorker::retryScheduled);
    QObject::connect(&runner, &RfidOperationRunner::finished, &loop,
                     [&](const RfidOperationResult &value) { result = value; loop.quit(); });
    // start() can finish synchronously. Schedule it after loop.exec() begins so
    // the finished signal cannot be emitted before the local event loop is active.
    QTimer::singleShot(0, &runner, [&runner, operation, retryEnabled]() {
        runner.start(operation, retryEnabled);
    });
    loop.exec();
    *runnerSlot = nullptr;
    return result;
}
}

RfidBusinessWorker::RfidBusinessWorker(QObject *parent)
    : QObject(parent)
{
}

void RfidBusinessWorker::read(void *reader, void *connectedTag, const InventoryObservation &tag)
{
    m_cancelRequested = false;
    emit completed(runWithRetry(this, &m_runner, [this, reader, connectedTag, tag]() {
        CaeRfidDeviceAdapter device(reader, connectedTag);
        emit deviceStage(QStringLiteral("TAG_SESSION"), QStringLiteral("Using the connected tag session."));
        const RfidOperationResult result = RfidTagService::readPayload(&device, tag);
        emit deviceStage(QStringLiteral("TAG_SESSION"), QStringLiteral("Business access finished; tag session remains connected."));
        return result;
    }));
}

void RfidBusinessWorker::write(void *reader, void *connectedTag, const InventoryObservation &tag, const TagPayload &payload)
{
    m_cancelRequested = false;
    emit completed(runWithRetry(this, &m_runner, [this, reader, connectedTag, tag, payload]() {
        CaeRfidDeviceAdapter device(reader, connectedTag);
        emit deviceStage(QStringLiteral("TAG_SESSION"), QStringLiteral("Using the connected tag session."));
        const RfidOperationResult result = RfidTagService::writePayload(&device, tag, payload);
        emit deviceStage(QStringLiteral("TAG_SESSION"), QStringLiteral("Business access finished; tag session remains connected."));
        return result;
    }, false));
}

void RfidBusinessWorker::cancel()
{
    requestCancel();
    if (m_runner)
        m_runner->cancel();
}

void RfidBusinessWorker::requestCancel()
{
    m_cancelRequested = true;
}
