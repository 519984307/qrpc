#include "./p_qrpc_listen_tcp.h"


#include <QTcpServer>
#include <QSslConfiguration>
#include <QFile>
#include <QSslCertificate>
#include <QSslKey>
#include "../qrpc_listen.h"
#include "../qrpc_listen_colletion.h"
#include "../qrpc_listen_protocol.h"
#include "../qrpc_listen_request.h"
#include "../qrpc_listen_request_cache.h"
#include "../qrpc_server.h"


namespace QRpc {

class ServerTCPSocket:public QObject{
public:
    QByteArray buffer;
    QHash<int,QTcpServer*> servers;
    QMap<QUuid, QTcpSocket*> clientsMap;
    /**
     * @brief listen
     * @return
     */
    QRPCListen&listen(){
        auto _listen=dynamic_cast<QRPCListen*>(this->parent());
        return*_listen;
    }

    explicit ServerTCPSocket(QObject*parent):QObject(parent){
    }

    bool start(){
        bool RETURN=false;

        auto&protocol=this->listen().colletions()->protocol(QRPCProtocol::TcpSocket);
        //auto port=protocol.port();
        this->stop();

        if(protocol.enabled()){
            for(auto&sport:protocol.port()){
                auto port=sport.toInt();
                if(port>0){
                    auto server = new QTcpServer(this);
                    connect(server, &QTcpServer::newConnection,this, &ServerTCPSocket::onServerNewConnection);
                    if (!server->listen(QHostAddress::LocalHost, port)) {
                        sWarning()<<tr("WebsocketListener: Cannot bind on port %1: %2").arg(port).arg(server->errorString());
                        server->close();
                        server->deleteLater();
                    }
                    else{
                        if(server->isListening()){
                            RETURN=true;
                            sDebug()<<QString("TcpsocketListener: Listening on port %i").arg(port);
                            this->servers.insert(port, server);
                        }
                        else{
                            sWarning()<<tr("TcpsocketListener: Cannot bind on port %1: %2").arg(port).arg(server->errorString());
                            server->close();
                            server->deleteLater();
                        }
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

public slots:

    void onRpcFinish(QRpc::QRPCListenRequest*request){
        Q_UNUSED(request)
        if(request->isValid()){
            auto socket = this->clientsMap.value(request->requestUuid());
            if(socket!=nullptr){
                socket->write(request->toJson());
                socket->flush();
                QThread::msleep(5);
                socket->close();
            }
            emit request->finish();
        }
    }

    void onRpcResponse(const QUuid&uuid, const QVariantHash&vRequest){
        auto&request=this->listen().cacheRequest()->toRequest(uuid);
        if(request.isValid()){
            if(!request.fromResponseMap(vRequest))
                request.co().setInternalServerError();
            this->onRpcFinish(&request);
        }
    }

    void onRpcRequest(QRpc::QRPCListenRequest*request){
        Q_UNUSED(request)
        if(!request->isValid()){
            request->co().setBadRequest();
            this->onRpcFinish(request);
        }
        else{
            emit this->listen().rpcRequest(request->toHash(), QVariant());
        }
    }

    void onServerNewConnection(){
        auto server=dynamic_cast<QTcpServer*>(QObject::sender());
        if(server!=nullptr){
            auto socket = server->nextPendingConnection();
            connect(socket, &QTcpSocket::disconnected, this, &ServerTCPSocket::deleteLater);
            connect(socket, &QTcpSocket::disconnected, this, &ServerTCPSocket::onClientDisconnected);
            while(socket->waitForReadyRead()){
                this->buffer.append(socket->readAll());
                QRPCListenRequest request;
                if(request.fromJson(this->buffer))
                    break;
            }
            auto&request=this->listen().cacheRequest()->createRequest(this->buffer);
            this->clientsMap.insert(request.requestUuid(), socket);
            this->onRpcRequest(&request);
        }
    }

    void onClientDisconnected()
    {
        auto socket = qobject_cast<QTcpSocket*>(QObject::sender());
        if (socket!=nullptr) {
            auto key=this->clientsMap.key(socket);
            if(!key.isNull())
                this->clientsMap.remove(key);
            socket->deleteLater();
        }
    }

};

#define dPvt()\
    auto&p =*reinterpret_cast<QRPCListenTCPPvt*>(this->p)

class QRPCListenTCPPvt:public QObject{
public:
    ServerTCPSocket*_listenServer=nullptr;

    explicit QRPCListenTCPPvt(QRPCListenTCP*parent):QObject(parent){
        this->_listenServer = new ServerTCPSocket(parent);
    }
    virtual ~QRPCListenTCPPvt(){
        this->_listenServer->stop();
        delete this->_listenServer;
        this->_listenServer=nullptr;
    }
};

QRPCListenTCP::QRPCListenTCP(QObject *parent):QRPCListen(parent)
{
    this->p = new QRPCListenTCPPvt(this);
}

QRPCListenTCP::~QRPCListenTCP()
{
    dPvt();
    p._listenServer->stop();
    delete&p;
}

bool QRPCListenTCP::start()
{
    dPvt();
    connect(this, &QRPCListen::rpcResponse, p._listenServer, &ServerTCPSocket::onRpcResponse);
    return p._listenServer->start();
}

bool QRPCListenTCP::stop()
{
    dPvt();
    return p._listenServer->stop();
}

}
