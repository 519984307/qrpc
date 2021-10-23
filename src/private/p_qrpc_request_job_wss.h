#pragma once

#include "./p_qrpc_request_job_protocol.h"
#include "./qrpc_macro.h"
#include "./qrpc_listen_request.h"
#include <QWebSocket>
#include <QFile>
#include <QSslConfiguration>
#include <QSslKey>
#include <QSslKey>


namespace QRpc {

class QRPCRequestJobWSS : public QRPCRequestJobProtocol
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit QRPCRequestJobWSS(QObject*parent):QRPCRequestJobProtocol(parent){
    }

    Q_INVOKABLE ~QRPCRequestJobWSS(){
    }

    QRPCRequestJobResponse*response=nullptr;
    QWebSocket*m_socket=nullptr;
    QByteArray buffer;


    virtual bool call(QRPCRequestJobResponse*response){

        this->response=response;

        QFile certFile(qsl(":/sslconfiguration/rpc.cert"));
        QFile keyFile(qsl(":/sslconfiguration/rpc.key"));

        if(!certFile.open(QIODevice::ReadOnly)){
            sWarning()<<tr("WebsocketListener: Cannot load certfile : %1").arg(certFile.fileName());
        }
        else if(!keyFile.open(QIODevice::ReadOnly)){
            sWarning()<<tr("WebsocketListener: Cannot load keyfile : %s").arg(keyFile.fileName());
        }
        else{
            QSslCertificate certificate(&certFile, QSsl::Pem);
            QSslKey sslKey(&keyFile, QSsl::Rsa, QSsl::Pem);
            certFile.close();
            keyFile.close();
            QSslConfiguration sslConfiguration;
            sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
            sslConfiguration.setLocalCertificate(certificate);
            sslConfiguration.setPrivateKey(sslKey);

            QUrl url(response->request_url.toUrl());
            m_socket = new QWebSocket();
            QObject::connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &QRPCRequestJobWSS::onReplyError);
            QObject::connect(m_socket, &QWebSocket::connected, this, &QRPCRequestJobWSS::onConnected);
            QObject::connect(m_socket, &QWebSocket::disconnected, this, &QRPCRequestJobWSS::onClosed);
            QObject::connect(m_socket, &QWebSocket::binaryMessageReceived, this, &QRPCRequestJobWSS::onBinaryMessageReceived);
            QObject::connect(m_socket, &QWebSocket::pong, this, &QRPCRequestJobWSS::onReplyPong);
            QObject::connect(m_socket, &QWebSocket::readChannelFinished, this, &QRPCRequestJobWSS::onReplyReadFinished);
/*
            m_socket->setSslConfiguration(sslConfiguration);
*/
            m_socket->open(url);
            return true;
        }
        return false;

    }

private slots:

    void onConnected(){
        QRPCListenRequest request(response->request_body);
        auto body=request.toJson();
        m_socket->sendBinaryMessage(body);
    };

    void onClosed(){
        //emit this->callback(QVariant());
    };

    void onReplyError(QAbstractSocket::SocketError e){
        Q_UNUSED(e)
        response->response_qt_status_code=QNetworkReply::UnknownServerError;
        emit this->callback(QVariant());
    };

    void onBinaryMessageReceived(const QByteArray &message){
        auto socket=dynamic_cast<QWebSocket*>(QObject::sender());
        if(socket!=nullptr){
            Q_UNUSED(message)
            this->buffer.append(message);
            socket->close();
        }
    };


    void onReplyTimeout(){
        response->response_qt_status_code=QNetworkReply::TimeoutError;
        emit this->callback(QVariant());
    };

    void onReplyPong(quint64 elapsedTime, const QByteArray &payload){
        Q_UNUSED(elapsedTime)
        Q_UNUSED(payload)
    }

    void onReplyReadFinished(){
//        if(response->response_qt_status_code!=QNetworkReply::TimeoutError)
//            response->response_qt_status_code = this->reply->error();
        this->onFinish();
    };

    void onFinish(){
        QRPCListenRequest request(this->buffer);
        if(!request.isValid()){
            response->response_qt_status_code=QNetworkReply::InternalServerError;
            response->response_status_code=404;
            response->response_body.clear();
        }
        else if(response->response_status_code!=QNetworkReply::TimeoutError){
            auto code=request.responseCode();
            response->response_qt_status_code = QNetworkReply::NetworkError(code==200?0:code);
            response->response_body = request.responseBody().toByteArray();
            response->response_status_code = request.responseCode();
            response->response_status_reason_phrase = request.responsePhrase();
            if(response->response_status_code==0)
                response->response_status_code=response->response_qt_status_code;
        }

        emit this->callback(QVariant());
    }

    void onReplySslError(const QList<QSslError> &errors)
    {
        Q_UNUSED(errors);
#ifdef QT_DEBUG
        // WARNING: Never ignore SSL errors in production code.
        // The proper way to handle self-signed certificates is to add a custom root
        // to the CA store.
        m_socket->ignoreSslErrors();
#endif
    }
};

}
