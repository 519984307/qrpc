#include "./p_qrpc_listen_qrpc.h"
#include "./p_qrpc_listen_qrpc_slot.h"
#include "../qrpc_listen_request.h"
#include "../qrpc_listen_request_parser.h"
#include "../qrpc_server.h"
#include "../qrpc_controller.h"
#include <QCryptographicHash>
#include <QMutex>
#include <QDebug>
#include <QMetaMethod>

namespace QRpc {

#define dPvt()\
    auto&p =*reinterpret_cast<QRPCListenQRPCPvt*>(this->p)

typedef QVector<QRPCListenQRPCSlot*> QRPCListenSlotList;
typedef QHash<QByteArray, QRPCListenSlotList*> QRPCListenSlotCache;

class QRPCListenQRPCPvt: public QObject{
public:
    QMutex mutexRunning;
    QMutex mutexMapLocker;
    QHash<QByteArray, const QMetaObject*> controller;
    QHash<QByteArray, const QMetaObject*> parsers;
    QRPCListenSlotCache listenSlotCache;
    QRPCListenQRPC*listenQRPC=nullptr;
    QMap<QUuid, QRPCListen*> listens;
    QRPCServer*server=nullptr;

    explicit QRPCListenQRPCPvt(QRPCListenQRPC*parent):QObject(parent)
    {
        this->listenQRPC = dynamic_cast<QRPCListenQRPC*>(this->parent());
        if(this->listenQRPC!=nullptr)
            this->server = this->listenQRPC->server();
    }
    virtual ~QRPCListenQRPCPvt()
    {
        auto list=listenSlotCache.values();
        listenSlotCache.clear();
        qDeleteAll(list);
    }

    void registerListen(QRPCListen*listen)
    {
        auto listenQRPC=this->listenQRPC;
        if(listen==listenQRPC)
            return;
        if(this->listens.contains(listen->uuid()))
            return;
        this->listens.insert(listen->uuid(), listen);
        listen->registerListenPool(listenQRPC);
        QObject::connect(listen, &QRPCListen::rpcRequest, this, &QRPCListenQRPCPvt::onRpcRequest);
    }

public slots:
    void onRpcRequest(QVariantHash vRequest, const QVariant&uploadedFiles){
        QRPCListenRequest request;
        if(!request.fromHash(vRequest)){
            if(request.co().isOK())
                request.co().setBadRequest();
            return;
        }

        if((server!=nullptr) && (!server->authorizationRequest(request))){
            if(request.co().isOK())
                request.co().setUnauthorized();
            return;
        }

        auto vMap=request.toHash();
        QRPCListenSlotList*listenSlotList=nullptr;
        {//locker
            QMutexLOCKER locker(&mutexMapLocker);
            auto md5=request.hash();
            listenSlotList=this->listenSlotCache.value(md5);
            if(listenSlotList==nullptr){
                listenSlotList = new QRPCListenSlotList();
                this->listenSlotCache[md5]=listenSlotList;
            }
        }

        auto requestInvoke=[&listenSlotList, &vMap, &uploadedFiles](QRPCListenQRPCSlot*&thread){
            thread=nullptr;
            for(auto&v:*listenSlotList){
                if(!v->canRequestInvoke(vMap, uploadedFiles))
                    continue;
                thread=v;
                return true;
            }
            return false;
        };

        QRPCListenQRPCSlot*thread=nullptr;
        while(!requestInvoke(thread)){
            QMutexLOCKER locker(&mutexMapLocker);
            auto thread=new QRPCListenQRPCSlot(this->listenQRPC);
            thread->start();
            listenSlotList->append(thread);
        }

    }
};

QRPCListenQRPC::QRPCListenQRPC(QObject *parent):QRPCListen(nullptr){
    Q_UNUSED(parent)
    this->p = new QRPCListenQRPCPvt(this);
}

QRPCListenQRPC::~QRPCListenQRPC()
{
    dPvt();
    QMutexLocker(&p.mutexRunning);
    delete&p;
}

void QRPCListenQRPC::run()
{
    dPvt();
    p.mutexRunning.lock();

    {
        auto listenSlotCacheAux=p.listenSlotCache;
        p.listenSlotCache.clear();
        for(auto&slot:listenSlotCacheAux){
            for(auto&t:*slot){
                t->quit();
                t->wait();
                t->start();
            }
        }

    }


    {
        p.parsers.clear();
        for(auto&mObj:this->server()->parsers()){
            auto name=QString::fromUtf8(mObj->className()).split(qsl("::")).last().toUtf8();
            auto object=mObj->newInstance(Q_ARG(QObject*, nullptr ));
            if(object==nullptr)
                continue;

            auto controller=dynamic_cast<QRPCListenRequestParser*>(object);
            if(controller!=nullptr)
                p.parsers.insert(name,mObj);
            delete object;
            object=nullptr;
        }
    }


    {
        p.controller.clear();
        for(auto&mObj:this->server()->controllers()){
            auto name=QString::fromUtf8(mObj->className()).split(qsl("::")).last().toUtf8();
            auto object=mObj->newInstance(Q_ARG(QObject*, nullptr ));
            if(object==nullptr)
                continue;
            auto controller=dynamic_cast<QRPCController*>(object);
            if(controller!=nullptr){
                controller->makeRoute();
                p.controller.insert(name,mObj);
            }
            delete object;
            object=nullptr;
        }
    }

    QRPCListen::run();


    {//free thread
        auto listenSlotCacheAux=p.listenSlotCache.values();
        p.listenSlotCache.clear();
        p.listens.clear();
        {
            for(auto&slotList:listenSlotCacheAux){
                for(auto&t:*slotList){
                    t->quit();
                    t->wait();
                    t->deleteLater();
                }
            }
            qDeleteAll(listenSlotCacheAux);
            listenSlotCacheAux.clear();
        }
    }


    p.mutexRunning.unlock();
}

void QRPCListenQRPC::registerListen(QRPCListen *listen)
{
    dPvt();
    p.registerListen(listen);
}

QRPCListen *QRPCListenQRPC::childrenListen(QUuid uuid)
{
    dPvt();
    auto listen = p.listens.value(uuid);
    return listen;
}

}
