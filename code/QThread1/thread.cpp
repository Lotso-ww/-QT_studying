#include "thread.h"

Thread::Thread()
{

}

void Thread::run()
{
    // 我们只能在主线程里面对界面进行修改
    // 这里可以计时,然后通知子进程去对界面进行修改
    for(int i = 0; i < 10; i++)
    {
        sleep(1);
        notify();
    }
}
