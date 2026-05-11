#ifndef THREAD_H
#define THREAD_H

#include <QWidget>
#include <QThread>

class Thread : public QThread
{
    Q_OBJECT
public:
    Thread();
    // 重要的目的就是重写父类的run
    void run();
signals:
    void notify();
};

#endif // THREAD_H
