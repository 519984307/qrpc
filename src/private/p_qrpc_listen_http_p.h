#ifndef QRPCListenHTTP_P_H
#define QRPCListenHTTP_P_H

#include <QWebSocket>
#include <QWebSocketServer>
#include "../qrpc_listen.h"
#include "httprequesthandler.h"
#include "httplistener.h"
#include "./p_qrpc_listen_http.h"

namespace QRpc {


class QRPCListenWebS:public QObject{
    Q_OBJECT
public:
    Q_INVOKABLE explicit QRPCListenWebS(QRPCListen*parent):QObject(parent){
    }
    Q_INVOKABLE  ~QRPCListenWebS(){
    }
private:
    QList<QWebSocket*> socketClients;
    QWebSocketServer* socketServer = nullptr;
public slots:

    void onNewConnection()
    {
        auto socket = socketServer->nextPendingConnection();

        connect(socket, &QWebSocket::textMessageReceived, this, &QRPCListenWebS::processTextMessage);
        connect(socket, &QWebSocket::binaryMessageReceived, this, &QRPCListenWebS::processBinaryMessage);
        connect(socket, &QWebSocket::disconnected, this, &QRPCListenWebS::socketDisconnected);
        this->socketClients << socket;
    }

    void processTextMessage(QString message)
    {
        Q_UNUSED(message)
        //auto bytes=message.toUtf8();
        //emit LogAgent::instance().receivedLog(bytes);
    }

    void processBinaryMessage(QByteArray bytes)
    {
        Q_UNUSED(bytes)
        //emit LogAgent::instance().receivedLog(bytes);
    }

    void socketDisconnected()
    {
        auto socket = qobject_cast<QWebSocket*>(sender());
        if (socket!=nullptr) {
            this->socketClients.removeAll(socket);
            socket->deleteLater();
        }
    }

    void onServerStart(){
        this->onServerStop();
        this->socketServer = new QWebSocketServer(QStringLiteral("SSL Echo Server"), QWebSocketServer::NonSecureMode, this);
        if (this->socketServer->listen(QHostAddress::Any, 8080)) {
            connect(this->socketServer, &QWebSocketServer::newConnection,this, &QRPCListenWebS::onNewConnection);
        }
    }

    void onServerStop(){
        for(auto&client:this->socketClients){
            client->close(QWebSocketProtocol::CloseCodeNormal);
        }
        if(this->socketServer!=nullptr){
            this->socketServer->disconnect();
            delete this->socketServer;
            this->socketServer=nullptr;
        }
    }

};



}

#endif
