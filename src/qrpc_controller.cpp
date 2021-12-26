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


namespace PrivateCPP {

typedef QVector<QByteArray> ByteArrayVector;
typedef QVector<const QMetaObject*> MetaMethodVector;
typedef QMultiHash<QByteArray, QStringList> MultStringList ;
typedef QMultiHash<QByteArray, QRpc::QRPCControllerMethods> MultStringMethod;

auto&makeControllerRoutes(){
    static QRpc::QRPCControllerMethods controllerRedirect;
    return controllerRedirect;
}

auto&makeControllerRedirect(){
    static MultStringList controllerRedirect;
    return controllerRedirect;
}

auto&makeMetaMethodVector(){
    static MetaMethodVector __return;
    return __return;
}

auto&makeControllerMethods(){
    static MultStringMethod __return;
    return __return;
}

auto&makeParserRequestMetaObjects(){
    static MetaMethodVector __return;
    return __return;
}

auto&makeControllerMethodBlackList(){
    static ByteArrayVector __return;
    return __return;
}

//Q_GLOBAL_STATIC(QRpc::QRPCControllerMethods, controllerRoutes);
//Q_GLOBAL_STATIC(MultStringList, controllerRedirect);
//Q_GLOBAL_STATIC(MultStringMethod, controllerMethods);
//Q_GLOBAL_STATIC(MetaMethodVector, staticRegisterInterfaceMetaObject);
//Q_GLOBAL_STATIC(MetaMethodVector, staticParserRequestMetaObjects);
//Q_GLOBAL_STATIC_WITH_ARGS(ByteArrayVector, staticControllerMethodBlackList,(ByteArrayVector()));
}

namespace QRpc {

#define dPvt()\
    auto&p =*reinterpret_cast<QRPCControllerPvt*>(this->p)


static auto&staticControllerRoutes=PrivateCPP::makeControllerRoutes();
static auto&staticControllerRedirect=PrivateCPP::makeControllerRedirect();
static auto&staticControllerMethods=PrivateCPP::makeControllerMethods();
static auto&staticRegisterInterfaceMetaObject=PrivateCPP::makeMetaMethodVector();
static auto&staticParserRequestMetaObjects=PrivateCPP::makeParserRequestMetaObjects();
static auto&staticControllerMethodBlackList=PrivateCPP::makeControllerMethodBlackList();

static void initQRPCRoutes(){
    for(auto&metaObject:staticParserRequestMetaObjects)
        QRPCListenRequestParser::makeRoute(*metaObject);
}

static void initQRPCController()
{
    static auto ignoreMethods=QVector<QByteArray>{"destroyed","objectNameChanged","deleteLater","_q_reregisterTimers"};
    auto&statiList=staticControllerMethodBlackList;
    auto mObj=&QRPCController::staticMetaObject;
    for (int i = 0; i < mObj->methodCount(); ++i) {
        auto mMth = mObj->method(i);
        if(ignoreMethods.contains(mMth.name()))
            continue;

        auto methodName = mMth.name().toLower();
        if(!statiList.contains(methodName))
            statiList.append(methodName);
    }
}

static void init(){
    initQRPCRoutes();
    initQRPCController();
}

Q_COREAPP_STARTUP_FUNCTION(init);

class QRPCControllerPvt{
public:
    QRPCServer*server=nullptr;
    QRPCListenRequest*request=nullptr;
    ControllerSetting controllerSetting;
    bool enabled=true;

    explicit QRPCControllerPvt(){
    }

    virtual ~QRPCControllerPvt(){
    }

