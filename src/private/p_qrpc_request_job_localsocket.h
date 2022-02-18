#pragma once

#include "./p_qrpc_request_job_protocol.h"
#include "../qrpc_listen_request.h"
#include <QTcpSocket>
#include <QUrl>

namespace QRpc {

//!
//! \brief The QRPCRequestJobLocalSocket class
//!
class QRPCRequestJobLocalSocket : public QRPCRequestJobProtocol
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit QRPCRequestJobLocalSocket(QObject*parent);

    ~QRPCRequestJobLocalSocket();

    QRPCRequestJobResponse*response=nullptr;
    QTcpSocket*m_socket=nullptr;
    QByteArray buffer;


    virtual bool call(QRPCRequestJobResponse*response)override;

private slots:

    void onConnected();;

    void onReplyError(QAbstractSocket::SocketError e);;

    void onFinish();
};

}
