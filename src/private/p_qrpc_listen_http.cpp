#include "./p_qrpc_listen_http.h"
#include <QCryptographicHash>
#include <QCoreApplication>
#include "./httprequesthandler.h"
#include "./httplistener.h"
#include "./staticfilecontroller.h"
#include "./httpsessionstore.h"
#include "./qrpc_server.h"
#include "./qrpc_macro.h"
#include "./qstm_url.h"
#include "./qrpc_listen_colletion.h"
#include "./qrpc_listen_protocol.h"
#include "./qrpc_listen_request.h"
#include "./qrpc_listen_request_cache.h"

namespace QRpc {

class HttpListeners3drparty{
public:

    explicit HttpListeners3drparty(){

    }

    virtual ~HttpListeners3drparty(){
        this->free();
    }

    QSettings*settings=nullptr;
    stefanfrings::HttpSessionStore *sessionStore=nullptr;
    stefanfrings::StaticFileController *fileController=nullptr;
    stefanfrings::HttpListener *listener=nullptr;

    static HttpListeners3drparty*make(stefanfrings::HttpRequestHandler*requestHandler, QSettings*settings){
        auto __return=new HttpListeners3drparty();
        __return->settings = settings;
        __return->listener = new stefanfrings::HttpListener(settings, requestHandler, requestHandler);
        __return->fileController = new stefanfrings::StaticFileController(settings, __return->listener);
        __return->sessionStore = new stefanfrings::HttpSessionStore(settings, __return->listener);
        QThread::msleep(100);
        return __return;
    };

    void free(){
        this->listener->close();
        this->listener->deleteLater();
        this->settings->deleteLater();

        this->settings=nullptr;
        this->listener=nullptr;
        this->listener=nullptr;
    }
};

class HttpServer3drparty:public stefanfrings::HttpRequestHandler{
public:
    QList<HttpListeners3drparty*> listenersPort;
    QMutex lock;
    bool realMessageOnException=false;

    explicit HttpServer3drparty(QObject* parent=nullptr):stefanfrings::HttpRequestHandler(parent){
        this->realMessageOnException=false;
        auto colletions=this->listen()->colletions();
        auto&option=colletions->protocol(QRPCProtocol::Http);
        for(auto&v:option.port()){
            auto port=v.toInt();
            if(port>0){
                auto settings=option.makeSettings(this);
                if(!this->realMessageOnException)
                    this->realMessageOnException=option.realMessageOnException();
                settings->setValue(qsl("port"),port);
                this->listenersPort<<HttpListeners3drparty::make(this, settings);
            }
        }
    }

    bool isListening(){
        QMutexLOCKER locker(&this->lock);
        for(auto&h:this->listenersPort){
            if(h->listener->isListening()){
                return true;
            }
        }
        return false;
    }

    virtual ~HttpServer3drparty(){
        QMutexLOCKER locker(&this->lock);
        auto aux=this->listenersPort;
        this->listenersPort.clear();
        qDeleteAll(aux);
    }

    QRPCListenHTTP*listen(){
        auto _listen=dynamic_cast<QRPCListenHTTP*>(this->parent());
        return _listen;
    }

