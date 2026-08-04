#ifndef RFIDOPERATIONRUNNER_H
#define RFIDOPERATIONRUNNER_H

#include "rfidtagtypes.h"

#include <QObject>
#include <functional>

class RfidOperationRunner : public QObject
{
    Q_OBJECT

public:
    using Operation = std::function<RfidOperationResult()>;

    explicit RfidOperationRunner(QObject *parent = nullptr);

    bool isRunning() const;
    void start(const Operation &operation, bool retryEnabled = true);
    void cancel();

signals:
    void attemptStarted(int attempt);
    void retryScheduled(int failedAttempt, int delayMs);
    void finished(const RfidOperationResult &result);

private:
    void executeAttempt();

    Operation m_operation;
    int m_attempt = 0;
    bool m_running = false;
    bool m_cancelled = false;
    bool m_retryEnabled = true;
};

#endif // RFIDOPERATIONRUNNER_H
