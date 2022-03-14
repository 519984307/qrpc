#include "./qrpc_listen.h"
#include "./qrpc_listen_colletion.h"
#include "./qrpc_listen_request_cache.h"
#include <QDateTime>
#include <QProcess>
#include <QCoreApplication>
#include <QCryptographicHash>

namespace QRpc {

#define dPvt()\
auto&p =*reinterpret_cast<ListenPvt*>(this->p)

typedef QHash<int, QPair<int, const QMetaObject*>> MetaObjectVector;
Q_GLOBAL_STATIC(MetaObjectVector, staticListenInstalled);
Q_GLOBAL_STATIC(QByteArray, baseUuid)

static void init()
{
    QProcess process;
    auto bytes=process.environment().join(qsl(",")).toUtf8()+QDateTime::currentDateTime().toString().toUtf8();
    *baseUuid=QCryptographicHash::hash(bytes, QCryptographicHash::Md5).toHex();
}

Q_COREAPP_STARTUP_FUNCTION(init)

class ListenPvt:public QObject{
public:
    QUuid uuid;
    Listen*listenPool=nullptr;
    ListenRequestCache cacheRequest;
    ListenColletions*collections=nullptr;
    Server*server=nullptr;
    explicit ListenPvt(Listen*parent):QObject(parent), cacheRequest(parent)
    {
        this->uuid=QUuid::createUuidV5(QUuid::createUuid(), *baseUuid);
    }

    virtual ~ListenPvt()
    {
    }

    Listen*listen()
    {
        auto listen=dynamic_cast<Listen*>(this->parent());
        return listen;
    }
};

Listen::Listen(QObject *parent):QThread(nullptr)
{
    Q_UNUSED(parent)
    this->p = new ListenPvt(this);
}

Listen::~Listen()
{
    dPvt();delete&p;
}

int Listen::install(const QVariant &type, const QMetaObject &metaObject)
{
    const auto itype=type.toInt();
    if(!staticListenInstalled->contains(itype)){
#if Q_RPC_LOG_VERBOSE
        if(staticListenInstalled->isEmpty())
            sInfo()<<qsl("interface registered: ")<<metaObject.className();
        qInfo()<<qbl("interface: ")+metaObject.className();
#endif
        QPair<int, const QMetaObject*> pair(itype, &metaObject);
        staticListenInstalled->insert(itype, pair);
    }
    return staticListenInstalled->contains(itype);
}

QVector<QPair<int, const QMetaObject*> > Listen::listenList()
{
    QVector<QPair<int, const QMetaObject*> > __return;
    QHashIterator <int, QPair<int, const QMetaObject*>> i(*staticListenInstalled);
    while(i.hasNext()){
        i.next();
        __return<<i.value();
    }
    return __return;
}


QUuid &Listen::uuid() const
{
    dPvt();
    return p.uuid;
}

QObject *Listen::parent()
{
    return QThread::parent();
}

void Listen::setParent(QObject *parent)
{
    QThread::setParent(parent);
}

void Listen::run()
{
    this->exec();
}

bool Listen::start()
{
    QThread::start();
    while(this->eventDispatcher()==nullptr)
        QThread::msleep(1);
    return true;
}

bool Listen::stop()
{
    if(!this->isRunning())
        return true;
    QThread::quit();
    while(this->eventDispatcher()==nullptr)
        QThread::msleep(1);
    if(this->wait(1000))
        return true;
    return false;
}

Server *Listen::server()
{
    dPvt();
    return p.server;
}

ListenColletions *Listen::colletions()
{
    dPvt();
    return p.collections;
}

ListenRequestCache *Listen::cacheRequest()
{
    dPvt();
    return&p.cacheRequest;
}

void Listen::registerListenPool(Listen *listenPool)
{
    dPvt();
    p.listenPool=listenPool;
}

Listen &Listen::listenPool()
{
    dPvt();
    return*p.listenPool;
}

void Listen::setServer(Server *server)
{
    dPvt();
    p.server=server;
}

void Listen::setColletions(ListenColletions *colletions)
{
    dPvt();
    p.collections=colletions;
}

}