    void service(stefanfrings::HttpRequest &req, stefanfrings::HttpResponse &ret){
        Q_UNUSED(req)
        Q_UNUSED(ret)
        const auto time_start=QDateTime::currentDateTime();

        const auto getHeaderMap=req.getHeaderMap();
        const auto getParameterMap=req.getParameterMap();

        QVariantHash requestHeaderMap;
        QVariantHash requestParameterMap;

        {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            QHashIterator<QByteArray, QByteArray> i(getHeaderMap);
#else
            QMultiHashIterator<QByteArray, QByteArray> i(getHeaderMap);
#endif

            while (i.hasNext()) {
                i.next();
                requestHeaderMap[i.key()]=i.value();
#if Q_RPC_LOG_SUPER_VERBOSE
                sInfo()<<"   header - "+i.key()+":"+i.value();
#endif
            }
        }

        {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            QHashIterator<QByteArray, QByteArray> i(getParameterMap);
#else
            QMultiHashIterator<QByteArray, QByteArray> i(getParameterMap);
#endif
            while (i.hasNext()) {
                i.next();
                requestParameterMap[i.key()]=i.value();
#if Q_RPC_LOG_SUPER_VERBOSE
                sInfo()<<"   parameter - "+i.key()+":"+i.value();
#endif
            }
        }

        auto listen=this->listen();
        auto&request=listen->cacheRequest()->createRequest();
        auto requestPath=QString(req.getPath());
        auto requestBody=QString(req.getBody()).trimmed();
        auto requestMethod=QString(req.getMethod()).toLower();

        request.setRequestProtocol(QRpc::Http);
        request.setRequestPath(requestPath.toUtf8());
        request.setRequestHeader(requestHeaderMap);
        request.setRequestParameter(requestParameterMap);
        request.setRequestMethod(requestMethod);
        request.setRequestBody(requestBody);


        auto vMap=request.toHash();
        QStringList uploadedFiles;
        {
            auto rList=req.uploadedFiles.values();
            for(auto&v:rList){
                uploadedFiles<<v->fileName();
            }
        }
        emit this->listen()->rpcRequest(vMap, QVariant(uploadedFiles));
        request.start();

        const auto mSecsSinceEpoch=double(QDateTime::currentDateTime().toMSecsSinceEpoch()-time_start.toMSecsSinceEpoch())/1000.000;
        const auto mSecs=QString::number(mSecsSinceEpoch,'f',3);
        const auto responseCode=request.responseCode();
        const auto responsePhrase=QString::fromUtf8(request.responsePhrase());

        QByteArray body;

        static const auto staticUrlNames=QVector<int>()<<QMetaType_QUrl<<QMetaType_QVariantMap<<QMetaType_QString << QMetaType_QByteArray<<QMetaType_QChar << QMetaType_QBitArray;
        const auto&responseBody=request.responseBody();
        Url rpc_url;
        if(!staticUrlNames.contains(qTypeId(responseBody)))
            body = request.responseBodyBytes();
        else if(!rpc_url.read(responseBody).isValid())
            body = request.responseBodyBytes();
        else if(rpc_url.isLocalFile()){
            QFile file(rpc_url.toLocalFile());
            if(!file.open(file.ReadOnly)){
                request.co().setNotFound();
            }
            else{
                body=file.readAll();
                file.close();
            }
        }

#if Q_RPC_LOG

        if(request.co().isOK()){
            auto msgOut=qsl("%1::%2:req:%3, ret: %4, %5 ms ").arg(QT_STRINGIFY2(QRpc::Server), requestMethod, requestPath).arg(responseCode).arg(mSecs);
            cInfo()<<msgOut;
        }
        else{
            auto msgOut=qsl("%1::%2:req:%3, ret: %4, %5 ms ").arg(QT_STRINGIFY2(QRpc::Server), requestMethod, requestPath).arg(responseCode).arg(mSecs);
            cWarning()<<msgOut;
            msgOut=qsl("ret=")+responsePhrase;
            cWarning()<<msgOut;
            {
                {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                    QHashIterator<QByteArray, QByteArray> i(getHeaderMap);
#else
                    QMultiHashIterator<QByteArray, QByteArray> i(getHeaderMap);
#endif
                    while (i.hasNext()) {
                        i.next();
                        cWarning()<<qsl("   header - %1 : %2").arg(i.key(), i.value());
                    }
                }
#if Q_RPC_LOG_SUPER_VERBOSE
                {
                    QHashIterator<QByteArray, QByteArray> i(getParameterMap);
                    while (i.hasNext()) {
                        i.next();
                        cWarning()<<qbl("   parameter - ")+i.key()+qbl(":")+i.value();
                    }
                }
#endif
#if Q_RPC_LOG_SUPER_VERBOSE
                cWarning()<<qsl("       method:%1").arg(requestMethod);
                cWarning()<<qbl("       body:")+requestBody;
#endif
            }
        }
#endif
        if(request.co().isOK() && rpc_url.isValid()){//se for download
            if(rpc_url.isLocalFile()){
                QHashIterator<QString, QVariant> i(request.responseHeader());
                while (i.hasNext()) {
                    i.next();
                    auto&k=i.key();
                    auto&v=i.value();
                    rpc_url.headers().insert(k,v);
                }

                QRPCHttpHeaders headers(rpc_url.headers());
                if(!headers.contentDisposition().isValid()){
                    auto fileName=rpc_url.toLocalFile().split(qsl("/")).last();
                    rpc_url.headers().insert(ContentDispositionName.toUtf8(), qsl("inline; filename=\"%1\"").arg(fileName).toUtf8());
                    if(!headers.contentType().isValid())
                        headers.setContentType(rpc_url.url());
                }
                request.setResponseHeader(rpc_url.headers());
            }
            else{
                ret.redirect(rpc_url.toString().toUtf8());
                return;
            }
        }
        if(ret.isConnected()){
            QHashIterator<QString, QVariant> i(request.responseHeader());
            while (i.hasNext()) {
                i.next();
                auto k=i.key().toUtf8();
                auto v=i.value().toByteArray();
                ret.setHeader(k, v);
            }
            if(this->realMessageOnException)
                ret.setStatus(request.responseCode(), request.responsePhrase());//mensagem do backend
            else
                ret.setStatus(request.responseCode(), request.responsePhrase(0));//mensagem padrao
            ret.write(body,true);
        }
    }

public slots:

    void onRpcResponse(QUuid uuid,const QVariantHash&vRequest){
        auto&request=this->listen()->cacheRequest()->toRequest(uuid);
        if(request.isValid()){
            if(!request.fromResponseMap(vRequest)){
                request.co().setInternalServerError();
            }
            emit request.finish();
        }
    }
};


#define dPvt()\
    auto&p =*reinterpret_cast<QRPCListenHTTPPvt*>(this->p)

class QRPCListenHTTPPvt:public QObject{
public:
    HttpServer3drparty*_listenServer=nullptr;
    QRPCListenHTTP*parent=nullptr;

    explicit QRPCListenHTTPPvt(QRPCListenHTTP*parent):QObject(parent){
        this->parent=parent;
    }

    virtual ~QRPCListenHTTPPvt(){
    }

    bool start(){
        auto&p=*this;
        p._listenServer = new HttpServer3drparty(this->parent);
        QObject::connect(this->parent, &QRPCListen::rpcResponse, p._listenServer, &HttpServer3drparty::onRpcResponse);
        return p._listenServer->isListening();
    }

    bool stop(){
        auto&p=*this;
        if(p._listenServer!=nullptr){
            QObject::disconnect(this->parent, &QRPCListen::rpcResponse, p._listenServer, &HttpServer3drparty::onRpcResponse);
            delete p._listenServer;
        }
        p._listenServer=nullptr;
        return true;
    }
};

QRPCListenHTTP::QRPCListenHTTP(QObject *parent):QRPCListen(parent)
{
    this->p = new QRPCListenHTTPPvt(this);
}

QRPCListenHTTP::~QRPCListenHTTP()
{
    dPvt();
    p.stop();
    delete&p;
}

bool QRPCListenHTTP::start(){
    dPvt();
    QRPCListen::start();
    return p.start();
}

bool QRPCListenHTTP::stop(){
    QRPCListen::stop();
    dPvt();
    return p.stop();
}

}
