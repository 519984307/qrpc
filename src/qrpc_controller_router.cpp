#include "./qrpc_controller_router.h"
#include "./qrpc_listen_request.h"

namespace QRpc {

#define dPvt()\
    auto&p =*reinterpret_cast<QRPCControllerRouterPvt*>(this->p)

class ConstsQRPCControllerRouter{
public:
    QList<const QMetaObject*> defaultRouters;
    explicit ConstsQRPCControllerRouter(){
        defaultRouters<<&QRPCControllerRouter::staticMetaObject;
    }
    ~ConstsQRPCControllerRouter(){

    }
};
Q_GLOBAL_STATIC_WITH_ARGS(ConstsQRPCControllerRouter, __constsQRPCControllerRouter, (ConstsQRPCControllerRouter()))
static auto&constsQRPCControllerRouter=*__constsQRPCControllerRouter;

class QRPCControllerRouterPvt{
public:
    QRPCControllerRouter*ControllerRouter=nullptr;
    explicit QRPCControllerRouterPvt(QRPCControllerRouter*parent){
        this->ControllerRouter=parent;
    }

    virtual ~QRPCControllerRouterPvt(){
    }
};

QRPCControllerRouter::QRPCControllerRouter(QObject *parent):QStm::Object(parent)
{
    this->p = new QRPCControllerRouterPvt(this);
}

QRPCControllerRouter::~QRPCControllerRouter()
{
    dPvt();delete&p;
}

ResultValue &QRPCControllerRouter::router(QRpc::QRPCListenRequest*request, QMetaMethod &metaMethod)
{
    Q_UNUSED(request)
    Q_UNUSED(metaMethod)
    return this->lr();
}

bool QRPCControllerRouter::installDefaultRouter(const QMetaObject*metaObject)
{
    constsQRPCControllerRouter.defaultRouters.clear();
    if(metaObject!=nullptr){
        constsQRPCControllerRouter.defaultRouters<<metaObject;
        return true;
    }
    else{
        constsQRPCControllerRouter.defaultRouters<<&QRPCControllerRouter::staticMetaObject;
    }
    return false;
}

QRPCControllerRouter *QRPCControllerRouter::newRouter(QObject *parent)
{
    QRPCControllerRouter*router=nullptr;

    auto&defaultRouters=constsQRPCControllerRouter.defaultRouters;
    auto&defaultRouter=defaultRouters.first();

    if(defaultRouters.isEmpty())
        router=new QRPCControllerRouter(parent);
    else{
        auto object=defaultRouter->newInstance(Q_ARG(QObject*, parent));
        router=dynamic_cast<QRPCControllerRouter*>(object);
        if(router==nullptr){
            if(object!=nullptr)
                delete object;

        }
    }
    return router;
}

}
