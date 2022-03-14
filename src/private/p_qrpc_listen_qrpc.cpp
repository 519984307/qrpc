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
    auto&p =*reinterpret_cast<ListenQRPCPvt*>(this->p)

typedef QVector<ListenQRPCSlot*> ListenSlotList;
typedef QHash<QByteArray, ListenSlotList*> ListenSlotCache;

class ListenQRPCPvt: public QObject{
public:
    QMutex mutexRunning;
    QMutex mutexMapLocker;
    QHash<QByteArray, const QMetaObject*> controller;
    QHash<QByteArray, const QMetaObject*> parsers;
    ListenSlotCache listenSlotCache;
    ListenQRPC*listenQRPC=nullptr;
    QMap<QUuid, Listen*> listens;
    Server*server=nullptr;

    explicit ListenQRPCPvt(ListenQRPC*parent):QObject(parent)
    {
        this->listenQRPC = dynamic_cast<ListenQRPC*>(this->parent());
        if(this->listenQRPC!=nullptr)
            this->server = this->listenQRPC->server();
    }

    virtual ~ListenQRPCPvt()
    {
        auto list=listenSlotCache.values();
        listenSlotCache.clear();
        qDeleteAll(list);
    }

    void registerListen(Listen*listen)
    {
        auto listenQRPC=this->listenQRPC;
        if(listen==listenQRPC)
            return;
        if(this->listens.contains(listen->uuid()))
            return;
        this->listens.insert(listen->uuid(), listen);
        listen->registerListenPool(listenQRPC);
        QObject::connect(listen, &Listen::rpcRequest, this, &ListenQRPCPvt::onRpcRequest);
    }

public slots:
    void onRpcRequest(QVariantHash vRequest, const QVariant&uploadedFiles)
    {
        ListenRequest request;
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

        auto vHash=request.toHash();
        ListenSlotList*listenSlotList=nullptr;
        {//locker
            QMutexLOCKER locker(&mutexMapLocker);
            auto md5=request.hash();
            listenSlotList=this->listenSlotCache.value(md5);
            if(listenSlotList==nullptr){
                listenSlotList = new ListenSlotList();
                this->listenSlotCache[md5]=listenSlotList;
            }
        }

        auto requestInvoke=[&listenSlotList, &vHash, &uploadedFiles](ListenQRPCSlot*&thread){
            thread=nullptr;
            for(auto&v:*listenSlotList){
                if(!v->canRequestInvoke(vHash, uploadedFiles))
                    continue;
                thread=v;
                return true;
            }
            return false;
        };

        ListenQRPCSlot*thread=nullptr;
        while(!requestInvoke(thread)){
            QMutexLOCKER locker(&mutexMapLocker);
            auto thread=new ListenQRPCSlot(this->listenQRPC);
            thread->start();
            listenSlotList->append(thread);
        }

    }
};

ListenQRPC::ListenQRPC(QObject *parent):Listen(nullptr){
    Q_UNUSED(parent)
    this->p = new ListenQRPCPvt(this);
}

ListenQRPC::~ListenQRPC()
{
    dPvt();
    QMutexLOCKER(&p.mutexRunning);
    delete&p;
}

void ListenQRPC::run()
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

            auto controller=dynamic_cast<ListenRequestParser*>(object);
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
            auto controller=dynamic_cast<Controller*>(object);
            if(controller!=nullptr){
                controller->apiInitialize();
                p.controller.insert(name,mObj);
            }
            delete object;
            object=nullptr;
        }
    }

    Listen::run();


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

void ListenQRPC::registerListen(Listen *listen)
{
    dPvt();
    p.registerListen(listen);
}

Listen *ListenQRPC::childrenListen(QUuid uuid)
{
    dPvt();
    auto listen = p.listens.value(uuid);
    return listen;
}

}
