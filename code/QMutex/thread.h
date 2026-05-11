#ifndef THREAD_H
#define THREAD_H

#include <QWidget>
#include <QThread>
#include <QMutex>

class Thread : public QThread
{
    Q_OBJECT
public:
    Thread();
    void run();

    // 我们要设置成静态变量, 让多个线程都可以看到
    static int num;
    static QMutex mutex; // 锁
};

#endif // THREAD_H
