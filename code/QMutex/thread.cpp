#include "thread.h"

int Thread::num = 0;
QMutex Thread::mutex;

Thread::Thread()
{

}

void Thread::run()
{
    for(int i = 0; i < 50000; i++)
    {
        mutex.lock();
        num++;
        mutex.unlock();
    }
}
