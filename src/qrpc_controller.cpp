#include "./qrpc_controller.h"
#include "./qrpc_listen_request_parser.h"
#include "./private/p_qrpc_util.h"
#include "./qrpc_server.h"
#include "./qrpc_request.h"
#include <QCoreApplication>
#include <QList>
#include <QVector>
#include <QMutex>
#include <QDebug>
#include <QMetaMethod>
#include <QJsonDocument>

namespace QRpc {

typedef QVector<QByteArray> ByteArrayVector;
typedef QVector<const QMetaObject*> MetaObjectVector;
typedef QMultiHash<QByteArray, QStringList> MultStringList ;
typedef QMultiHash<QByteArray, QRpc::ControllerMethods> MultStringMethod;
typedef QHash<QByteArray, QVariantHash> NotationMaked;

#define dPvt()\
    auto&p =*reinterpret_cast<ControllerPvt*>(this->p)

Q_GLOBAL_STATIC(ControllerMethods, staticControllerRoutes);
Q_GLOBAL_STATIC(MultStringList, staticControllerRedirect);
Q_GLOBAL_STATIC(MultStringMethod, staticControllerMethods);
Q_GLOBAL_STATIC(MetaObjectVector, staticRegisterInterfaceMetaObject);
Q_GLOBAL_STATIC(MetaObjectVector, staticParserRequestMetaObjects);
Q_GLOBAL_STATIC(ByteArrayVector, staticControllerMethodBlackList);
Q_GLOBAL_STATIC(NotationMaked, staticControllerNotationClassMaked);
Q_GLOBAL_STATIC(NotationMaked, staticControllerNotationMethodsMaked);


static QVariantHash reMakeNotation(const QVariant &notations)
{
    QVariantHash __return;
    auto vList=notations.toList();
    for(auto&v:vList){
        auto vHash=v.toHash();

        if(vHash.isEmpty())
            continue;

        QHashIterator<QString, QVariant> i(vHash);
        while(i.hasNext()){
            i.next();
            __return.insert(i.key(), i.value());
        }
    }
    return __return;
}

static bool staticApiNotationsMethod(const QMetaMethod &method, QVariantHash &vNotation)
{
    if(!staticControllerNotationMethodsMaked->contains(method.name()))
        return {};

    auto v=staticControllerNotationMethodsMaked->value(method.name());

    if(v.isEmpty())
        return {};

    vNotation=v;

    return true;
}

static void staticApiMakeNotations(QObject*makeObject, const QMetaObject*metaObject)
{
    auto className=QByteArray(metaObject->className()).toLower().trimmed();

    if(staticControllerMethods->contains(className))
        return;

    auto ctrMethods=staticControllerMethods->value(className);
    if(!ctrMethods.isEmpty())
        return;

    auto controller=dynamic_cast<Controller*>(makeObject);
    if(controller==nullptr)
        return;

    if(controller->redirectCheck()){
        static QMutex controllerMethodsMutex;
        auto vList=controller->basePath().toStringList();
        QMutexLOCKER locker(&controllerMethodsMutex);
        staticControllerRedirect->insert(className, vList);
    }

#if Q_RPC_LOG_SUPER_VERBOSE
    sWarning()<<"registered class : "<<makeObject->metaObject()->className();
#endif

    static const auto _rpc_notation_method=QByteArray("_rpc_notation_method");
    static const auto _rpc_notation_class=QByteArray("_rpc_notation_class");

    for (auto i = 0; i < metaObject->methodCount(); ++i) {
        auto method = metaObject->method(i);

        if(method.methodType()!=method.Method && method.methodType()!=method.Slot)
            continue;

        if(method.parameterCount()>0){
#if Q_RPC_LOG_SUPER_VERBOSE
            sWarning()<<qsl("Method(%1) ignored").arg(mMth.name().constData());
#endif
            continue;
        }

        auto methodName = method.name().toLower();

        if(!methodName.startsWith(qbl("_")))//ignore methods with [_] in start name
            continue;

        if(method.name().startsWith(_rpc_notation_class)){
            QVariantList returnVariant;
            if(method.invoke(makeObject, Qt::DirectConnection, Q_ARG(QVariantList, returnVariant)))
                staticControllerNotationClassMaked->insert(method.name(), reMakeNotation(returnVariant));
        }

        if(method.name().startsWith(_rpc_notation_method)){
            QVariantList returnVariant;
            if(method.invoke(makeObject, Qt::DirectConnection, Q_ARG(QVariantList, returnVariant)))
                staticControllerNotationMethodsMaked->insert(method.name(), reMakeNotation(returnVariant));
        }
    }
}

static void staticApiMakeBasePath(QObject*makeObject, const QMetaObject*metaObject)
{
    auto className=QByteArray(metaObject->className()).toLower().trimmed();

    if(staticControllerMethods->contains(className))
        return;

    auto ctrMethods=staticControllerMethods->value(className);
    if(!ctrMethods.isEmpty())
        return;

    auto controller=dynamic_cast<Controller*>(makeObject);
    if(controller==nullptr)
        return;

    if(controller->redirectCheck()){
        static QMutex controllerMethodsMutex;
        auto vList=controller->basePath().toStringList();
        QMutexLOCKER locker(&controllerMethodsMutex);
        staticControllerRedirect->insert(className, vList);
    }

#if Q_RPC_LOG_SUPER_VERBOSE
    sWarning()<<"registered class : "<<makeObject->metaObject()->className();
#endif
    for (auto i = 0; i < metaObject->methodCount(); ++i) {
        auto method = metaObject->method(i);

        if(method.methodType()!=method.Method && method.methodType()!=method.Slot)
            continue;

        auto methodName = method.name().toLower();

        if(method.parameterCount()>0){
#if Q_RPC_LOG_SUPER_VERBOSE
            sWarning()<<qsl("Method(%1) ignored").arg(mMth.name().constData());
#endif
            continue;
        }

        if(methodName.startsWith(qbl("_")))//ignore methods with [_] in start name
            continue;

        if(staticControllerMethodBlackList->contains(methodName)){
#if Q_RPC_LOG_SUPER_VERBOSE
            sWarning()<<qsl("Method(%1) in blacklist").arg(mMth.name().constData());
#endif
            continue;
        }

        const auto vRoute=controller->basePath();
        auto vList=qTypeId(vRoute)==QMetaType_QStringList?vRoute.toStringList():QStringList{vRoute.toString()};
        for(auto&v:vList){
            auto route=v.replace(qsl("*"),qsl_null).toLower().toUtf8();
            route=qbl("/")+route+qbl("/")+methodName;
            while(route.contains(qbl("\"")) || route.contains(qbl("//"))){
                route=QString(route).replace(qsl("\""), qsl_null).replace(qsl("//"), qsl("/")).toUtf8();
            }
            if(ctrMethods.contains(route))
                continue;

            ctrMethods.insert(route, method);
            staticControllerRoutes->insert(route, method);
        }
    }
    staticControllerMethods->insert(className, ctrMethods);

}


static void staticApiInitialize(QObject*makeObject, const QMetaObject*metaObject)
{
    staticApiMakeNotations(makeObject, metaObject);
    staticApiMakeBasePath(makeObject, metaObject);
}

static void initQRPCParserRoutes()
{
    for(auto&metaObject:*staticParserRequestMetaObjects){
        ListenRequestParser::apiInitialize(*metaObject);
    }
}

static void initController()
{
    static auto ignoreMethods=QVector<QByteArray>{"destroyed", "objectNameChanged", "deleteLater", "_q_reregisterTimers"};
    auto&statiList=*staticControllerMethodBlackList;
    const auto &metaObject=&Controller::staticMetaObject;
    for (int i = 0; i < metaObject->methodCount(); ++i) {
        auto method = metaObject->method(i);

        if(method.methodType()!=method.Method && method.methodType()!=method.Slot)
            continue;

        if(ignoreMethods.contains(method.name()))
            continue;

        auto methodName = method.name().toLower();
        if(!statiList.contains(methodName))
            statiList.append(methodName);
    }
}

static void init()
{
    initQRPCParserRoutes();
    initController();
}

Q_COREAPP_STARTUP_FUNCTION(init)

class ControllerPvt{
public:
    Server*server=nullptr;
    ListenRequest*request=nullptr;
    ControllerSetting controllerSetting;
    bool enabled=true;