    static void makeRoute(QObject*makeObject, const QMetaObject*metaObject){
        static QMutex controllerMethodsMutex;

        auto className=QByteArray(metaObject->className()).toLower().trimmed();

        if(staticControllerMethods.contains(className))
            return;

        auto ctrMethods=staticControllerMethods.value(className);
        if(!ctrMethods.isEmpty())
            return;

        auto controller=dynamic_cast<QRPCController*>(makeObject);
        if(controller!=nullptr){
            if(controller->redirectCheck()){
                auto vList=controller->route().toStringList();
                QMutexLOCKER locker(&controllerMethodsMutex);
                staticControllerRedirect.insert(className, vList);
            }
        }
#if Q_RPC_LOG_SUPER_VERBOSE
        sWarning()<<"class register: "<<className;
#endif
        auto&mObj=metaObject;
        for (auto i = 0; i < mObj->methodCount(); ++i) {
            auto mMth = mObj->method(i);
            auto methodName = mMth.name().toLower();
            if(mMth.parameterCount()>0){
#if Q_RPC_LOG_SUPER_VERBOSE
                sWarning()<<qsl("Method(%1) ignored").arg(mMth.name().constData());
#endif
                continue;
            }

            if(staticControllerMethodBlackList.contains(methodName)){
#if Q_RPC_LOG_SUPER_VERBOSE
                sWarning()<<qsl("Method(%1) in blacklist").arg(mMth.name().constData());
#endif
                continue;
            }

            if(controller!=nullptr){
                const auto vRoute=controller->route();
                auto vList=qTypeId(vRoute)==QMetaType_QStringList?vRoute.toStringList():QStringList{vRoute.toString()};
                for(auto&v:vList){
                    auto route=v.replace(qsl("*"),qsl_null).toLower().toUtf8();
                    route=qbl("/")+route+qbl("/")+methodName;
                    while(route.contains(qbl("\"")) || route.contains(qbl("//"))){
                        route=QString(route).replace(qsl("\""), qsl_null).replace(qsl("//"), qsl("/")).toUtf8();
                    }
                    if(ctrMethods.contains(route))
                        continue;

                    ctrMethods.insert(route, mMth);
                    staticControllerRoutes.insert(route, mMth);
                }
            }
        }
        staticControllerMethods.insert(className, ctrMethods);

    }

};

QRPCController::QRPCController(QObject *parent):QObject(parent)
{
    this->p = new QRPCControllerPvt();
}

QRPCController::~QRPCController()
{
    dPvt();delete&p;
}

QVariant QRPCController::route() const
{
    return QVariant(qsl("/"));
}

void QRPCController::makeRoute()
{
    return QRPCController::makeRoute(this, this->metaObject());
}

QString QRPCController::module() const
{
    return QString();
}

QUuid QRPCController::moduleUuid()const
{
    return QUuid();
}

bool QRPCController::redirectCheck() const
{
    return false;
}

QString QRPCController::description() const
{
    return QString();
}

ControllerSetting &QRPCController::controllerSetting()
{
    dPvt();
    return p.controllerSetting;
}

bool QRPCController::enabled() const
{
    dPvt();
    return p.enabled;
}

void QRPCController::setEnabled(bool enabled)
{
    dPvt();
    p.enabled=enabled;
}

bool QRPCController::routeRedirectCheckClass(const QByteArray &className)
{
    if(staticControllerRedirect.contains(className.toLower()))
        return true;
    return false;
}

bool QRPCController::routeRedirectCheckRoute(const QByteArray &className, const QByteArray &routePath)
{
    const auto classNameA=className.toLower();
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QHashIterator<QByteArray, QStringList> i(staticControllerRedirect);
#else
    QMultiHashIterator<QByteArray, QStringList> i(staticControllerRedirect);
#endif
    while (i.hasNext()) {
        i.next();
        const auto classNameB=i.key().toLower();
        const auto&vList=i.value();
        if(!className.isEmpty() && (classNameB!=classNameA))
            continue;
        if(!QRPCRequest::startsWith(routePath, vList))
            continue;
        return true;
    }
    return false;
}

bool QRPCController::routeRedirectMethod(const QByteArray &className, const QByteArray &routePath, QMetaMethod&method)
{
    Q_UNUSED(className)
    Q_UNUSED(routePath)
    Q_UNUSED(method)
    return true;
}

bool QRPCController::methodExists(const QByteArray &methodName)const
{
    auto _methodName=QRpc::Util::routeParser(methodName);
    auto __route=QRpc::Util::routeExtract(methodName);
    _methodName=QRpc::Util::routeExtractMethod(methodName);
    auto route=QRpc::Util::routeParser(this->route());
    if(__route.isEmpty() || route==__route){
        auto mObj=this->metaObject();
        for (int i = 0; i < mObj->methodCount(); ++i) {
            auto mMth = mObj->method(i);
            if(mMth.name().toLower()==_methodName)
                return true;
        }
    }
    return false;
}

bool QRPCController::routeExists(const QByteArray &routePath)
{
    auto __return=staticControllerRoutes.contains(routePath);
    return __return;
}

QRPCControllerMethods QRPCController::routeMethods(const QByteArray&className)
{
    return staticControllerMethods.value(className.toLower().trimmed());
}

QRPCListenRequest &QRPCController::request()
{
    dPvt();
    return*p.request;
}

QRPCListenRequest &QRPCController::rq()
{
    dPvt();
    static QRPCListenRequest ____request;
    return (p.request==nullptr)?(____request):(*p.request);
}

bool QRPCController::canAuthorization()
{
    auto&rq=this->rq();
    if(rq.isMethodOptions())
        return false;
    return true;
}

bool QRPCController::beforeAuthorization()
{
    return true;
}

bool QRPCController::authorization()
{
    return true;
}

bool QRPCController::afterAuthorization()
{
    return true;
}

bool QRPCController::requestBeforeInvoke()
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
    }
    else{
        //https://developer.mozilla.org/en-US/docs/Web/HTTP/CORS
        header_application_json=(!header_application_json.isEmpty())?header_application_json:qsl("text/javascripton");
        insertHeaderResponse(qsl("Date"), QDateTime::currentDateTime().toString(Qt::TextDate));
        insertHeaderResponse(qsl("Server"), this->server()->serverName() );
        insertHeaderResponse(qsl("Access-Control-Allow-Origin"), accessControlAllowOrigin);
        insertHeaderResponse(qsl("Vary"), qsl("Accept-Encoding, Origin"));
        insertHeaderResponse(qsl("Keep-Alive"), keepAlive);
        insertHeaderResponse(qsl("Connection"), connection);
        insertHeaderResponse(qsl("Connection-Type"), connectionType);

    }
    rq.setResponseHeader(vHearderResponse);
    rq.co().setOK();
    return rq.co().isOK();
}

