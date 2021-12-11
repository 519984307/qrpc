#include "./p_qrpc_listen_wss.h"

#include <QWebSocket>
#include <QWebSocketServer>
#include <QSslConfiguration>
#include <QFile>
#include <QSslCertificate>
#include <QSslKey>
#include "./qrpc_macro.h"
#include "./qrpc_server.h"
#include "./qrpc_listen.h"
#include "./qrpc_listen_colletion.h"
#include "./qrpc_listen_protocol.h"
#include "./qrpc_listen_request.h"
#include "./qrpc_listen_request_cache.h"


namespace QRpc {

class WebSocketServer:public QObject{
    //Q_OBJECT
public:
    QHash<int,QWebSocketServer*> servers;

    QList<QWebSocket*> clients;
    QMap<QUuid, QWebSocket*> clientsMap;
    /**
     * @brief listen
     * @return
     */
    QRPCListenWebSocket&listen(){
        auto _listen=dynamic_cast<QRPCListenWebSocket*>(this->parent());
        return*_listen;
    }

    explicit WebSocketServer(QObject*parent=nullptr):QObject(parent){
    }

    bool start(){
        bool RETURN=false;

        auto&protocol=this->listen().colletions()->protocol(QRPCProtocol::WebSocket);

        this->stop();

        if(protocol.enabled()){
            for(auto&sport:protocol.port()){
                auto port=sport.toInt();
                if(port>0){
                    QFile certFile(qsl(":/sslconfiguration/rpc.cert"));
                    QFile keyFile(qsl(":/sslconfiguration/rpc.key"));

                    if(!certFile.open(QIODevice::ReadOnly)){
                        sWarning()<<tr("LocalServerListener: Cannot load certfile : %1").arg(certFile.fileName());
                        continue;
                    }

                    if(!keyFile.open(QIODevice::ReadOnly)){
                        sWarning()<<tr("LocalServerListener: Cannot load keyfile : %s").arg(keyFile.fileName());
                        continue;
                    }

                    QSslCertificate certificate(&certFile, QSsl::Pem);
                    QSslKey sslKey(&keyFile, QSsl::Rsa, QSsl::Pem);
                    certFile.close();
                    keyFile.close();
                    QSslConfiguration sslConfiguration;
                    sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
                    sslConfiguration.setLocalCertificate(certificate);
                    sslConfiguration.setPrivateKey(sslKey);

                    auto server = new QWebSocketServer(qsl("SSL QRpcServer"),  QWebSocketServer::NonSecureMode, this);

                    connect(server, &QWebSocketServer::newConnection,this, &WebSocketServer::onServerNewConnection);
                    connect(server, &QWebSocketServer::closed, this, &WebSocketServer::onServerClosed);

                    if (!server->listen(QHostAddress(QHostAddress::LocalHost), port)) {
                        sWarning()<<tr("LocalServerListener: Cannot bind on port %1: %2").arg(port).arg(server->errorString());
                        server->close();
                        server->deleteLater();
                    }
                    else if(!server->isListening()){
                        sWarning()<<tr("LocalServerListener: Cannot bind on port %1: %2").arg(port).arg(server->errorString());
                        server->close();
                        server->deleteLater();
                    }
                    else{
                        RETURN=true;
                        sDebug()<<QString("LocalServerListener: Listening on port %1").arg(port);
                        this->servers.insert(port, server);
                    }

                }
            }
        }

        if(!RETURN){
            this->stop();
        }

        return RETURN;

    }

    bool stop(){
        {
            auto aux=this->clientsMap.values();
            this->clientsMap.clear();
            for(auto&client:aux){
                client->close();
            }
        }
        for(auto&server:this->servers){
            if(server!=nullptr){
                server->disconnect();
                delete server;
                server=nullptr;
            }
        }
        return true;
    };

    void onRpcFinish(QRPCListenRequest&request){
        if(request.isValid()){
            auto socket = this->clientsMap.value(request.requestUuid());
            if(socket!=nullptr){
                socket->sendBinaryMessage(request.toJson());
                socket->flush();
                //socket->close();//NO CLOSE
            }
            emit request.finish();
        }
    }

    void onRpcRequest(QRPCListenRequest&request){
        if(!request.isValid()){
            request.co().setBadRequest();
            this->onRpcFinish(request);
        }
        else{
            emit this->listen().rpcRequest(request.toHash(), QVariant());
        }

    }

public slots:



    void onRpcResponse(QUuid uuid, QVariantHash vRequest){
        auto&request=this->listen().cacheRequest()->toRequest(uuid);
        if(request.isValid()){
            if(!request.fromResponseMap(vRequest))
                request.co().setInternalServerError();
            onRpcFinish(request);
        }
    }


    void onServerNewConnection(){
        auto server=dynamic_cast<QWebSocketServer*>(QObject::sender());
        if(server!=nullptr){
            auto socket = server->nextPendingConnection();
            connect(socket, &QWebSocket::disconnected          , this, &WebSocketServer::onClientDisconnected);
            connect(socket, &QWebSocket::binaryMessageReceived , this, &WebSocketServer::onBodyBinary);
            connect(socket, &QWebSocket::textMessageReceived   , this, &WebSocketServer::onBodyText);
        }
    }

    void onServerClosed(){

    }

    void onBodyText(QString bytes){
        auto socket = qobject_cast<QWebSocket*>(QObject::sender());
        if (socket!=nullptr) {
            auto&request=this->listen().cacheRequest()->createRequest(bytes);
            this->clientsMap.insert(request.requestUuid(), socket);
            this->onRpcRequest(request);
        }
    }

    void onBodyBinary(QByteArray bytes) {
        auto socket = qobject_cast<QWebSocket*>(QObject::sender());
        if (socket!=nullptr) {
            auto&request=this->listen().cacheRequest()->createRequest(bytes);
            this->clientsMap.insert(request.requestUuid(), socket);
            this->onRpcRequest(request);
        }
    }

    void onClientDisconnected()    {
        auto socket = qobject_cast<QWebSocket*>(QObject::sender());
        if (socket!=nullptr) {
            auto key=this->clientsMap.key(socket);
            if(!key.isNull())
                this->clientsMap.remove(key);
            socket->deleteLater();
        }
    }

};



#define dPvt()\
    auto&p =*reinterpret_cast<QRPCListenWebSocketPvt*>(this->p)

class QRPCListenWebSocketPvt:public QObject{
public:
    WebSocketServer*_listenServer=nullptr;

    explicit QRPCListenWebSocketPvt(QRPCListenWebSocket*parent):QObject(parent){
        this->_listenServer = new WebSocketServer(parent);
    }
    virtual ~QRPCListenWebSocketPvt(){
        this->_listenServer->stop();
        delete this->_listenServer;
        this->_listenServer=nullptr;
    }
};

QRPCListenWebSocket::QRPCListenWebSocket(QObject *parent):QRPCListen(parent)
{
    this->p = new QRPCListenWebSocketPvt(this);
}

QRPCListenWebSocket::~QRPCListenWebSocket()
{
    dPvt();
    p._listenServer->stop();
    delete&p;
}

bool QRPCListenWebSocket::start()
{
    dPvt();
    this->stop();
    QRPCListen::start();
    p._listenServer = new WebSocketServer(this);
    connect(this, &QRPCListen::rpcResponse, p._listenServer, &WebSocketServer::onRpcResponse);
    return p._listenServer->start();
}

bool QRPCListenWebSocket::stop()
{
    dPvt();
    return p._listenServer->stop();
}

}

