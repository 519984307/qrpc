#ifndef QRPC_AMQPThreadPublish_H
#define QRPC_AMQPThreadPublish_H

#include "./p_qrpc_amqp_thread.h"
#include <QMutex>

namespace QRpc {

class AMQPThreadPublish:public AMQPThread{
    Q_OBJECT
public:
    Q_INVOKABLE explicit AMQPThreadPublish(QObject* parent=nullptr):AMQPThread(nullptr){
        Q_UNUSED(parent)
//        QObject::connect(this, &AMQPThreadPublish::init, this, &AMQPThreadPublish::on_init);
//        QObject::connect(this, &AMQPThreadPublish::publish, this, &AMQPThreadPublish::on_publish);
    }
private:
    QMutex mutex;
    QString hostName;
    int port;
    QString userName;
    QString passWord;
    QString vHost;


    AMQPHandle*handle=nullptr;
    AMQP::Connection*connection=nullptr;
    QMap<QString,AMQP::Channel*> channelMap;

    QVariantList listMessage;


private slots:

    virtual void on_ping(){
        if(this->handle!=nullptr){
            this->handle->go();
            QVariantList list;
            {
                QMutexLocker locker(&mutex);
                list=listMessage;
                listMessage.clear();
            }
            if(!list.isEmpty()){
                for(auto&v:list){
                    auto map=v.toMap();
                    auto queue = map.value("queue").toString();
                    auto body = map.value("body").toByteArray();

                    auto channel=channelMap.value(queue);
                    if(channel==nullptr){
                        channel=new AMQP::Channel(this->connection);
                        channelMap.insert(queue, channel);
                        if(channel->usable()){
                            channel->declareExchange(queue.toStdString(), AMQP::fanout);
                            channel->declareQueue(queue.toStdString(), AMQP::fanout);
                            channel->bindQueue(queue.toStdString(), queue.toStdString(), queue.toStdString());
                        }
                    }
                    if(channel->usable()){
                        channel->publish("", queue.toStdString(), body);
                    }
                }
                this->handle->go();
            }
        }
    }

    void on_publish(const QString&queue, const QByteArray&body){
        QMutexLocker locker(&mutex);
        QVariantMap map;
        map.insert("queue",queue);
        map.insert("body",body);
        this->listMessage<<map;
    }

    void on_init(const QString&hostName, const int port, const QString&userName, const QString&passWord, const QString&vHost){
        this->hostName = hostName;
        this->port     = port    ;
        this->userName = userName;
        this->passWord = passWord;
        this->vHost    = vHost   ;

        if(this->handle==nullptr){
            this->handle=new AMQPHandle(hostName.toStdString(), port);
            this->connection=new AMQP::Connection(handle, AMQP::Login(userName.toStdString(), passWord.toStdString()), vHost.toStdString());
            this->connection->waiting();
        }
    }

signals:
    void init(const QString&hostName, const int port, const QString&userName, const QString&passWord, const QString&vHost);
    void publish(const QString&queue, const QByteArray&body);

};


}
#endif