    explicit ControllerPvt()
    {
    }

    virtual ~ControllerPvt()
    {
    }


};

Controller::Controller(QObject *parent):QObject(parent), NotationsExtended(this)
{
    this->p = new ControllerPvt();
}

Controller::~Controller()
{
    dPvt();delete&p;
}

QVariant Controller::basePath() const
{
    return qsl("/");
}

QVariant Controller::route() const
{
    return qsl("/");
}

Controller &Controller::apiInitialize()
{
    Controller::apiInitialize(this, this->metaObject());
    return*this;
}

void Controller::apiInitialize(QObject*object, const QMetaObject*metaObject)
{
    staticApiInitialize(object, metaObject);
}

QString Controller::module() const
{
    return QString();
}

QUuid Controller::moduleUuid()const
{
    return QUuid();
}

bool Controller::redirectCheck() const
{
    return false;
}

QString Controller::description() const
{
    return QString();
}

ControllerSetting &Controller::controllerSetting()
{
    dPvt();
    return p.controllerSetting;
}

bool Controller::enabled() const
{
    dPvt();
    return p.enabled;
}

void Controller::setEnabled(bool enabled)
{
    dPvt();
    p.enabled=enabled;
}

bool Controller::routeRedirectCheckClass(const QByteArray &className)
{
    if(staticControllerRedirect->contains(className.toLower()))
        return true;
    return false;
}

bool Controller::routeRedirectCheckRoute(const QByteArray &className, const QByteArray &routePath)
{
    const auto classNameA=className.toLower();
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QHashIterator<QByteArray, QStringList> i(*staticControllerRedirect);
#else
    QMultiHashIterator<QByteArray, QStringList> i(staticControllerRedirect);
#endif
    while (i.hasNext()) {
        i.next();
        const auto classNameB=i.key().toLower();
        const auto&vList=i.value();
        if(!className.isEmpty() && (classNameB!=classNameA))
            continue;
        if(!Request::startsWith(routePath, vList))
            continue;
        return true;
    }
    return false;
}

bool Controller::routeRedirectMethod(const QByteArray &className, const QByteArray &routePath, QMetaMethod&method)
{
    Q_UNUSED(className)
    Q_UNUSED(routePath)
    Q_UNUSED(method)
    return true;
}

bool Controller::methodExists(const QByteArray &methodName)const
{
    auto _methodName=QRpc::Util::routeParser(methodName);
    auto __route=QRpc::Util::routeExtract(methodName);
    _methodName=QRpc::Util::routeExtractMethod(methodName);
    auto route=QRpc::Util::routeParser(this->basePath());
    if(!(__route.isEmpty() || route==__route))
        return false;

    auto metaObject=this->metaObject();
    for (int i = 0; i < metaObject->methodCount(); ++i) {
        auto method = metaObject->method(i);

        if(method.methodType()!=method.Method && method.methodType()!=method.Slot)
            continue;

        if(method.name().toLower()==_methodName)
            return true;
    }
    return false;
}

bool Controller::routeExists(const QByteArray &routePath)
{
    auto __return=staticControllerRoutes->contains(routePath);
    return __return;
}

ControllerMethods Controller::routeMethods(const QByteArray&className)
{
    return staticControllerMethods->value(className.toLower().trimmed());
}

ListenRequest &Controller::request()
{
    dPvt();
    return*p.request;
}

ListenRequest &Controller::rq()
{
    dPvt();
    static ListenRequest ____request;
    return (p.request==nullptr)?(____request):(*p.request);
}

bool Controller::canOperation(const QMetaMethod &method)
{
    QVariantHash vNotation;
    if(!staticApiNotationsMethod(method, vNotation))
        return true;
    return true;
}

bool Controller::canAuthorization()
{
    auto&rq=this->rq();
    if(rq.isMethodOptions())
        return false;
    return true;
}

bool Controller::canAuthorization(const QMetaMethod &method)
{
    QVariantHash vNotation;
    if(!staticApiNotationsMethod(method, vNotation))
        return true;
    return true;
}

bool Controller::beforeAuthorization()
{
    return true;
}

bool Controller::authorization()
{
    return true;
}

bool Controller::authorization(const QMetaMethod &method)
{
    Q_UNUSED(method)
    return true;
}

bool Controller::afterAuthorization()
{
    return true;
}

bool Controller::requestBeforeInvoke()
{
    auto&rq=this->rq();
    auto vHearder = rq.requestHeader();
    {
        QVariantHash vMap;
        QHashIterator<QString, QVariant> i(vHearder);
        while (i.hasNext()) {
            i.next();
            vMap.insert(QRpc::Util::headerFormatName(i.key()), i.value());
        }
        vMap.remove(qsl("host"));
        vHearder=vMap;
    }

    QVariantHash vHearderResponse;
    auto insertHeaderResponse=[&vHearderResponse](const QString&v0, const QString&v1)
    {
        const auto key=QRpc::Util::headerFormatName(v0);
        const auto value=v1.trimmed();
        if(!key.isEmpty() && !value.isEmpty())
            vHearderResponse.insert(key, value);
    };

    auto header_application_json=vHearder.value(ContentTypeName).toString().trimmed();
    auto origin=vHearder.value(qsl("Origin")).toString();
    auto referer=vHearder.value(qsl("Referer")).toString();
    auto connectionType=vHearder.value(qsl("Connection-Type")).toString();
    auto connection=vHearder.value(qsl("Connection")).toString();
    auto accessControlRequestHeaders=vHearder.value(qsl("Access-Control-Request-Headers")).toString();
    auto accessControlAllowMethods=vHearder.value(qsl("Access-Control-Request-Method")).toString();
    auto accessControlAllowOrigin=origin.isEmpty()?referer:origin;
    auto keepAlive=vHearder.value(qsl("Keep-Alive")).toString();

    if(rq.isMethodOptions()){
        //https://developer.mozilla.org/en-US/docs/Web/HTTP/CORS
        //https://developer.mozilla.org/pt-BR/docs/Web/HTTP/Methods/OPTIONS
        header_application_json=(!header_application_json.isEmpty())?header_application_json:qsl("text/javascripton");
        insertHeaderResponse(qsl("Date"), QDateTime::currentDateTime().toString(Qt::TextDate));
        insertHeaderResponse(qsl("Server"), this->server()->serverName() );
        insertHeaderResponse(qsl("Access-Control-Allow-Origin"), accessControlAllowOrigin);
        insertHeaderResponse(qsl("Access-Control-Allow-Methods"), accessControlAllowMethods);
        insertHeaderResponse(qsl("Access-Control-Allow-Headers"), accessControlRequestHeaders);
        insertHeaderResponse(qsl("Access-Control-Max-Age"), qsl("86400"));
        insertHeaderResponse(qsl("Vary"), qsl("Accept-Encoding, Origin"));
        insertHeaderResponse(qsl("Keep-Alive"), keepAlive);
        insertHeaderResponse(qsl("Connection"), connection);

        rq.setResponseHeader(vHearderResponse);
        rq.co().setOK();
        return rq.co().isOK();
    }

    //https://developer.mozilla.org/en-US/docs/Web/HTTP/CORS
    header_application_json=(!header_application_json.isEmpty())?header_application_json:qsl("text/javascripton");
    insertHeaderResponse(qsl("Date"), QDateTime::currentDateTime().toString(Qt::TextDate));
    insertHeaderResponse(qsl("Server"), this->server()->serverName() );
    insertHeaderResponse(qsl("Access-Control-Allow-Origin"), accessControlAllowOrigin);
    insertHeaderResponse(qsl("Vary"), qsl("Accept-Encoding, Origin"));
    insertHeaderResponse(qsl("Keep-Alive"), keepAlive);
    insertHeaderResponse(qsl("Connection"), connection);
    insertHeaderResponse(qsl("Connection-Type"), connectionType);

    rq.setResponseHeader(vHearderResponse);
    rq.co().setOK();
    return rq.co().isOK();

}

bool Controller::requestRedirect()
{
    return false;
}

bool Controller::requestAfterInvoke()
{
    return true;
}

Server *Controller::server()
{
    dPvt();
    return p.server;
}

int Controller::interfaceRegister(const QMetaObject &metaObject)
{
    if(!staticRegisterInterfaceMetaObject->contains(&metaObject)){
#if Q_RPC_LOG_VERBOSE
        if(staticRegisterInterfaceMetaObject->isEmpty())
            sInfo()<<qsl("interface registered: ")<<metaObject.className();
        qInfo()<<qbl("interface: ")+metaObject.className();
#endif
        (*staticRegisterInterfaceMetaObject)<<&metaObject;
    }
    return staticRegisterInterfaceMetaObject->indexOf(&metaObject);
}

int Controller::parserRequestRegister(const QMetaObject &metaObject)
{
    if(!staticParserRequestMetaObjects->contains(&metaObject)){
#if Q_RPC_LOG_VERBOSE
        if(staticParserRequestMetaObjects->isEmpty())
            sInfo()<<qsl("parser interface registered: ")<<metaObject.className();
        qInfo()<<qbl("parser interface")+metaObject.className();
#endif
        (*staticParserRequestMetaObjects)<<&metaObject;
    }
    return staticParserRequestMetaObjects->indexOf(&metaObject);
}


QVector<const QMetaObject*>&Controller::staticInterfaceList()
{
    return *staticRegisterInterfaceMetaObject;
}

QVector<const QMetaObject *> &Controller::staticParserRequestList()
{
    return *staticParserRequestMetaObjects;
}

void Controller::setServer(Server *server)
{
    dPvt();
    p.server=server;
}

void Controller::setRequest(ListenRequest &request)
{
    dPvt();
    p.request=&request;
}

QVariantHash Controller::routeFlags(const QString &route) const
{
    QString __method=QRpc::Util::routeParser(route);
    if((__method.contains(qsl("/")))){
        auto lst=__method.split(qsl("/"));
        __method=lst.takeLast();
    }
    auto n1=qsl("flags_%1").arg(__method).toUtf8().toLower();

    auto&metaObject=*this->metaObject();
    for (int row = 0; row < metaObject.propertyCount(); ++row) {
        auto property = metaObject.property(row);
        auto n2=QByteArray(property.name()).toLower();
        if(n1!=n2)
            continue;
        auto flags=property.read(this);
        return flags.toHash();
    }
    return {};
}

const QVariantHash Controller::routeFlagsMaker(const QString &request_path, const QVariant &flag)
{
    QString __route;
    auto __method=request_path.trimmed().toLower();
    if((__method.contains(qsl("/")))){
        __method=qsl("/")+__method+qsl("/");
        while(__method.contains(qsl("//")))
            __method=__method.replace(qsl("//"), qsl("/"));

        auto lst=__method.split(qsl("/"));
        __method=lst.takeLast();
        __route=lst.join(qsl("/"));
    }
    auto typeId=qTypeId(flag);
    if(QMetaTypeUtilVariantDictionary.contains(typeId))
        return flag.toHash();

    if(QMetaTypeUtilVariantList.contains(typeId))
        return {};
    return QJsonDocument::fromJson(flag.toString().toUtf8()).toVariant().toHash();
}

const QVector<QByteArray> &Controller::methodBlackList()
{
    return *staticControllerMethodBlackList;
}

}

