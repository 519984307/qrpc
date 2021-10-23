#ifndef QRPCRequestJobAMQP_H
#define QRPCRequestJobAMQP_H

#include "./p_qrpc_amqp_pool.h"

namespace QRpc {

class QRPCRequestJobAMQP : public QRPCRequestJobProtocol
{
    Q_OBJECT
public:

    static AMQPPool&___pool();

    QRPCRequestJobResponse*response=nullptr;
    QVariantMap vCallback;
    QDateTime limitTime=QDateTime::currentDateTime();

    Q_INVOKABLE explicit QRPCRequestJobAMQP(QObject*parent):QRPCRequestJobProtocol(parent){
        connect(&this->___pool(), &AMQPPool::dispatcher, this, &QRPCRequestJobAMQP::service);
    }

    Q_INVOKABLE ~QRPCRequestJobAMQP(){
    }

    bool call(QRPCRequestJobResponse*response){
        this->response=response;
        QVariantMap callBack;
        if(response->request_exchange.back().isValid())
            callBack=response->request_exchange.back().toMap();

        {
            QRPCRequestExchangeSetting e(response->request_exchange.back());
            const auto vHost=e.vHost();
            const auto hostName=e.hostName();
            const auto topic=e.topic();
            const auto userName=e.userName();
            const auto passWord=e.passWord();
            const uint16_t port=e.port();

            emit this->___pool().consumer(hostName, port, userName, passWord, topic);
        }

        {
            QRPCRequestExchangeSetting e(response->request_exchange.call());
            const auto vHost=e.vHost();
            const auto hostName=e.hostName();
            const auto topic=e.topic();
            const auto userName=e.userName();
            const auto passWord=e.passWord();
            const uint16_t port=e.port();

            QRPCListenRequest request(response->request_body);
            request.setResponseCallback(callBack);
            auto body=request.toJson();
            emit this->___pool().publish(hostName, port, userName, passWord, topic, vHost, body);
        }
        return true;
    }
public slots:

    void service(const QUuid&uuid, const QVariantMap&vBody){
        if(uuid==this->response->request_uuid){
            this->response->response_body=QRPCListenRequest(vBody).toJson();
            this->on_finish();
        }
    }


    void on_finish(){
        if(response->response_status_code!=QNetworkReply::TimeoutError){
            auto bodyMap=QJsonDocument::fromJson(response->response_body).object().toVariantMap();
            response->response_qt_status_code = QNetworkReply::NetworkError(vCallback["qt_status_code"].toInt());
            response->response_body = bodyMap["response_body"].toByteArray();
            response->response_status_code = bodyMap["status_code"].toInt();
            response->response_status_reason_phrase = bodyMap["reasonphrase"].toByteArray();
            if(response->response_status_code==0)
                response->response_status_code=response->response_qt_status_code;
        }
        emit this->callback(QVariant());
    }
};
}

#endif
