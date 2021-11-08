#include "./qrpc_listen_request_cache.h"
#include "./qrpc_listen.h"
#include <QMutex>
#include <QUuid>
#include <QMap>
#include <QJsonDocument>

namespace QRpc {

#define dPvt()\
    auto&p =*reinterpret_cast<QRPCListenRequestCachePvt*>(this->p)

class QRPCListenRequestCachePvt:public QObject{
public:
    QMutex lock;
    QHash<QString, QRPCListenRequest*> cache;
    QRPCListen*listen(){
        auto listen=dynamic_cast<QRPCListen*>(this->parent()->parent());
        return listen;

    };
    explicit QRPCListenRequestCachePvt(QObject*parent):QObject(parent){
    }
    virtual ~QRPCListenRequestCachePvt(){
        this->clear();
    }

    void clear(){
        QMutexLocker<QMutex> locker(&this->lock);
        auto aux=this->cache;
        this->cache.clear();
        qDeleteAll(aux);
    }
};

QRPCListenRequestCache::QRPCListenRequestCache(QRPCListen *parent):QObject(parent)
{
    this->p=new QRPCListenRequestCachePvt(this);
}

QRPCListenRequestCache::~QRPCListenRequestCache()
{
    dPvt();delete&p;
}

void QRPCListenRequestCache::clear()
{
    dPvt();
    p.clear();
}

QRPCListenRequest &QRPCListenRequestCache::toRequest(const QUuid &uuid)
{
    dPvt();
    QMutexLocker<QMutex> locker(&p.lock);
    static QRPCListenRequest ___QRPCListenRequest;
    auto request=p.cache.value(uuid.toString());
    if(request!=nullptr)
        return*request;
    else
        return ___QRPCListenRequest;
}

QRPCListenRequest &QRPCListenRequestCache::createRequest()
{
    QVariant vRequest;
    return this->createRequest(vRequest);
}

QRPCListenRequest &QRPCListenRequestCache::createRequest(const QVariant &vRequest)
{
    dPvt();
    QMutexLocker<QMutex> locker(&p.lock);
    auto request = new QRPCListenRequest(vRequest);
    request->setListenUuid(p.listen()->uuid());
    if(request->isEmpty() || !request->isValid())
        request->setRequestBody(vRequest);

    auto uuid=request->requestUuid().isNull()?request->makeUuid():request->requestUuid();
    request->setRequestUuid(uuid);
    p.cache.insert(uuid.toString(), request);
    return*request;
}

}
