#include "./qrpc_listen_colletion.h"
#include <QSettings>
#include <QDir>
#include <QMutex>
#include "./qrpc_server.h"
#include "./qrpc_listen.h"
#include "./qrpc_listen_protocol.h"

#include "./private/p_qrpc_server.h"
#include "./private/p_qrpc_listen_tcp.h"
#include "./private/p_qrpc_listen_udp.h"
#include "./private/p_qrpc_listen_wss.h"
#include "./private/p_qrpc_listen_broker_mqtt.h"
#include "./private/p_qrpc_listen_broker_amqp.h"
#include "./private/p_qrpc_listen_broker_kafka.h"
#include "./private/p_qrpc_listen_broker_database.h"
#include "./private/p_qrpc_listen_http.h"
#include "./private/p_qrpc_listen_qrpc.h"

namespace QRpc {

#define dPvt()\
    auto&p =*reinterpret_cast<QRPCListenColletionsPvt*>(this->p)

class QRPCListenColletionsPvt:public QObject{
public:
    QMutex lockMake;
    QMutex lockWaitRun;
    QMutex lockWaitQuit;
    QMutex lockRunning;
    QHash<int,QRPCListen*> listensActive;
    QRPCListenProtocols listenProtocol;
    QRPCServer*server=nullptr;
    QVariantHash settings;

    QRPCListenColletions *collections(){
        auto collections=dynamic_cast<QRPCListenColletions*>(this->parent());
        return collections;
    }

    explicit QRPCListenColletionsPvt(QRPCServer*server, const QVariantHash&settings, QRPCListenColletions*parent):QObject(parent){
        this->server=server;
        this->settings=settings;
        this->makeListens();
    }

    ~QRPCListenColletionsPvt(){
    }

    void setSettings(const QVariantHash&settings){
        this->settings=settings;
        this->loadSettings();
    }

    void loadSettings(){
        auto settingsDefault = this->settings.value(qsl("default")).toHash();
        for(auto&v : this->listenProtocol){
            auto optionName=v->optionName();
            auto settings = this->settings.value(optionName).toHash();
            if(!settings.isEmpty()){
                v->setSettings(settings, settingsDefault);
            }
        }

        auto list=this->listenProtocol.value(0);
        if(list!=nullptr)
            list->setEnabled(true);
    }

    void makeListens(){
        this->makeOption(0, QRPCListenQRPC::staticMetaObject);
        this->makeOption(QRPCProtocol::TcpSocket, QRPCListenTCP::staticMetaObject);
        this->makeOption(QRPCProtocol::UdpSocket, QRPCListenUDP::staticMetaObject);
        this->makeOption(QRPCProtocol::WebSocket, QRPCListenWebSocket::staticMetaObject);
        this->makeOption(QRPCProtocol::Mqtt, QRPCListenBrokerMQTT::staticMetaObject);
        //this->makeOption(QRPCProtocol::DataBase, QRPCListenBrokerDataBase::staticMetaObject);
        this->makeOption(QRPCProtocol::Amqp, QRPCListenBrokerAMQP::staticMetaObject);
        this->makeOption(QRPCProtocol::Kafka, QRPCListenBrokerKAFKA::staticMetaObject);
        this->makeOption(QRPCProtocol::Http, QRPCListenHTTP::staticMetaObject);
        this->makeOption(QRPCProtocol::Https, QRPCListenHTTP::staticMetaObject);
        this->loadSettings();
    }

    bool makeOption(int protocol, const QMetaObject&metaObject){
        QMutexLocker<QMutex> locker(&this->lockMake);
        if(this->listenProtocol.contains(protocol))
            return true;
        else{
            auto option = new QRPCListenProtocol(protocol, metaObject, this->parent());
            option->setObjectName(qsl("set_%1").arg(QString::fromUtf8(option->protocolName())));
            this->listenProtocol.insert(option->protocol(), option);
            return true;
        }
    }


    void listenClear(){
        if(!this->listensActive.isEmpty()){
            auto aux=this->listensActive.values();
            this->listensActive.clear();
            for(auto&listen:aux){
                if(listen->isRunning()){
                    listen->quit();
                    listen->wait();
                    listen->deleteLater();
                }
            }
        }
    }

