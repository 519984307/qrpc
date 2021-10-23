#include "./p_qrpc_listen_udp.h"
#include "../qrpc_listen_colletion.h"
#include "../qrpc_listen_protocol.h"
#include "../qrpc_listen_request.h"
#include "../qrpc_server.h"
#include <QUdpSocket>
#include <QNetworkDatagram>

namespace QRpc {

class ServerUDPSocket:public QObject{
public:
    QHash<int,QUdpSocket*> servers;
    /**
     * @brief listen
     * @return
     */
    QRPCListen&listen(){
        auto _listen=dynamic_cast<QRPCListen*>(this->parent());
        return*_listen;
    }

    explicit ServerUDPSocket(QObject*parent):QObject(parent){
    }

    bool start(){
        bool RETURN=true;

        auto&protocol=this->listen().colletions()->protocol(QRPCProtocol::UdpSocket);
        this->stop();

        if(protocol.enabled()){
            for(auto&sport:protocol.port()){
                auto port=sport.toInt();
                if(port>0){
                    auto server = new QUdpSocket(this);
                    connect(server, &QUdpSocket::readyRead, this, &ServerUDPSocket::onClientConnected);
                    if(!server->bind(QHostAddress::LocalHost, port)){
                        RETURN=false;
                        break;
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

    void sendRequest(const QByteArray&body){
        QVariantHash request;
        request.insert("protocol","udp");
        request.insert("body",body);
        emit this->listen().rpcRequest(request, QVariant());
    }

    void onClientConnected(){
        QByteArray body;
        auto server=dynamic_cast<QUdpSocket*>(QObject::sender());
        if(server!=nullptr){
            while (server->hasPendingDatagrams()) {
                auto datagram = server->receiveDatagram();
                body.append(datagram.data());
            }
            this->sendRequest(body);
            server->close();
        }
    }

    void onClientDisconnected()
    {
        auto socket = qobject_cast<QUdpSocket*>(QObject::sender());
        if (socket!=nullptr) {
            socket->deleteLater();
        }
    }

};

#define dPvt()\
    auto&p =*reinterpret_cast<QRPCListenUDPPvt*>(this->p)

class QRPCListenUDPPvt:public QObject{
public:
    ServerUDPSocket*_listenServer=nullptr;

    explicit QRPCListenUDPPvt(QRPCListenUDP*parent):QObject(parent){
        this->_listenServer = new ServerUDPSocket(parent);
    }
    virtual ~QRPCListenUDPPvt(){
        this->_listenServer->stop();
        delete this->_listenServer;
        this->_listenServer=nullptr;
    }
};

QRPCListenUDP::QRPCListenUDP(QObject *parent):QRPCListen(parent)
{
    this->p = new QRPCListenUDPPvt(this);
}

QRPCListenUDP::~QRPCListenUDP()
{
    dPvt();
    p._listenServer->stop();
    delete&p;
}

bool QRPCListenUDP::start()
{
    dPvt();
    return p._listenServer->start();
}

bool QRPCListenUDP::stop()
{
    dPvt();
    return p._listenServer->stop();
}

}
