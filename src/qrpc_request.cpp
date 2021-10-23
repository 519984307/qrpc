#include "./qrpc_request.h"
#include "./private/p_qrpc_request.h"
#include "./private/p_qrpc_listen_request_code.h"

namespace QRpc {



QRPCRequest::QRPCRequest(QObject *parent):QObject(parent)
{
    this->p = new QRPCRequestPvt(this);
}

//QRPCRequest::QRPCRequest(const ServiceSetting &setting, QObject *parent):QObject(parent)
//{
//    this->p = new QRPCRequestPvt(this);
//    dPvt();
//    p.setSettings(setting);
//}

QRPCRequest::~QRPCRequest()
{
    dPvt();delete&p;
}

bool QRPCRequest::startsWith(const QString &requestPath, const QVariant &requestPathBase)
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

        if(startWith){
            if(path.startsWith(route))
                return true;
        }
        else{
            if((path==route))
                return true;
        }

    }

    return false;
}

bool QRPCRequest::startsWith(const QString &requestPath)
{
    return this->startsWith(this->route(), requestPath);
}

bool QRPCRequest::isEqual(const QString &requestPath)
{
    return this->isEqual(requestPath, this->route());
}

bool QRPCRequest::isEqual(const QString &requestPath, const QVariant &requestPathBase)
{
    return QRPCRequest::startsWith(requestPath,requestPathBase);
}

bool QRPCRequest::canRequest() const
{
    if(this->port().toInt()<0)
        return false;
    else if(this->hostName().isEmpty())
        return false;
    else
        return true;
}

QRPCRequest &QRPCRequest::setSettings(const ServiceSetting &setting)
{
    dPvt();
    p.setSettings(setting);
    return*this;
}

QRPCRequest &QRPCRequest::setSettings(const QVariantHash &setting)
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

QString QRPCRequest::url() const
{
    return this->url(qsl_null);
}

QString QRPCRequest::url(const QString &path) const
{
    auto&rq=*this;

    auto spath=path.trimmed().isEmpty()?this->route().trimmed():path.trimmed();
    spath=qsl("/%1").arg(spath);
    while(spath.contains(qsl("//")))
        spath=spath.replace(qsl("//"),"/");
    if(path.isEmpty())
        return qsl("%1://%2:%3").arg(rq.protocolName(),rq.hostName(),rq.port().toString());
    else
        return qsl("%1://%2:%3%4").arg(rq.protocolName(),rq.hostName(),rq.port().toString(),spath);
}

QRPCProtocol QRPCRequest::protocol() const
{
    dPvt();
    return p.exchange.call().protocol();
}

QString QRPCRequest::protocolName() const
{
    dPvt();
    return p.exchange.call().protocolName();
}

QRPCRequest &QRPCRequest::setProtocol(const QVariant &value)
{
    dPvt();
    p.exchange.setProtocol(value);
    return*this;
}

QRPCRequestMethod QRPCRequest::method() const
{
    dPvt();
    return QRPCRequestMethod(p.exchange.call().method());
}

QRPCRequest &QRPCRequest::setMethod(const QString &value)
{
    dPvt();
    p.exchange.setMethod(value);
    return*this;
}

QRPCRequest &QRPCRequest::setMethod(const QByteArray &value)
{
    dPvt();
    p.exchange.setMethod(value);
    return*this;
}

QRPCRequest &QRPCRequest::setMethod(const int &value)
{
    dPvt();
    p.exchange.setMethod(QRPCRequestMethod(value));
    return*this;
}

QString QRPCRequest::driver() const
{
    dPvt();
    return p.exchange.call().driver();
}

QRPCRequest &QRPCRequest::setDriver(const QString &value)
{
    dPvt();
    p.exchange.call().setDriver(value);
    return*this;
}

QString QRPCRequest::hostName() const
{
    dPvt();
    return p.exchange.call().hostName();
}

QRPCRequest &QRPCRequest::setHostName(const QString &value)
{
    dPvt();
    p.exchange.setHostName(value);
    return*this;
}

QString QRPCRequest::userName() const
{
    dPvt();
    return p.exchange.call().userName();
}

QRPCRequest &QRPCRequest::setUserName(const QString &value)
{
    dPvt();
    p.exchange.call().setUserName(value);
    return*this;
}

QString QRPCRequest::password() const
{
    dPvt();
    return p.exchange.call().passWord();
}

QRPCRequest &QRPCRequest::setPassword(const QString &value)
{
    dPvt();
    p.exchange.call().setPassWord(value);
    return*this;
}

QString&QRPCRequest::route() const
{
    dPvt();
    return p.exchange.call().route();
}

QRPCRequest &QRPCRequest::setRoute(const QVariant &value)
{
    dPvt();
    p.exchange.call().setRoute(value);
    return*this;
}

QVariant QRPCRequest::body() const
{
    dPvt();
    return p.qrpcBody.body();
}

QRPCRequest &QRPCRequest::setBody(const QVariant &value)
{
    dPvt();
    p.qrpcBody.setBody(value);
    return*this;
}

QVariant QRPCRequest::port() const
{
    dPvt();
    return p.exchange.call().port();
}

