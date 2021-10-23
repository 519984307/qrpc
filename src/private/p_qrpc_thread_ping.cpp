#include "./p_qrpc_thread_ping.h"
#include <QTimer>

namespace QRpc {


#define dPvt()\
    auto&p = *reinterpret_cast<ThreadPingPvt*>(this->p)

class ThreadPingPvt{
public:
    int interval=1000;
    explicit ThreadPingPvt(){
    }
    virtual ~ThreadPingPvt(){
    }
};

ThreadPing::ThreadPing(const int interval):QThread(nullptr){
    this->p = new ThreadPing();
    dPvt();
    p.interval=interval;
}

ThreadPing::~ThreadPing(){
    dPvt();
    delete&p;
}

ThreadPing *ThreadPing::start(const int interval)
{
    QThread*r=new ThreadPing(interval);
    r->start();
    while(r->eventDispatcher()==nullptr)
        QThread::msleep(1);
    return dynamic_cast<ThreadPing*>(r);
}

void ThreadPing::run(){
    QTimer*timer=nullptr;
    auto doRun=[&timer, this](){
        timer=new QTimer(nullptr);
        timer->setInterval(this->interval());
        connect(timer, &QTimer::timeout, this, &ThreadPing::ping);
        timer->start();
    };
    QTimer::singleShot(1, this, doRun);
    this->exec();
    timer->stop();
    timer->deleteLater();
}

int ThreadPing::interval() const
{
    dPvt();
    return p.interval;
}

void ThreadPing::setInterval(int value)
{
    dPvt();
    p.interval=(value<=0)?1000:value;
}

ThreadPing &ThreadPing::on(ThreadPingMethod method){
    connect(this, &ThreadPing::ping, method);
    return*this;
}

}
