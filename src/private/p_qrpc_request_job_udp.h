#pragma once

#include "./p_qrpc_request_job_protocol.h"
#include "../qrpc_listen_request.h"
#include <QUdpSocket>
#include <QUrl>

namespace QRpc {

//!
//! \brief The QRPCRequestJobUDP class
//!
class QRPCRequestJobUDP : public QRPCRequestJobProtocol
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit QRPCRequestJobUDP(QObject*parent);

    ~QRPCRequestJobUDP();

    QRPCRequestJobResponse*response=nullptr;
    QUdpSocket*m_socket=nullptr;
    QByteArray buffer;


    virtual bool call(QRPCRequestJobResponse*response)override;

private slots:

    void onConnected();;

    void onClosed();;

    void onReplyError(QAbstractSocket::SocketError e);;

//    void onReplyTimeout(){
//        response->response_qt_status_code=QNetworkReply::TimeoutError;
//        emit this->callback(QVariant());
//    };

//    void onReplyPong(quint64 elapsedTime, const QByteArray &payload){
//        Q_UNUSED(elapsedTime)
//        Q_UNUSED(payload)
//    }
    void onFinish();
};

}