QRPCRequest &QRPCRequest::setPort(const QVariant &value)
{
    dPvt();

    QVariant v;
     if(value.type()==v.StringList || value.type()==v.List){
        auto l=value.toList();
        v=l.isEmpty()?0:l.last().toInt();
    }
    else if(value.type()==v.Map || value.type()==v.Hash){
        auto l=value.toHash().values();
        v=l.isEmpty()?0:l.last().toInt();
    }
    else{
        v=value.toInt();
    }
    p.exchange.setPort(v.toInt());
    return*this;
}

qlonglong QRPCRequest::activityLimit() const
{
    dPvt();
    return p.exchange.call().activityLimit();
}

QRPCRequest &QRPCRequest::setActivityLimit(const QVariant &value)
{
    dPvt();
    p.exchange.call().setActivityLimit(value);
    return*this;
}

QRPCRequestExchange &QRPCRequest::exchange()
{
    dPvt();
    return p.exchange;
}

QRpc::QRPCHttpHeaders &QRPCRequest::header()
{
    dPvt();
    return p.qrpcHeader;
}

QRPCRequest::Body &QRPCRequest::body()
{
    dPvt();
    return p.qrpcBody;
}

QRPCHttpResponse &QRPCRequest::response()
{
    dPvt();
    return p.qrpcResponse;
}

LastError &QRPCRequest::lastError()
{
    dPvt();
    return p.qrpcLastError;
}

QRPCHttpResponse &QRPCRequest::call()
{
    dPvt();
    auto&e=p.exchange.call();
    return p.call(e.method(), e.route(), QByteArray());
}

QRPCHttpResponse &QRPCRequest::call(const QVariant &route)
{
    dPvt();
    auto&e=p.exchange.call();
    e.setRoute(route);
    return p.call(e.method(), e.route(), this->body().body());
}

QRPCHttpResponse &QRPCRequest::call(const QVariant &route, const QVariant &body)
{
    dPvt();
    auto&e=p.exchange.call();
    e.setRoute(route);
    return p.call(e.method(), e.route(), body);
}

QRPCHttpResponse &QRPCRequest::call(const QVariant &route, const QVariant&body, const QString &method, const QVariantHash parameter)
{
    dPvt();
    auto&e=p.exchange.call();
    e.setMethod(method);
    e.setRoute(route);
    e.setParameter(parameter);
    return p.call(e.method(), e.route(), body);
}

QRPCHttpResponse &QRPCRequest::call(const QRPCRequestMethod &method, const QString &route, const QVariant &body)
{
    dPvt();
    auto&e=p.exchange.call();
    e.setRoute(route);
    e.setMethod(method);
    return p.call(e.method(), e.route(), body);
}

QRPCHttpResponse &QRPCRequest::call(const QRPCRequestMethod &method)
{
    dPvt();
    auto&e=p.exchange.call();
    e.setMethod(method);
    auto&body=this->body().body();
    return p.call(e.method(), e.route(), body);
}

QRPCHttpResponse &QRPCRequest::call(const QVariant &route, const QObject &objectBody)
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

QRPCHttpResponse &QRPCRequest::call(const QRPCRequestMethod &method, const QString &route, const QObject &objectBody)
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

QRPCHttpResponse &QRPCRequest::call(const QVariant &route, QIODevice &ioDeviceBody)
{
    dPvt();
    auto body=ioDeviceBody.readAll();
    auto&e=p.exchange.call();
    e.setMethod(QRpc::Post);
    e.setRoute(route);
    return p.call(e.method(), e.route(), body);
}

QRPCHttpResponse &QRPCRequest::call(const QRPCRequestMethod &method, const QString &route)
{
    dPvt();
    auto&e=p.exchange.call();
    e.setRoute(route);
    e.setMethod(method);
    return p.call(e.method(), e.route(), this->body().body());
}

QRPCHttpResponse &QRPCRequest::call(const QRPCRequestMethod &method, const QString &route, QIODevice &ioDeviceBody)
{
    dPvt();
    auto body=ioDeviceBody.readAll();
    auto&e=p.exchange.call();
    e.setRoute(route);
    e.setMethod(method);
    return p.call(e.method(), e.route(), body);
}

QRPCRequest &QRPCRequest::operator=(const QRpc::ServiceSetting &value)
{
    this->setSettings(value);
    return*this;
}

QRPCHttpResponse &QRPCRequest::upload(QFile &file)
{
    dPvt();
    auto&e=p.exchange.call();
    e.setMethod(QRpc::Post);
    p.upload(e.route(), file.fileName());
    file.close();
    return this->response();
}

QRPCHttpResponse &QRPCRequest::upload(const QString &route, const QByteArray &buffer)
{
    dPvt();
    auto&e=p.exchange.call();
    e.setRoute(route);
    e.setMethod(QRpc::Post);
    QTemporaryFile file;
    if(!file.open())
        return this->response();
    else{
        file.write(buffer);
        file.flush();
        p.upload(e.route(), file.fileName());
        file.close();
        return this->response();
    }
}

