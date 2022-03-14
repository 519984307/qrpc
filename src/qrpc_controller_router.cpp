#include "./qrpc_controller_router.h"
#include "./qrpc_listen_request.h"

namespace QRpc {

#define dPvt()\
    auto&p =*reinterpret_cast<ControllerRouterPvt*>(this->p)

class ConstsControllerRouter{
public:
    QList<const QMetaObject*> defaultRouters;
    explicit ConstsControllerRouter()
    {
        defaultRouters<<&ControllerRouter::staticMetaObject;
    }
    virtual ~ConstsControllerRouter()
    {
    }
};
Q_GLOBAL_STATIC_WITH_ARGS(ConstsControllerRouter, __constsControllerRouter, (ConstsControllerRouter()))
static auto&constsControllerRouter=*__constsControllerRouter;

class ControllerRouterPvt{
public:
    QRpc::ControllerRouter*ControllerRouter=nullptr;
    explicit ControllerRouterPvt(QRpc::ControllerRouter*parent)
    {
        this->ControllerRouter=parent;
    }

    virtual ~ControllerRouterPvt(){
    }
};

ControllerRouter::ControllerRouter(QObject *parent):QStm::Object(parent)
{
    this->p = new ControllerRouterPvt(this);
}

ControllerRouter::~ControllerRouter()
{
    dPvt();delete&p;
}

ResultValue &ControllerRouter::router(QRpc::ListenRequest*request, QMetaMethod &metaMethod)
{
    Q_UNUSED(request)
    Q_UNUSED(metaMethod)
    return this->lr();
}

bool ControllerRouter::installDefaultRouter(const QMetaObject*metaObject)
{
    constsControllerRouter.defaultRouters.clear();
    if(metaObject!=nullptr){
        constsControllerRouter.defaultRouters<<metaObject;
        return true;
    }
    constsControllerRouter.defaultRouters<<&ControllerRouter::staticMetaObject;
    return false;
}

ControllerRouter *ControllerRouter::newRouter(QObject *parent)
{
    ControllerRouter*router=nullptr;

    auto&defaultRouters=constsControllerRouter.defaultRouters;
    auto&defaultRouter=defaultRouters.first();

    if(defaultRouters.isEmpty())
        return new ControllerRouter(parent);

    auto object=defaultRouter->newInstance(Q_ARG(QObject*, parent));
    router=dynamic_cast<ControllerRouter*>(object);
    if(router!=nullptr)
        return router;

    if(object!=nullptr)
        delete object;

    return nullptr;
}

}
