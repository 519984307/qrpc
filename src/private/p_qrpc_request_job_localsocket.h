#pragma once

#include "./p_qrpc_request_job_protocol.h"
#include "../qrpc_listen_request.h"
#include <QTcpSocket>
#include <QUrl>


namespace QRpc {

class QRPCRequestJobLocalSocket : public QRPCRequestJobProtocol
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit QRPCRequestJobLocalSocket(QObject*parent):QRPCRequestJobProtocol(parent){
    }

    Q_INVOKABLE ~QRPCRequestJobLocalSocket(){
    }

    QRPCRequestJobResponse*response=nullptr;
    QTcpSocket*m_socket=nullptr;
    QByteArray buffer;


    virtual bool call(QRPCRequestJobResponse*response)override{
        this->response=response;
        if(m_socket!=nullptr)
            delete m_socket;
        m_socket = new QTcpSocket();
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        QObject::connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred), this, &QRPCRequestJobLocalSocket::onReplyError);
#else
        QObject::connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error), this, &QRPCRequestJobLocalSocket::onReplyError);
#endif
        QObject::connect(m_socket, &QTcpSocket::connected, this, &QRPCRequestJobLocalSocket::onConnected);
        QObject::connect(m_socket, &QTcpSocket::disconnected, this, &QRPCRequestJobLocalSocket::onClosed);
        QObject::connect(m_socket, &QTcpSocket::readChannelFinished, this, &QRPCRequestJobLocalSocket::onFinish);
        QUrl url(this->response->request_url.toUrl());
        const QString hostName=url.host(QUrl::FullyDecoded);
        const int port=url.port();
        m_socket->connectToHost(hostName, port);
        return true;
    }

private slots:

    void onConnected(){
        this->buffer.clear();
        if(!m_socket->waitForConnected(response->activityLimit)){
            response->response_qt_status_code=QNetworkReply::TimeoutError;
            response->response_status_code=501;
        }
        else{
            QRPCListenRequest request(response->request_body);
            auto body=request.toJson();
            m_socket->write(body);
            m_socket->flush();
            if(!m_socket->waitForDisconnected(response->activityLimit)){
                response->response_qt_status_code=QNetworkReply::TimeoutError;
                response->response_status_code=501;
            }
            else{
                this->buffer.append(m_socket->readAll());
                if(request.fromJson(this->buffer)){
                    response->response_qt_status_code=QNetworkReply::NoError;
                    response->response_status_code=200;
                    response->response_body=this->buffer;
                }
                m_socket->close();
            }
        }
        this->onFinish();
    };

    void onClosed(){
        //emit this->callback(QVariant());
    };

    void onReplyError(QAbstractSocket::SocketError e){
        Q_UNUSED(e)
        if(e!=QAbstractSocket::SocketError::RemoteHostClosedError){
            response->response_qt_status_code=QNetworkReply::UnknownServerError;
            this->onFinish();

        }
    };

//    void onReplyTimeout(){
//        response->response_qt_status_code=QNetworkReply::TimeoutError;
//        emit this->callback(QVariant());
//    };

//    void onReplyPong(quint64 elapsedTime, const QByteArray &payload){
//        Q_UNUSED(elapsedTime)
//        Q_UNUSED(payload)
//    }
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
};

}
