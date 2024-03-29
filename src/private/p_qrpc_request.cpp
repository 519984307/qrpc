#include "./p_qrpc_request.h"

namespace QRpc {

static bool static_log_register=false;
Q_GLOBAL_STATIC(QString, static_log_dir)

static void static_log_dir_clear(const QString &ormLogDir)
{
    QStringList dir_found;
    QStringList dir_rm_file;
    dir_found.append(ormLogDir);
    while(!dir_found.isEmpty()){
        auto scanDir = dir_found.takeFirst();
        dir_rm_file.append(scanDir);

        QDir dir(scanDir);
        if(!dir.exists(scanDir))
            continue;

        dir.setFilter(QDir::AllDirs);
        for(auto &scanInDir:dir.entryList()){
            if(scanInDir==qsl(".") || scanInDir==qsl(".."))
                continue;

            auto dir=qsl("%1/%2").arg(scanDir, scanInDir);
            dir_rm_file.append(dir);
            dir_found.append(dir);
        }
    }

    auto ext=QStringList{qsl("*.*")};
    for(auto &sdir:dir_rm_file){
        QDir scanDir(sdir);
        if(!scanDir.exists())
            continue;
        scanDir.setFilter(QDir::Drives | QDir::Files);
        scanDir.setNameFilters(ext);
        auto list=scanDir.entryList();
        for(auto &dirFile : list){
            auto fileName=sdir+qsl("/")+dirFile;
            QFile::remove(fileName);
        }
    }
}

static void static_log_init_dir()
{
    auto env = QString{getenv(qbl("Q_LOG_ENABLED"))}.trimmed();
#ifdef QT_DEBUG
    static_log_register = env.isEmpty()?true :QVariant{env}.toBool();
#else
    static_log_register = env.isEmpty()?false:QVariant{env}.toBool();
#endif
    if(!static_log_register)
        return;

    static const auto log_local_name=QString{__PRETTY_FUNCTION__}.split(qsl("::")).first().replace(qsl("void "),qsl_null).split(qsl_space).last();
    *static_log_dir=qsl("%1/%2/%3").arg(QDir::homePath(), log_local_name, qApp->applicationName());
    QDir dir(*static_log_dir);
    if(!dir.exists(*static_log_dir))
        dir.mkpath(*static_log_dir);
    if(dir.exists(*static_log_dir))
        static_log_dir_clear(*static_log_dir);
}

Q_COREAPP_STARTUP_FUNCTION(static_log_init_dir)


RequestPvt::RequestPvt(Request *parent):
    QObject{parent},
    exchange{parent},
    qrpcHeader{parent},
    qrpcBody{parent},
    qrpcResponse{parent},
    qrpcLastError{parent},
    sslConfiguration(QSslConfiguration::defaultConfiguration())
{
    auto currentName=QThread::currentThread()->objectName().trimmed();
    if(currentName.isEmpty())
        currentName=QString::number(qlonglong(QThread::currentThreadId()),16);
    if(static_log_register)
        this->fileLog=qsl("%1/%2.json").arg(*static_log_dir, QString::number(qlonglong(QThread::currentThreadId()),16));
    this->parent=parent;
    this->qrpcBody.p=this;
}

RequestPvt::~RequestPvt()
{
}

void RequestPvt::setSettings(const ServiceSetting &setting)
{
    auto __header=setting.headers();
    parent->header().setRawHeader(__header);
    parent->setProtocol(setting.protocol());
    parent->setPort(setting.port());
    parent->setDriver(setting.driverName());
    parent->setHostName(setting.hostName());
    parent->setMethod(setting.method());
    parent->setRoute(setting.route());
}

QString RequestPvt::parseFileName(const QString &fileName)
{
    auto _fileName=fileName.trimmed();
    if(_fileName.isEmpty()){
        QTemporaryFile file;
        file.open();
        _fileName=file.fileName();
        file.close();
    }
    return _fileName;
}

void RequestPvt::writeLog(RequestJobResponse &response, const QVariant &request)
{
    if(!static_log_register)
        return;

    if(!request.isValid())
        return;

    QFile file(this->fileLog);
    if (!file.open(QIODevice::Append | QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream outText(&file);
    auto &e=response.request_exchange.call();
    outText << RequestMethodName[e.method()]<<qsl(": ")<<response.request_url.toString()<<qsl("\n");
    outText << QJsonDocument::fromVariant(request).toJson(QJsonDocument::Indented);
    outText << qsl("\n");
    outText << qsl("\n");
    file.flush();
    file.close();
}

HttpResponse &RequestPvt::upload(const QString &route, const QString &fileName)
{
    this->qrpcLastError.clear();

    {//configuracao do contentType pela extencao
        auto content=this->qrpcHeader.contentType();
        if(!content.isValid() || content.isNull()){
            auto ext=qsl(".")+fileName.split(qsl(".")).last().toLower();
            if(!ContentTypeExtensionToHeader.contains(ext)){
                this->qrpcHeader.setContentType(QRpc::AppOctetStream);
            }
            else{
                auto contentType=ContentTypeExtensionToHeader.value(ext);
                this->qrpcHeader.setContentType(contentType);
            }
        }
    }
    {//configuracao do upload
        auto content=this->qrpcHeader.contentDisposition();
        if(!content.isValid() || content.isNull()){
            auto file=fileName.split(qsl("/")).last();
            auto value=qsl("form-data; name=\"%1\" filename=\"%1\"").arg(file);
            this->qrpcHeader.addRawHeader(ContentDispositionName, value);
        }
    }
    auto baseRoute = QString::fromUtf8(this->parent->baseRoute()).trimmed().replace(qsl("\""),qsl_null);
    auto routeCall=route.trimmed();
    if(!routeCall.startsWith(baseRoute))
        routeCall=qsl("/%1/%2").arg(baseRoute, route);

    auto &e=this->exchange.call();
    e.setMethod(QRpc::Post);
    e.setRoute(routeCall);

    switch (e.protocol()) {
    case QRpc::Http:
    case QRpc::Https:
    {
        auto e_port=e.port()==80?qsl_null:qsl(":%1").arg(e.port());
        auto request_url = qsl("%1%2/%3").arg(e.hostName(), e_port, e.route()).replace(qsl("\""),qsl_null).replace(qsl("//"), qsl("/"));
        while(request_url.contains(qsl("//")))
            request_url=request_url.replace(qsl("//"), qsl("/"));
        request_url = qsl("%1://%2").arg(e.protocolUrlName(), request_url.simplified());
        this->request_url=QUrl{request_url}.toString();
        break;
    }
    default:
        auto request_url = qsl("%1:%2").arg(e.hostName(),e.port()).replace(qsl("\""),qsl_null).replace(qsl("//"), qsl("/"));
        request_url = qsl("%1://%2").arg(e.protocolUrlName(), request_url.simplified());
        while(request_url.contains(qsl("//")))
            request_url=request_url.replace(qsl("//"), qsl("/"));
        this->request_url=QUrl{request_url};
    }

    auto job = RequestJob::newJob(Request::acUpload);
    QObject::connect(this, &RequestPvt::runJob, job, &RequestJob::onRunJob);
    job->start();
    emit runJob(&this->sslConfiguration, this->qrpcHeader.rawHeader(), this->request_url, fileName, this->parent);
    job->wait();
    QObject::disconnect(this, &RequestPvt::runJob, job, &RequestJob::onRunJob);
    this->qrpcResponse.setResponse(&job->response());
    this->writeLog(job->response(), job->response().toVariant());
    job->release();
    return this->qrpcResponse;
}

HttpResponse &RequestPvt::download(const QString &route, const QString &fileName)
{

    if(!this->qrpcHeader.contentType().isValid())
        this->qrpcHeader.setContentType(QRpc::AppOctetStream);
    this->qrpcHeader.addRawHeader(ContentDispositionName, qsl("form-data; name=%1; filename=%1").arg(fileName));

    this->qrpcLastError.clear();

    auto baseRoute = QString::fromUtf8(this->parent->baseRoute()).trimmed().replace(qsl("\""),qsl_null);

    auto routeCall=route.trimmed();
    if(!routeCall.startsWith(baseRoute))
        routeCall=qsl("/%1/%2").arg(baseRoute, route);

    auto &e=this->exchange.call();
    e.setMethod(QRpc::Get);
    e.setRoute(routeCall);
    auto &vBody=this->request_body;
    auto method=e.method();
    QMultiHash<QString,QVariant> paramsGet;
    switch (method) {
    case QRpc::Head:
    case QRpc::Get:
    case QRpc::Delete:
    case QRpc::Options:
    {
        Q_DECLARE_VU;
        paramsGet=vu.toMultiHash(vBody);
        vBody.clear();
        auto paramsGetOriginais = e.parameter();
        if (!paramsGetOriginais.isEmpty())
            paramsGet.unite(paramsGetOriginais);
        break;
    }
    default:
        break;
    }

    switch (e.protocol()) {
    case QRpc::Http:
    case QRpc::Https:
    {
        auto e_port=e.port()==80?qsl_null:qsl(":%1").arg(e.port());
        auto request_url_str = qsl("%1%2/%3").arg(e.hostName(), e_port, e.route()).replace(qsl("\""), qsl_null).replace(qsl("//"), qsl("/"));
        auto request_url_part = request_url_str.split(qsl("/"));
        request_url_str.clear();
        for(auto &line:request_url_part){
            if(line.trimmed().isEmpty())
                continue;
            if(!request_url_str.isEmpty())
                request_url_str+=qsl("/");
            request_url_str+=line;
        }
        request_url_str=qsl("%1://%2").arg(e.protocolUrlName(), request_url_str);
        this->request_url=QUrl(request_url_str);
        if(!paramsGet.isEmpty()){
            QUrlQuery url_query;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            QHashIterator<QString, QVariant> i(paramsGet);
#else
            QMultiHashIterator<QString, QVariant> i(paramsGet);
#endif
            while (i.hasNext()){
                i.next();
                const auto &k=i.key();
                const auto v=Util::parseQueryItem(i.value());
                url_query.addQueryItem(k, v);
            }
            QUrl url{request_url};
            url.setQuery(url_query);
            this->request_url=url;
        }
        break;
    }
    default:
        auto request_url = qsl("%1:%2").arg(e.hostName()).arg(e.port()).replace(qsl("\""),qsl_null).replace(qsl("//"), qsl("/"));
        request_url = qsl("%1://%2").arg(e.protocolUrlName(), request_url);
        this->request_url=QUrl{request_url};
    }

    auto job = RequestJob::newJob(Request::acDownload, fileName);
    QObject::connect(this, &RequestPvt::runJob, job, &RequestJob::onRunJob);
    job->start();
    emit runJob(&this->sslConfiguration, this->qrpcHeader.rawHeader(), this->request_url, fileName, this->parent);
    job->wait();
    QObject::disconnect(this, &RequestPvt::runJob, job, &RequestJob::onRunJob);
    this->qrpcResponse.setResponse(&job->response());
    this->writeLog(job->response(), job->response().toVariant());
    job->release();
    return this->qrpcResponse;
}

HttpResponse &RequestPvt::call(const RequestMethod &method, const QVariant &vRoute, const QVariant &body)
{
    this->qrpcLastError.clear();

    auto vBody=body;

    auto baseRoute = QString::fromUtf8(this->parent->baseRoute()).trimmed().replace(qsl("\""), qsl_null);
    auto route=vRoute.toString().trimmed();
    auto routeCall=route.trimmed();
    if(!routeCall.startsWith(baseRoute))
        routeCall=qsl("/%1/%2").arg(baseRoute,route);

    auto &e=this->exchange.call();
    e.setMethod(method);
    e.setRoute(routeCall);

    QMultiHash<QString,QVariant> paramsGet;
    switch (method) {
    case QRpc::Head:
    case QRpc::Get:
    case QRpc::Delete:
    case QRpc::Options:
    {
        Q_DECLARE_VU;
        paramsGet=vu.toMultiHash(vBody);
        vBody.clear();
        auto paramsGetOriginais = e.parameter();
        if (!paramsGetOriginais.isEmpty())
            paramsGet.unite(paramsGetOriginais);
        break;
    }
    default:
        break;
    }

    switch (e.protocol()) {
    case QRpc::Http:
    case QRpc::Https:
    {
        auto e_port=e.port()==80?qsl_null:qsl(":%1").arg(e.port());
        auto request_url_str = qsl("%1%2/%3").arg(e.hostName(), e_port, e.route()).replace(qsl("\""), qsl_null).replace(qsl("//"), qsl("/"));
        auto request_url_part = request_url_str.split(qsl("/"));
        request_url_str.clear();
        for(auto &line:request_url_part){
            if(line.trimmed().isEmpty())
                continue;
            if(!request_url_str.isEmpty())
                request_url_str+=qsl("/");
            request_url_str+=line;
        }
        request_url_str=qsl("%1://%2").arg(e.protocolUrlName(), request_url_str);
        this->request_url=QUrl{request_url_str};
        if(!paramsGet.isEmpty()){
            QUrlQuery url_query;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            QHashIterator<QString, QVariant> i(paramsGet);
#else
            QMultiHashIterator<QString, QVariant> i(paramsGet);
#endif
            while (i.hasNext()) {
                i.next();
                url_query.addQueryItem(i.key(), i.value().toString());
            }
            QUrl url{request_url};
            url.setQuery(url_query);
            this->request_url=url;
        }
        break;
    }
    case QRpc::DataBase:
    {
        auto topic=e.topic().trimmed();
        if(topic.isEmpty()){
            auto driver=e.driver();
            auto port=e.port();
            topic=qsl("broker-%1-%2").arg(driver).arg(port).toLower();
        }
        this->request_url=topic;
        break;
    }
    default:
        auto request_url = qsl("%1:%2").arg(e.hostName()).arg(e.port()).replace(qsl("\""), qsl_null).replace(qsl("//"),qsl("/"));
        request_url = qsl("%1://%2").arg(e.protocolUrlName(), request_url);
        this->request_url=QUrl{request_url};
    }

    switch (e.protocol()) {
    case QRpc::Http:
    case QRpc::Https:
    {
        switch (qTypeId(vBody)) {
        case QMetaType_QVariantHash:
        case QMetaType_QVariantList:
        case QMetaType_QStringList:
        case QMetaType_QVariantMap:
            this->request_body = QJsonDocument::fromVariant(vBody).toJson(QJsonDocument::Compact);
            break;
        default:
            this->request_body = vBody.toByteArray();
        }
        break;
    }
    case QRpc::DataBase:
    case QRpc::Kafka:
    case QRpc::Amqp:
    case QRpc::WebSocket:
    case QRpc::TcpSocket:
    case QRpc::UdpSocket:
    {
        switch (qTypeId(vBody)) {
        case QMetaType_QVariantHash:
        case QMetaType_QVariantList:
        case QMetaType_QStringList:
        case QMetaType_QVariantMap:
            this->request_body = QJsonDocument::fromVariant(vBody).toJson(QJsonDocument::Compact);
            break;
        default:
            this->request_body = vBody.toByteArray();
        }

        auto base=QUuid::createUuid().toString() + QDateTime::currentDateTime().toString();
        ListenRequest request;
        request.setRequestProtocol(e.protocol());
        request.setRequestUuid( QUuid::createUuidV3(QUuid::createUuid(), base.toUtf8()) );
        request.setRequestMethod(e.methodName().toUtf8().toLower());
        request.setRequestHeader(this->qrpcHeader.rawHeader());
        request.setRequestBody(vBody);
        request.setRequestPath(routeCall.toUtf8());
        this->request_body = request.toHash();
        break;
    }
    default:
        this->request_body = vBody;
    }
    const auto &requestRecovery=this->requestRecovery;
    auto job = RequestJob::newJob(Request::acRequest);
    QObject::connect(this, &RequestPvt::runJob, job, &RequestJob::onRunJob);
    int executeCount=0;
    forever{
        job=RequestJob::runJob(job);
        ++executeCount;
        emit runJob(&this->sslConfiguration, this->qrpcHeader.rawHeader(), this->request_url, qsl_null, this->parent);
        job->wait();

        if(job->response().response_qt_status_code==QNetworkReply::NoError)//if succes then break
            break;

        auto response_status_code=job->response().response_status_code;
        if(!requestRecovery.contains(response_status_code)){//if not check status code then check reason_phrase
            auto response_status_reason_phrase=job->response().response_status_reason_phrase.toLower().simplified();
            QHashIterator<int,int> i(requestRecovery);
            bool doContinue=false;
            while(i.hasNext()){
                i.next();
                //auto reason_phrase=ListenRequestCode::reasonPhrase(response_status_code).toLower().simplified();
                if(response_status_reason_phrase.contains(i.value())){
                    doContinue=true;
                    break;
                }
            }
            if(!doContinue)
                break;
        }

        auto recoveryCount=requestRecovery.contains(response_status_code);

        if(recoveryCount<=0)//if status code invalid then break
            break;

        if(recoveryCount<=executeCount)//if recovery limit then break
            break;
    }
    QObject::disconnect(this, &RequestPvt::runJob, job, &RequestJob::onRunJob);
    this->qrpcResponse.setResponse(&job->response());
    this->writeLog(job->response(), job->response().toVariant());
    job->release();

#ifdef Q_RPC_LOG
    if(!this->parent->response().isOk()){
        sWarning()<<this->parent->toString();
        this->qrpcHeader.print();
    }
#endif
    return this->qrpcResponse;
}

}
