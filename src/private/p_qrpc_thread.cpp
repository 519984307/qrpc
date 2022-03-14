#include "./p_qrpc_thread.h"
#include <QMutex>
#include <QTimer>

namespace QRpc {

#define dPvt()\
    auto&p =*reinterpret_cast<ThreadPvt*>(this->p)

class ThreadPvt{
public:
    QMutex lockRunWait;
    QMutex mutexRunning;

    explicit ThreadPvt()
    {
    }

    virtual ~ThreadPvt()
    {
    }

};

Thread::Thread(QObject *parent):QThread(nullptr)
{
    Q_UNUSED(parent)
    this->p = new ThreadPvt();
}

Thread::~Thread()
{
    dPvt();delete&p;
}

void Thread::run()
{
    dPvt();
    if(p.mutexRunning.tryLock(1000)){
        QTimer::singleShot(10, this, &Thread::eventRun);
        this->exec();
        p.mutexRunning.unlock();
    }
}

void Thread::eventRun()
{
    QThread::sleep(1);
    this->quit();
}

bool Thread::start()
{
    dPvt();
    if(p.mutexRunning.tryLock(1000)){
        p.lockRunWait.lock();
        QThread::start();
        while(this->eventDispatcher()==nullptr){
            QThread::msleep(1);
        }
        p.lockRunWait.lock();
        p.lockRunWait.unlock();
        return this->isRunning();
    }
    return false;
}

bool Thread::stop()
{
    return this->quit();
}

bool Thread::quit()
{
    QThread::quit();
    return true;
}

}
