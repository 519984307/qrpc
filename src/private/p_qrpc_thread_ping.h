#pragma once

#include <QThread>
#include <QVariant>

namespace QRpc {

typedef std::function<void()> ThreadPingMethod;

class ThreadPing:public QThread{
    Q_OBJECT
public:

    Q_INVOKABLE explicit ThreadPing(const int interval=-1);

    Q_INVOKABLE virtual ~ThreadPing();

    static ThreadPing*start(const int interval=-1);

    void run();

    virtual int interval() const;
    virtual void setInterval(int value);

    ThreadPing&on(ThreadPingMethod method);

signals:
    void ping();
private:
    void*p=nullptr;
};

}