QRPCHttpResponse &QRPCRequest::upload(const QString &route, QFile &file)
{
    dPvt();
    auto&e=p.exchange.call();
    e.setRoute(route);
    e.setMethod(QRpc::Post);
    p.upload(e.route(), file.fileName());
    file.close();
    return this->response();
}

QRPCHttpResponse &QRPCRequest::download(QString &fileName)
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

QRPCHttpResponse &QRPCRequest::download(const QString &route, QString &fileName)
{
    dPvt();
    auto _fileName=p.parseFileName(fileName);
    return this->download(QUrl(route), _fileName);
}

QRPCHttpResponse &QRPCRequest::download(const QUrl &route, QString &fileName)
{
    dPvt();
    auto _fileName=p.parseFileName(fileName);
    auto&e=p.exchange.call();
    e.setRoute(route.toString());
    e.setMethod(QRpc::Get);
    auto&response=p.download(e.route(), _fileName);
    if(response)
        fileName=_fileName;
    return response;
}

QRPCRequest &QRPCRequest::autoSetCookie()
{
    auto vMap=this->response().header().rawHeader();
    QHashIterator<QString, QVariant> i(vMap);
    auto cookies=this->header().cookies().toStringList();
    while (i.hasNext()){
        i.next();
        if(i.key().toLower().startsWith(qsl("set-cookie"))){
            auto cookieList=i.value().toString().split(qsl(";"));
            for(auto&cookie:cookieList){
                if(!cookies.contains(cookie))
                    cookies.append(cookie);
            }
        }
    }
    this->header().setCookies(cookies);
    return*this;
}

QString QRPCRequest::toString() const
{
    dPvt();\
    auto&response=p.qrpcResponse;
    auto qt_text=QRPCListenRequestCode::qt_network_error_phrase(p.response_qt_status_code);
    auto msg=qsl("%1:QtStatus: Status:%2, %3, %4").arg(p.exchange.call().url(), QString::number(response.qtStatusCode()),response.reasonPhrase(), qt_text);
    return msg;
}

QSslConfiguration &QRPCRequest::sslConfiguration()
{
    dPvt();
    return p.sslConfiguration;
}

QRPCRequest&QRPCRequest::setSslConfiguration(const QSslConfiguration &value)
{
    dPvt();
    p.sslConfiguration = value;
    return*this;
}

QSsl::SslProtocol QRPCRequest::sslProtocol() const
{
    dPvt();
    return p.sslConfiguration.protocol();
}

QRPCRequest&QRPCRequest::setSslProtocol(const QVariant &value)
{
    dPvt();
    auto protocol=p.sslConfiguration.protocol();
    if(value.toInt()>0 && QSslSslProtocolToName.contains(value.toInt()) )
        protocol=QSsl::SslProtocol(value.toInt());
    else if(QSslProtocolNameToProtocol.contains(value.toString().toLower()) )
        protocol=QSslProtocolNameToProtocol.value(value.toString().toLower());
    p.sslConfiguration.setProtocol(protocol);
    return*this;
}

QString &QRPCRequest::sslCertificate() const
{
    dPvt();
    return p.sslCertificate;
}

QRPCRequest&QRPCRequest::setSslCertificate(const QString &value)
{
    dPvt();
    p.sslCertificate = value;
    return*this;
}

QRPCRequest &QRPCRequest::print()
{
    for(auto&v:this->printOut())
        sInfo()<<v;
    return*this;
}

QStringList QRPCRequest::printOut()
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

QRPCRequest::Body::Body(QObject *parent):QObject(parent)
{
}

QRPCRequest::Body::~Body()
{
    
}

QVariant &QRPCRequest::Body::body() const
{
    dPvt();
    return p.request_body;
}

void QRPCRequest::Body::setBody(const QVariant &value)
{
    dPvt();
    p.request_body=value;
}

QString QRPCRequest::Body::toString()const
{
    dPvt();
    auto type=p.request_body.type();
    if(type==QVariant::List || type==QVariant::StringList || type==QVariant::Map || type==QVariant::Hash)
        return QJsonDocument::fromVariant(p.request_body).toJson();
    else
        return p.request_body.toString();
}

QVariantMap QRPCRequest::Body::toMap() const
{
    return QVariant(this->toHash()).toMap();
}

QVariantHash QRPCRequest::Body::toHash()const
{
    dPvt();
    auto type=p.request_body.type();
    if(type==QVariant::List || type==QVariant::StringList || type==QVariant::Map || type==QVariant::Hash)
        return QJsonDocument::fromVariant(p.request_body).object().toVariantHash();
    else
        return QJsonDocument::fromJson(p.request_body.toByteArray()).object().toVariantHash();
}

QVariantList QRPCRequest::Body::toList() const
{
    dPvt();
    auto type=p.request_body.type();
    if(type==QVariant::List || type==QVariant::StringList || type==QVariant::Map || type==QVariant::Hash)
        return QJsonDocument::fromVariant(p.request_body).array().toVariantList();
    else
        return QJsonDocument::fromJson(p.request_body.toByteArray()).array().toVariantList();
}

QRPCRequest &QRPCRequest::Body::rq()
{
    dPvt();
    return*p.parent;
}

}
