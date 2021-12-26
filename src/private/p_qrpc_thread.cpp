#include "./p_qrpc_thread.h"
#include <QMutex>
#include <QTimer>

namespace QRpc {

#define dPvt()\
    auto&p =*reinterpret_cast<QRPCThreadPvt*>(this->p)

class QRPCThreadPvt{
public:
    QMutex lockRunWait;
    QMutex mutexRunning;
    explicit QRPCThreadPvt(){
    }

    virtual ~QRPCThreadPvt(){
    }

};

QRPCThread::QRPCThread(QObject *parent):QThread(nullptr)
{
    Q_UNUSED(parent)
    this->p = new QRPCThreadPvt();
}

QRPCThread::~QRPCThread()
{
    dPvt();delete&p;
}

void QRPCThread::run()
{
    dPvt();
    if(p.mutexRunning.tryLock(1000)){
        QTimer::singleShot(10, this, &QRPCThread::eventRun);
        this->exec();
        p.mutexRunning.unlock();
    }
}

void QRPCThread::eventRun()
{
    QThread::sleep(1);
    this->quit();
}

bool QRPCThread::start()
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
    else{
        return false;
    }
}

bool QRPCThread::stop()
{
    return this->quit();
}

bool QRPCThread::quit()
{
    QThread::quit();
    QThread::wait();
    return true;
}

}
