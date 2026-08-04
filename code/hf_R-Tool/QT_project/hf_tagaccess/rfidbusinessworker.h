#ifndef RFIDBUSINESSWORKER_H
#define RFIDBUSINESSWORKER_H

#include "rfidtagservice.h"
#include "rfidoperationrunner.h"

#include <QObject>
#include <atomic>

class RfidBusinessWorker : public QObject
{
    Q_OBJECT
public:
    explicit RfidBusinessWorker(QObject *parent = nullptr);
    void read(void *reader, void *connectedTag, const InventoryObservation &tag);
    void write(void *reader, void *connectedTag, const InventoryObservation &tag, const TagPayload &payload);
    void cancel();
    void requestCancel();

signals:
    void attemptStarted(int attempt);
    void retryScheduled(int failedAttempt, int delayMs);
    void deviceStage(const QString &stage, const QString &message);
    void completed(const RfidOperationResult &result);

private:
    RfidOperationRunner *m_runner = nullptr;
    std::atomic_bool m_cancelRequested {false};
};

#endif // RFIDBUSINESSWORKER_H
