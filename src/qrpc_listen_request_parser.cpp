#include "./qrpc_listen_request_parser.h"
#include "./qrpc_listen_request.h"
#include "./private/p_qrpc_listen_request_code.h"
#include "./private/p_qrpc_util.h"
#include "./qrpc_listen_request.h"
#include "./qrpc_controller.h"
#include <QThread>
#include <QMetaType>
#include <QMetaMethod>
#include <QVariantHash>
#include <QByteArray>
#include <QUuid>
#include <QMutex>
#include <QMutexLocker>
#include <QDebug>

namespace QRpc {

typedef QHash<QString, QString> StringStringMap ;
typedef QMultiHash<QString, QMetaMethod> StringMetaMethod;

Q_GLOBAL_STATIC(StringStringMap, staticMetaObjectRoute);
Q_GLOBAL_STATIC(StringMetaMethod, staticMetaObjectMetaMethod);
Q_GLOBAL_STATIC(QMutex, staticMetaObjectLock);

#define dPvt()\
    auto&p =*reinterpret_cast<QRPCListenRequestParserPvt*>(this->p)

class QRPCListenRequestParserPvt{
public:
    QRPCController*controller=nullptr;
    explicit QRPCListenRequestParserPvt(QObject*parent=nullptr){
        Q_UNUSED(parent)
    }
    virtual ~QRPCListenRequestParserPvt(){
    }
};


QRPCListenRequestParser::QRPCListenRequestParser(QObject *parent) : QObject(parent)
{
    this->p = new QRPCListenRequestParserPvt(parent);
}

QRPCListenRequestParser::~QRPCListenRequestParser(){
    dPvt();
    delete&p;
}

QRPCController &QRPCListenRequestParser::controller()
{
    dPvt();
    return*p.controller;
}

QRPCListenRequest &QRPCListenRequestParser::request()
{
    dPvt();
    if(p.controller==nullptr){
        static QRPCListenRequest req;
        req.clear();
        return req;
    }
    else{
        return p.controller->request();
    }
}

QRPCListenRequest &QRPCListenRequestParser::rq()
{
    return this->request();
}

bool QRPCListenRequestParser::canRoute(const QMetaObject &metaObject, const QString &route)
{
    auto className=QByteArray(metaObject.className());
    QString staticRoute=staticMetaObjectRoute->value(className);
    auto route1=QRpc::Util::routeParser(staticRoute).toLower();
    auto route2=QRpc::Util::routeExtract(route).toLower();
    if(route==route2 || route==route2 || route1==route2)
        return true;
    else
        return false;
}

bool QRPCListenRequestParser::routeToMethod(const QMetaObject &metaObject, const QString &route, QMetaMethod&outMethod)
{
    auto vRoute=QRpc::Util::routeParser(route).split(qbl("/")).join(qbl("/"));
    if(QRPCListenRequestParser::canRoute(metaObject, vRoute)){
        auto list = staticMetaObjectMetaMethod->values(metaObject.className());
        auto v0=QRpc::Util::routeExtractMethod(route);
        for(auto&v:list){
            auto v1=QByteArray(v.name()).toLower();
            if(v0==v1){
                outMethod=v;
                return outMethod.isValid();
            }
        }
    }
    outMethod=QMetaMethod();
    return false;
}

void QRPCListenRequestParser::makeRoute(const QMetaObject &metaObject)
{
    QScopedPointer<QObject> scopePointer(metaObject.newInstance(Q_ARG(QObject*, nullptr )));
    auto object=scopePointer.data();
    if(object!=nullptr){
        auto parser=dynamic_cast<QRPCListenRequestParser*>(object);
        if(parser!=nullptr){
            static const auto ignoreNames=QVector<QString>()<<qsl("route")<<qsl("makeRoute");
            auto className=QByteArray(metaObject.className());
            if(!staticMetaObjectRoute->contains(className)){
                auto route=parser->route();
                QMutexLOCKER locker(staticMetaObjectLock);
                staticMetaObjectRoute->insert(className, route);
                for(int methodIndex = 0; methodIndex < metaObject.methodCount(); ++methodIndex) {
                    auto method = metaObject.method(methodIndex);
                    if(method.returnType()!=QMetaType_Bool)
                        continue;

                    if(method.parameterCount()>0)
                        continue;

                    if(ignoreNames.contains(method.name()))
                        continue;

                    {
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
                        staticMetaObjectMetaMethod->insert(className, method);
#else
                        staticMetaObjectMetaMethod->insertMulti(className, method);
#endif
                    }
                }
            }
        }
    }
}

bool QRPCListenRequestParser::parse(const QMetaMethod&metaMethod)
{
    bool returnVariant=false;
    auto argReturn=Q_RETURN_ARG(bool, returnVariant);
    const auto&className=this->metaObject()->className();
    if(!metaMethod.invoke(this, Qt::DirectConnection, argReturn)){
#if Q_RPC_LOG
        sWarning()<<"Parser not valid "<<className;
#endif
    }
    else if(!returnVariant){//Unauthorized
#if Q_RPC_LOG
        sWarning()<<"Parser validation error "<<className;
#endif
    }
    else{
        return true;
    }
    return this->rq().co().setInternalServerError().isOK();
}

void QRPCListenRequestParser::setController(QRPCController *value)
{
    dPvt();
    p.controller = value;
}

}