    void listenStart(){
        this->listenClear();
        auto listenProtocol=this->listenProtocol.values();
        for(auto&protocol:listenProtocol){
            if(protocol->enabled()){
                auto listen=protocol->makeListen();
                if(listen!=nullptr){
                    listen->setServer(this->server);
                    listen->setColletions(this->collections());
                    this->listensActive.insert(protocol->protocol(), listen);
                }
            }
        }

        auto listenPool=this->collections()->listenPool();
        if(listenPool==nullptr){
            qFatal("invalid pool");
        }
        else{
            for(auto&listen:this->listensActive){
                listenPool->registerListen(listen);
                listen->setServer(this->server);
                listen->setColletions(this->collections());
            }
        }

        for(auto&listen:this->listensActive){
            if(listen!=nullptr){
                listen->start();
            }
        }
        if(!this->lockWaitRun.tryLock(10))
            this->lockWaitRun.unlock();
        else
            this->lockWaitRun.unlock();
    }

    void listenQuit(){
        this->listenClear();
        this->lockRunning.tryLock(1);
        this->lockRunning.unlock();
    }

};

QRPCListenColletions::QRPCListenColletions(QRPCServer *server):QThread(nullptr)
{
    this->p = new QRPCListenColletionsPvt(server, QVariantHash(),this);
}

QRPCListenColletions::QRPCListenColletions(const QVariantHash&settings, QRPCServer *server)
{
    this->p = new QRPCListenColletionsPvt(server, settings, this);
}

QRPCListenColletions::~QRPCListenColletions()
{
    dPvt();
    delete&p;
}

QRPCListenProtocol &QRPCListenColletions::protocol()
{
    return this->protocol(QRPCProtocol::Http);
}

QRPCListenProtocol &QRPCListenColletions::protocol(const QRPCProtocol &protocol)
{
    if((protocol>=rpcProtocolMin) && (protocol<=rpcProtocolMax)){
        dPvt();
        auto&listenProtocol=p.listenProtocol;
        auto ___return=listenProtocol.value(protocol);
        if(___return==nullptr){
            if(protocol==Http || protocol==Https){
                auto __protocol=(protocol==Http)?Https:protocol;
                ___return=listenProtocol.value(__protocol);
            }
        }
        if(___return!=nullptr)
            return*___return;
    }
    static QRPCListenProtocol __protocol;
    return __protocol;
}

QRPCListenProtocols &QRPCListenColletions::protocols()
{
    dPvt();
    return p.listenProtocol;
}

void QRPCListenColletions::run()
{
    dPvt();
    p.listenStart();
    this->exec();
    p.listenQuit();
    p.lockWaitQuit.tryLock(1);
    p.lockWaitQuit.unlock();
}

void QRPCListenColletions::requestEnabled()
{
    //criar mutex de controle nos listens
}

void QRPCListenColletions::requestDisable()
{
    //criar mutex de controle nos listens
}

QRPCServer *QRPCListenColletions::server()
{
    dPvt();
    return p.server;
}

void QRPCListenColletions::setSettings(const QVariantHash &settings)const
{
    dPvt();
    return p.setSettings(settings);
}

QRPCListenQRPC *QRPCListenColletions::listenPool()
{
    dPvt();
    QHashIterator<int, QRPCListen*> i(p.listensActive);
    while (i.hasNext()) {
        i.next();
        if(i.value()!=nullptr){
            auto listen = dynamic_cast<QRPCListenQRPC*>(i.value());
            if(listen!=nullptr)
                return listen;
        }
    }
    return nullptr;
}

bool QRPCListenColletions::start()
{
    dPvt();
    bool __return=false;
    if(p.lockRunning.tryLock(1000)){
        p.lockWaitRun.lock();
        QThread::start();
        QMutexLocker<QMutex> locker(&p.lockWaitRun);
        __return=this->isRunning();
    }
    return __return;
}

bool QRPCListenColletions::stop()
{
    return this->quit();
}

bool QRPCListenColletions::quit()
{
    dPvt();
    p.lockWaitQuit.lock();
    QMutexLocker<QMutex> lockerRun(&p.lockWaitRun);//evitar crash antes da inicializacao de todos os listainers
    p.listenQuit();
    QThread::quit();
    QMutexLocker<QMutex> lockerQuit(&p.lockWaitQuit);
    QThread::wait();
    return true;
}

}
