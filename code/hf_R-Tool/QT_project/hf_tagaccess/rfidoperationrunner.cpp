#include "rfidoperationrunner.h"

#include "rfidretrypolicy.h"

#include <QTimer>

RfidOperationRunner::RfidOperationRunner(QObject *parent)
    : QObject(parent)
{
}

bool RfidOperationRunner::isRunning() const
{
    return m_running;
}

void RfidOperationRunner::start(const Operation &operation, bool retryEnabled)
{
    if (m_running || !operation)
        return;
    m_operation = operation;
    m_attempt = 0;
    m_cancelled = false;
    m_retryEnabled = retryEnabled;
    m_running = true;
    executeAttempt();
}

void RfidOperationRunner::cancel()
{
    if (!m_running)
        return;
    m_cancelled = true;
}

void RfidOperationRunner::executeAttempt()
{
    if (!m_running)
        return;
    if (m_cancelled) {
        RfidOperationResult result;
        result.errorKind = RfidErrorKind::Cancelled;
        result.attemptCount = m_attempt;
        result.message = QStringLiteral("Operation cancelled.");
        m_running = false;
        emit finished(result);
        return;
    }

    ++m_attempt;
    emit attemptStarted(m_attempt);
    RfidOperationResult result = m_operation();
    result.attemptCount = m_attempt;
    if (result.success || !m_retryEnabled
            || !RfidRetryPolicy::shouldRetry(result.errorKind, m_attempt)) {
        m_running = false;
        emit finished(result);
        return;
    }

    const int delayMs = RfidRetryPolicy::delayBeforeRetryMs(m_attempt);
    emit retryScheduled(m_attempt, delayMs);
    QTimer::singleShot(delayMs, this, [this]() { executeAttempt(); });
}
