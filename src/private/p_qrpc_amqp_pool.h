#ifndef QRPC_AMQPPool_H
#define QRPC_AMQPPool_H

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <memory>
#ifdef COMPILE_AMQP
#include <amqpcpp.h>
#include <amqpcpp/message.h>
#endif
#include "./p_qrpc_amqp_thread_publish.h"
#include "./p_qrpc_amqp_thread_consumer.h"

namespace QRpc {

class AMQPPool:public QThread{
    Q_OBJECT
public:

    Q_INVOKABLE explicit AMQPPool(QObject* parent=nullptr):QThread(nullptr){
        Q_UNUSED(parent)
        QObject::connect(this, &AMQPPool::publish, this, &AMQPPool::on_publish);
        //QObject::connect(this, &AMQPPool::consumer, this, &AMQPPool::on_consumer);
    }

    virtual ~AMQPPool(){
    }


    AMQPPool&start(){
        if(!this->isRunning()){
            QThread::start();
            while(this->eventDispatcher()==nullptr)
                QThread::msleep(10);
        }
        return*this;
    }

    void run(){
        this->exec();
        for(auto&t:listThreadPublish){
            t->quit();
            t->wait();
            t->deleteLater();
        }
        for(auto&t:listThreadConsumer){
            t->quit();
            t->wait();
            t->deleteLater();
        }
        listThreadPublish.clear();
        listThreadConsumer.clear();
    }

    QMap<QString, AMQPThreadPublish*> listThreadPublish;
    QMap<QString, AMQPThreadConsumer*> listThreadConsumer;

private slots:
    void on_publish(const QString&hostName, const int port, const QString&userName, const QString&passWord, const QString&queue, const QString&vHost, const QByteArray&body){
        auto hash=QString("%1.%2.%3.%4.%5").arg(hostName).arg(port).arg(userName).arg(passWord).arg(vHost).toUtf8();
        hash=QCryptographicHash::hash(hash, QCryptographicHash::Md5).toHex();

        auto thread=this->listThreadPublish.value(hash);
        if(thread==nullptr){
            thread=new AMQPThreadPublish();
            listThreadPublish.insert(hash, thread);
            thread->start();
            while(thread->eventDispatcher()==nullptr)
                QThread::msleep(1);
            emit thread->init(hostName, port, userName, passWord, vHost);
        }
        emit thread->publish(queue, body.toHex());
    }

    void on_consumer(const QString&hostName, const int port, const QString&userName, const QString&passWord, const QString&queue){
        auto hash=QString("%1.%2.%3.%4.%5").arg(hostName).arg(port).arg(userName).arg(passWord).arg(queue).toUtf8();
        hash=QCryptographicHash::hash(hash, QCryptographicHash::Md5).toHex();

        auto thread=this->listThreadConsumer.value(hash);
        if(thread==nullptr){
            thread=new AMQPThreadConsumer();
            QObject::connect(thread, &AMQPThreadConsumer::receive, this, &AMQPPool::on_consumer_dispatcher);
            QObject::connect(this, &AMQPPool::ack, thread, &AMQPThreadConsumer::on_ack);
            listThreadConsumer.insert(hash, thread);
            thread->start();
            while(thread->eventDispatcher()==nullptr)
                QThread::msleep(1);
            emit thread->init(hostName, port, userName, passWord, queue);
        }
    }

    void on_consumer_dispatcher(const QUuid&uuid, const QVariantMap&vMap){
        emit dispatcher(uuid, vMap);
    }

signals:
    void publish(const QString&hostName, const int port, const QString&userName, const QString&passWord, const QString&queue, const QString&vHost, const QByteArray&body);
    void consumer(const QString&hostName, const int port, const QString&userName, const QString&passWord, const QString&queue);
    void dispatcher(const QUuid&uuid, const QVariantMap&vMap);
    void ack(const QUuid&uuid);
};


}
#endif
