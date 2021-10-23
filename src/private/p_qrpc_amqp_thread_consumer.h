#ifndef QRPC_AMQPThreadConsumer_H
#define QRPC_AMQPThreadConsumer_H

#include "./p_qrpc_amqp_thread.h"
#include "../qrpc_listen_request.h"

namespace QRpc {

class AMQPThreadConsumer:public AMQPThread{
    Q_OBJECT
public:
    Q_INVOKABLE explicit AMQPThreadConsumer(QObject* parent=nullptr):AMQPThread(nullptr){
        Q_UNUSED(parent)
        QObject::connect(this, &AMQPThreadConsumer::init, this, &AMQPThreadConsumer::on_init);
    }

    QMap<QString, uint64_t> uuidList;


    void run(){

        this->channel=nullptr;
        this->connection=nullptr;
        this->handle=nullptr;

        AMQPThread::run();

        if(this->handle!=nullptr){
            this->channel->close();
            this->connection->close();
            this->handle->quit();

            delete this->channel;
            delete this->connection;
            delete this->handle;
        }
        this->channel=nullptr;
        this->connection=nullptr;
        this->handle=nullptr;
    }


private:
    AMQPHandle*handle=nullptr;
    AMQP::Connection*connection=nullptr;
    AMQP::Channel*channel=nullptr;

public slots:

    virtual void on_ping(){
        if(this->channel!=nullptr){
            if(this->channel->usable()){
                this->handle->go();
            }
        }
    }


    void on_ack(const QUuid&uuid){
        auto suuid=uuid.toString();
        if(uuidList.contains(suuid)){
            auto deliveryTag=this->uuidList.take(suuid);
            this->channel->ack(deliveryTag);
        }
    }


    void on_init(const QString&hostName, const int port, const QString&userName, const QString&passWord, const QString&queue){

        Q_UNUSED(hostName    );
        Q_UNUSED(port        );
        Q_UNUSED(userName    );
        Q_UNUSED(passWord    );
        Q_UNUSED(queue       );

        if(this->handle==nullptr){
            this->handle=new AMQPHandle(hostName.toStdString(), port);
            this->connection=new AMQP::Connection(this->handle, AMQP::Login(userName.toStdString(), passWord.toStdString()));
            this->channel=new AMQP::Channel(this->connection);
            channel->declareExchange(queue.toStdString(), AMQP::fanout);
            channel->declareQueue(queue.toStdString(), AMQP::fanout);
            channel->bindQueue(queue.toStdString(), queue.toStdString(), queue.toStdString());
            if(!channel->usable()){
                this->quit();
            }
            else{
                this->channel->consume(queue.toStdString()/*, AMQP::noack*/).onReceived(
                            [this](const AMQP::Message &message,
                            uint64_t deliveryTag,
                            bool redelivered)
                {
                    Q_UNUSED(redelivered)
                    QRPCListenRequest request;
                    auto msg=QString(message.body()).trimmed();
                    QByteArray body=msg.toUtf8();
                    body=msg.mid(0,msg.indexOf(QChar(65533))).toUtf8();
                    if(!body.contains("{")){
                        body=QByteArray::fromHex(body);
                    }
                    else if(body.isEmpty()){
                        rCritical()<<"amqp : automatic ack: invalid body is empty";
                        rWarning()<<body;
                        this->channel->ack(deliveryTag);
                    }
                    else if(!request.fromJson(body)){
                        rCritical()<<"amqp : automatic ack: json parser error";
                        rWarning()<<body;
                        this->channel->ack(deliveryTag);
                    }
                    else{
                        auto uuid = request.requestUuid();
                        this->uuidList.insert(uuid.toString(), deliveryTag);
                        auto vMap = request.toMap();
                        emit this->receive(uuid, vMap);
                    }
                });
            }
        }
    }

signals:
    void init(const QString&hostName, const int port, const QString&userName, const QString&passWord, const QString&queue);
    void receive(const QUuid&uuid, const QVariantMap&vMap);

};


}
#endif
