#include "./qrpc_listen.h"
#include "./qrpc_listen_colletion.h"
#include "./qrpc_listen_request_cache.h"
#include <QDateTime>
#include <QProcess>
#include <QCryptographicHash>

namespace PrivateQRpc {
    Q_GLOBAL_STATIC(QByteArray, baseUuid);
}

namespace QRpc {

static auto&baseUuid=*PrivateQRpc::baseUuid;

#define dPvt()\
    auto&p =*reinterpret_cast<QRPCListenPvt*>(this->p)

class QRPCListenPvt:public QObject{
public:
    QUuid uuid;
    QRPCListen*listenPool=nullptr;
    QRPCListenRequestCache cacheRequest;
    explicit QRPCListenPvt(QRPCListen*parent):QObject(parent), cacheRequest(parent){
        if(baseUuid.isEmpty()){
            QProcess process;
            auto bytes=process.environment().join(qsl(",")).toUtf8()+QDateTime::currentDateTime().toString().toUtf8();
            baseUuid=QCryptographicHash::hash(bytes, QCryptographicHash::Md5).toHex();
        }
        auto uuidNs = QUuid::createUuid();
        this->uuid=QUuid::createUuidV5(uuidNs, baseUuid);
    }

    virtual ~QRPCListenPvt(){
    }

    QRPCListen*listen(){
        auto listen=dynamic_cast<QRPCListen*>(this->parent());
        return listen;
    }

    QRPCListenColletions*collections=nullptr;
    QRPCServer*server=nullptr;
};

QRPCListen::QRPCListen(QObject *parent):QThread(nullptr)
{
    Q_UNUSED(parent)
    this->p = new QRPCListenPvt(this);
}

QRPCListen::~QRPCListen()
{
    dPvt();delete&p;
}

QUuid QRPCListen::uuid() const
{
    dPvt();
    return p.uuid;
}

QObject *QRPCListen::parent()
{
    return QThread::parent();
}

void QRPCListen::setParent(QObject *parent)
{
    QThread::setParent(parent);
}

void QRPCListen::run()
{
    this->exec();
}

bool QRPCListen::start()
{
    QThread::start();
    while(this->eventDispatcher()==nullptr)
        QThread::msleep(1);
    return true;
}

bool QRPCListen::stop()
{
    if(this->isRunning()){
        QThread::quit();
        while(this->eventDispatcher()==nullptr)
            QThread::msleep(1);
        if(this->wait(1000))
            return true;
        else
            return false;
    }
    return true;
}

QRPCServer *QRPCListen::server()
{
    dPvt();
    return p.server;
}

QRPCListenColletions *QRPCListen::colletions()
{
    dPvt();
    return p.collections;
}

QRPCListenRequestCache *QRPCListen::cacheRequest()
{
    dPvt();
    return&p.cacheRequest;
}

void QRPCListen::registerListenPool(QRPCListen *listenPool)
{
    dPvt();
    p.listenPool=listenPool;
}

QRPCListen &QRPCListen::listenPool()
{
    dPvt();
    return*p.listenPool;
}

void QRPCListen::setServer(QRPCServer *server)
{
    dPvt();
    p.server=server;
}

void QRPCListen::setColletions(QRPCListenColletions *colletions)
{
    dPvt();
    p.collections=colletions;
}

}