bool QRPCController::requestRedirect()
{
    return false;
}

bool QRPCController::requestAfterInvoke()
{
    return true;
}

QRPCServer *QRPCController::server()
{
    dPvt();
    return p.server;
}

int QRPCController::registerInterface(const QMetaObject &metaObject)
{
    if(!staticRegisterInterfaceMetaObject.contains(&metaObject)){
#if Q_RPC_LOG_VERBOSE
        if(staticRegisterInterfaceMetaObject.isEmpty())
            sInfo()<<qsl("interface registered: ")<<metaObject.className();
        qInfo()<<qbl("interface: ")+metaObject.className();
#endif
        staticRegisterInterfaceMetaObject<<&metaObject;
    }
    return staticRegisterInterfaceMetaObject.indexOf(&metaObject);
}

int QRPCController::registerParserRequest(const QMetaObject &metaObject)
{
    if(!staticParserRequestMetaObjects.contains(&metaObject)){
#if Q_RPC_LOG_VERBOSE
        if(staticParserRequestMetaObjects.isEmpty())
            sInfo()<<qsl("parser interface registered: ")<<metaObject.className();
        qInfo()<<qbl("parser interface")+metaObject.className();
#endif
        staticParserRequestMetaObjects<<&metaObject;
    }
    return staticParserRequestMetaObjects.indexOf(&metaObject);
}


QVector<const QMetaObject*>&QRPCController::staticInterfaceList()
{
    return staticRegisterInterfaceMetaObject;
}

QVector<const QMetaObject *> &QRPCController::parserRequestList()
{
    return staticParserRequestMetaObjects;
}

void QRPCController::setServer(QRPCServer *server)
{
    dPvt();
    p.server=server;
}

void QRPCController::setRequest(QRPCListenRequest &request)
{
    dPvt();
    p.request=&request;
}

void QRPCController::makeRoute(QObject*object, const QMetaObject*metaObject)
{
    Q_UNUSED(object)
    Q_UNUSED(metaObject)
    QRPCControllerPvt::makeRoute(object,metaObject);
}

QVariantHash QRPCController::routeFlags(const QString &route) const
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
        if(n1==n2){
            auto flags=property.read(this);
            return flags.toHash();
        }
    }
    return {};
}

const QVariantHash QRPCController::routeFlagsMaker(const QString &request_path, const QVariant &flag)
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
    if(QStmTypesVariantDictionary.contains(typeId))
        return flag.toHash();

    if(QStmTypesVariantList.contains(typeId))
        return {};
    return QJsonDocument::fromJson(flag.toString().toUtf8()).toVariant().toHash();
}

const QVector<QByteArray> &QRPCController::methodBlackList()
{
    return staticControllerMethodBlackList;
}

}
