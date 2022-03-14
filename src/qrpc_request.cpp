#include "./qrpc_request.h"
#include "./private/p_qrpc_request.h"
#include "./private/p_qrpc_listen_request_code.h"

namespace QRpc {

Request::Request(QObject *parent):QObject(parent)
{
    this->p = new RequestPvt(this);
}

Request::~Request()
{
    dPvt();delete&p;
}

bool Request::startsWith(const QString &requestPath, const QVariant &requestPathBase)
{
    auto list=requestPathBase.toStringList();
    for(auto&requestPathBase:list){
        auto startWith=requestPathBase.contains(qsl("*"));
        if(startWith){
            requestPathBase=requestPathBase.split(qsl("*")).first();
        }

        auto route=qsl("/%1/").arg(requestPathBase.trimmed().toLower());
        auto path=qsl("/%1/").arg(requestPath.trimmed().toLower());

        while(route.contains(qsl("//")))
            route=route.replace(qsl("//"),qsl("/"));

        while(path.contains(qsl("//")))
            path=path.replace(qsl("//"),qsl("/"));

        if(startWith && path.startsWith(route))
            return true;

        if(path==route)
            return true;

    }

    return false;
}

bool Request::startsWith(const QString &requestPath)
{
    return this->startsWith(this->route(), requestPath);
}

bool Request::isEqual(const QString &requestPath)
{
    return this->isEqual(requestPath, this->route());
}

bool Request::isEqual(const QString &requestPath, const QVariant &requestPathBase)
{
    return Request::startsWith(requestPath,requestPathBase);
}

bool Request::canRequest() const
{
    if(this->port().toInt()<0)
        return false;

    if(this->hostName().isEmpty())
        return false;

    return true;
}

Request &Request::setSettings(const ServiceSetting &setting)
{
    dPvt();
    p.setSettings(setting);
    return*this;
}

Request &Request::setSettings(const QVariantHash &setting)
{
    ServiceSetting _setting;
    _setting.fromHash(setting);
    auto&request=*this;
    request.header().addRawHeader(_setting.headers());//precisa ser add
    request.setProtocol(_setting.protocol());
    request.setPort(_setting.port());
    request.setHostName(_setting.hostName());
    request.setMethod(_setting.method());
    request.setRoute(_setting.route());

    auto method=request.exchange().call().method();
    if(method==QRpc::Post || method==QRpc::Put)
        request.setBody(_setting.body());
    else if(!_setting.parameters().isEmpty())
        request.setBody(_setting.parameters());
    else
        request.setBody(_setting.body());
    return*this;
}

QString Request::url() const
{
    return this->url(qsl_null);
}

QString Request::url(const QString &path) const
{
    auto&rq=*this;

    auto spath=path.trimmed().isEmpty()?this->route().trimmed():path.trimmed();
    spath=qsl("/%1").arg(spath);
    while(spath.contains(qsl("//")))
        spath=spath.replace(qsl("//"),"/");

    if(path.isEmpty())
        return qsl("%1://%2:%3").arg(rq.protocolName(),rq.hostName(),rq.port().toString());

    return qsl("%1://%2:%3%4").arg(rq.protocolName(),rq.hostName(),rq.port().toString(),spath);
}

QRPCProtocol Request::protocol() const
{
    dPvt();
    return p.exchange.call().protocol();
}

QString Request::protocolName() const
{
    dPvt();
    return p.exchange.call().protocolName();
}

Request &Request::setProtocol(const QVariant &value)
{
    dPvt();
    p.exchange.setProtocol(value);
    return*this;
}

RequestMethod Request::method() const
{
    dPvt();
    return RequestMethod(p.exchange.call().method());
}

Request &Request::setMethod(const QString &value)
{
    dPvt();
    p.exchange.setMethod(value);
    return*this;
}

Request &Request::setMethod(const QByteArray &value)
{
    dPvt();
    p.exchange.setMethod(value);
    return*this;
}

Request &Request::setMethod(const int &value)
{
    dPvt();
    p.exchange.setMethod(RequestMethod(value));
    return*this;
}

QString Request::driver() const
{
    dPvt();
    return p.exchange.call().driver();
}

Request &Request::setDriver(const QString &value)
{
    dPvt();
    p.exchange.call().setDriver(value);
    return*this;
}

QString Request::hostName() const
{
    dPvt();
    return p.exchange.call().hostName();
}

Request &Request::setHostName(const QString &value)
{
    dPvt();
    p.exchange.setHostName(value);
    return*this;
}

QString Request::userName() const
{
    dPvt();
    return p.exchange.call().userName();
}

Request &Request::setUserName(const QString &value)
{
    dPvt();
    p.exchange.call().setUserName(value);
    return*this;
}

QString Request::password() const
{
    dPvt();
    return p.exchange.call().passWord();
}

Request &Request::setPassword(const QString &value)
{
    dPvt();
    p.exchange.call().setPassWord(value);
    return*this;
}

QString&Request::route() const
{
    dPvt();
    return p.exchange.call().route();
}

Request &Request::setRoute(const QVariant &value)
{
    dPvt();
    p.exchange.call().setRoute(value);
    return*this;
}

QVariant Request::body() const
{
    dPvt();
    return p.qrpcBody.body();
}

Request &Request::setBody(const QVariant &value)
{
    dPvt();
    p.qrpcBody.setBody(value);
    return*this;
}

QVariant Request::port() const
{
    dPvt();
    return p.exchange.call().port();
}

Request &Request::setPort(const QVariant &value)
{
    dPvt();

    QVariant v;
    auto type=qTypeId(value);
    switch (type) {
    case QMetaType_QVariantList:
    case QMetaType_QStringList:
    {
        auto l=value.toList();
        v=l.isEmpty()?0:l.last().toInt();
        break;
    }
    case QMetaType_QVariantHash:
    case QMetaType_QVariantMap:
    {
        auto l=value.toHash().values();
        v=l.isEmpty()?0:l.last().toInt();
        break;
    }
    default:
        v=value.toInt();
    }
    p.exchange.setPort(v.toInt());
    return*this;
}

qlonglong Request::activityLimit() const
{
    dPvt();
    return p.exchange.call().activityLimit();
}

Request &Request::setActivityLimit(const QVariant &value)
{
    dPvt();
    p.exchange.call().setActivityLimit(value);
    return*this;
}

RequestExchange &Request::exchange()
{
    dPvt();
    return p.exchange;
}

QRpc::HttpHeaders &Request::header()
{
    dPvt();
    return p.qrpcHeader;
}

Request::Body &Request::body()
{
    dPvt();
    return p.qrpcBody;
}

HttpResponse &Request::response()
{
    dPvt();
    return p.qrpcResponse;
}

QHash<int, int> Request::requestRecovery()const
{
    dPvt();
    return p.requestRecovery;
}

Request &Request::setRequestRecovery(int statusCode)
{
    dPvt();
    p.requestRecovery[statusCode]=1;
    return*this;
}

Request &Request::setRequestRecovery(int statusCode, int repeatCount)
{
    dPvt();
    p.requestRecovery[statusCode]=repeatCount;
    return*this;
}

Request &Request::setRequestRecoveryOnBadGateway(int repeatCount)
{
    dPvt();
    p.requestRecovery[ListenRequestCode::ssBadGateway]=repeatCount;
    return*this;
}

LastError &Request::lastError()
{
    dPvt();
    return p.qrpcLastError;
}

HttpResponse &Request::call()
{
    dPvt();
    auto&e=p.exchange.call();
    return p.call(e.method(), e.route(), QByteArray());
}

HttpResponse &Request::call(const QVariant &route)
{
    dPvt();
    auto&e=p.exchange.call();
    e.setRoute(route);
    return p.call(e.method(), e.route(), this->body().body());
}

HttpResponse &Request::call(const QVariant &route, const QVariant &body)
{
    dPvt();
    auto&e=p.exchange.call();
    e.setRoute(route);
    return p.call(e.method(), e.route(), body);
}

HttpResponse &Request::call(const QVariant &route, const QVariant&body, const QString &method, const QVariantHash parameter)
{
    dPvt();
    auto&e=p.exchange.call();
    e.setMethod(method);
    e.setRoute(route);
    e.setParameter(parameter);
    return p.call(e.method(), e.route(), body);
}

HttpResponse &Request::call(const RequestMethod &method, const QString &route, const QVariant &body)
{
    dPvt();
    auto&e=p.exchange.call();
    e.setRoute(route);
    e.setMethod(method);
    return p.call(e.method(), e.route(), body);
}

HttpResponse &Request::call(const RequestMethod &method)
{
    dPvt();
    auto&e=p.exchange.call();
    e.setMethod(method);
    auto&body=this->body().body();
    return p.call(e.method(), e.route(), body);
}

HttpResponse &Request::call(const QVariant &route, const QObject &objectBody)
{
    QVariantHash mapObject;
    for(int i = 0; i < objectBody.metaObject()->propertyCount(); ++i) {
        auto property=objectBody.metaObject()->property(i);
        mapObject.insert(property.name(), property.read(&objectBody));
    }
    dPvt();
    auto&e=p.exchange.call();
    e.setMethod(QRpc::Post);
    e.setRoute(route);
    return p.call(e.method(), route, QJsonDocument::fromVariant(mapObject).toJson());
}

HttpResponse &Request::call(const RequestMethod &method, const QString &route, const QObject &objectBody)
{
    QVariantHash mapObject;
    for(int i = 0; i < objectBody.metaObject()->propertyCount(); ++i) {
        auto property=objectBody.metaObject()->property(i);
        mapObject.insert(property.name(), property.read(&objectBody));
    }
    dPvt();
    auto&e=p.exchange.call();
    e.setRoute(route);
    e.setMethod(method);
    return p.call(e.method(), e.route(), mapObject);
}

HttpResponse &Request::call(const QVariant &route, QIODevice &ioDeviceBody)
{
    dPvt();
    auto body=ioDeviceBody.readAll();
    auto&e=p.exchange.call();
    e.setMethod(QRpc::Post);
    e.setRoute(route);
    return p.call(e.method(), e.route(), body);
}

HttpResponse &Request::call(const RequestMethod &method, const QString &route)
{
    dPvt();
    auto&e=p.exchange.call();
    e.setRoute(route);
    e.setMethod(method);
    return p.call(e.method(), e.route(), this->body().body());
}

HttpResponse &Request::call(const RequestMethod &method, const QString &route, QIODevice &ioDeviceBody)
{
    dPvt();
    auto body=ioDeviceBody.readAll();
    auto&e=p.exchange.call();
    e.setRoute(route);
    e.setMethod(method);
    return p.call(e.method(), e.route(), body);
}

Request &Request::operator=(const QRpc::ServiceSetting &value)
{
    this->setSettings(value);
    return*this;
}

HttpResponse &Request::upload(QFile &file)
{
    dPvt();
    auto&e=p.exchange.call();
    e.setMethod(QRpc::Post);
    p.upload(e.route(), file.fileName());
    file.close();
    return this->response();
}

HttpResponse &Request::upload(const QVariant &route, const QByteArray &buffer)
{
    dPvt();
    auto&e=p.exchange.call();
    e.setRoute(route);
    e.setMethod(QRpc::Post);
    QTemporaryFile file;

    if(!file.open())
        return this->response();
    file.write(buffer);
    file.flush();
    p.upload(e.route(), file.fileName());
    file.close();
    return this->response();
}


HttpResponse &Request::upload(const QVariant &route, QFile &file)
{
    dPvt();
    auto&e=p.exchange.call();
    e.setRoute(route);
    e.setMethod(QRpc::Post);
    p.upload(e.route(), file.fileName());
    file.close();
    return this->response();
}

HttpResponse &Request::upload(const QVariant &route, QString &fileName, QFile &file)
{
    dPvt();
    auto&e=p.exchange.call();
    e.setRoute(route);
    e.setMethod(QRpc::Post);
    p.upload(e.route(), fileName);
    file.close();
    return this->response();
}


HttpResponse &Request::download(QString &fileName)
{
    dPvt();
    auto _fileName=p.parseFileName(fileName);
    auto&e=p.exchange.call();
    e.setMethod(QRpc::Get);
    auto&response=p.download(e.route(), _fileName);
    if(response)
        fileName=_fileName;
    return this->response();
}

HttpResponse &Request::download(const QVariant &route, QString &fileName)
{
    dPvt();
    auto _fileName=p.parseFileName(fileName);
    auto&e=p.exchange.call();
    e.setRoute(route);
    e.setMethod(QRpc::Get);
    auto&response=p.download(e.route(), _fileName);
    if(response)
        fileName=_fileName;
    return response;
}

HttpResponse &Request::download(const QVariant &route, QString &fileName, const QVariant &parameter)
{
    dPvt();
    auto _fileName=p.parseFileName(fileName);
    auto&e=p.exchange.call();
    e.setRoute(route);
    e.setMethod(QRpc::Get);
    this->setBody(parameter);
    auto&response=p.download(e.route(), _fileName);
    if(response)
        fileName=_fileName;
    return response;
}

Request &Request::autoSetCookie()
{
    auto vRawHeader=this->response().header().rawHeader();
    QHashIterator<QString, QVariant> i(vRawHeader);
    auto cookies=this->header().cookies().toStringList();
    while (i.hasNext()){
        i.next();
        if(!i.key().toLower().startsWith(qsl("set-cookie")))
            continue;
        auto cookieList=i.value().toString().split(qsl(";"));
        for(auto&cookie:cookieList){
            if(!cookies.contains(cookie))
                cookies.append(cookie);
        }
    }
    this->header().setCookies(cookies);
    return*this;
}

QString Request::toString() const
{
    dPvt();\
    auto&response=p.qrpcResponse;
    auto qt_text=ListenRequestCode::qt_network_error_phrase(p.response_qt_status_code);
    auto msg=qsl("%1:QtStatus: Status:%2, %3, %4").arg(p.exchange.call().url(), QString::number(response.qtStatusCode()),response.reasonPhrase(), qt_text);
    return msg;
}

QVariantHash Request::toResponse()const
{
    dPvt();
    return p.qrpcResponse.toResponse();
}

QSslConfiguration &Request::sslConfiguration()
{
    dPvt();
    return p.sslConfiguration;
}

Request&Request::setSslConfiguration(const QSslConfiguration &value)
{
    dPvt();
    p.sslConfiguration = value;
    return*this;
}

Request &Request::print()
{
    for(auto&v:this->printOut())
        sInfo()<<v;
    return*this;
}

QStringList Request::printOut()
{
    QStringList out;
    for(auto&v:this->exchange().printOut(qsl("exchange")))
        out<<v;
    for(auto&v:this->header().printOut(qsl("request")))
        out<<v;
    for(auto&v:this->response().printOut(qsl("response")))
        out<<v;
    return out;
}

Request::Body::Body(QObject *parent):QObject(parent)
{
}

Request::Body::~Body()
{
    
}

QVariant &Request::Body::body() const
{
    dPvt();
    return p.request_body;
}

void Request::Body::setBody(const QVariant &value)
{
    dPvt();
    p.request_body=value;
}

QString Request::Body::toString()const
{
    dPvt();
    auto type=qTypeId(p.request_body);
    switch (type) {
    case QMetaType_QVariantList:
    case QMetaType_QVariantHash:
    case QMetaType_QVariantMap:
    case QMetaType_QStringList:
        return QJsonDocument::fromVariant(p.request_body).toJson();
    default:
        return p.request_body.toString();
    }
}

QVariantMap Request::Body::toMap() const
{
    return QVariant(this->toHash()).toMap();
}

QVariantHash Request::Body::toHash()const
{
    dPvt();
    auto type=qTypeId(p.request_body);
    switch (type) {
    case QMetaType_QVariantHash:
    case QMetaType_QVariantList:
    case QMetaType_QVariantMap:
    case QMetaType_QStringList:
        return QJsonDocument::fromVariant(p.request_body).object().toVariantHash();
    default:
        return QJsonDocument::fromJson(p.request_body.toByteArray()).object().toVariantHash();
    }
}

QVariantList Request::Body::toList() const
{
    dPvt();
    auto type=qTypeId(p.request_body);
    switch (type) {
    case QMetaType_QVariantHash:
    case QMetaType_QVariantList:
    case QMetaType_QVariantMap:
    case QMetaType_QStringList:
        return QJsonDocument::fromVariant(p.request_body).array().toVariantList();
    default:
        return QJsonDocument::fromJson(p.request_body.toByteArray()).array().toVariantList();
    }
}

Request &Request::Body::rq()
{
    dPvt();
    return*p.parent;
}

}
