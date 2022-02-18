#pragma once

#include "./p_qrpc_request_job_protocol.h"
#include "../qrpc_macro.h"
#include "../qrpc_listen_request.h"
#include <QtWebSockets/QWebSocket>
#include <QFile>
#include <QSslConfiguration>
#include <QSslKey>
#include <QSslKey>

namespace QRpc {

//!
//! \brief The QRPCRequestJobWSS class
//!
class QRPCRequestJobWSS : public QRPCRequestJobProtocol
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit QRPCRequestJobWSS(QObject*parent);

    ~QRPCRequestJobWSS();

    QRPCRequestJobResponse*response=nullptr;
    QWebSocket*m_socket=nullptr;
    QByteArray buffer;


    virtual bool call(QRPCRequestJobResponse*response)override;

private slots:

    void onConnected();;

    void onClosed();;

    void onReplyError(QAbstractSocket::SocketError e);;

    void onBinaryMessageReceived(const QByteArray &message);;


    void onReplyTimeout();;

    void onReplyPong(quint64 elapsedTime, const QByteArray &payload);

    void onReplyReadFinished();;

    void onFinish();

    void onReplySslError(const QList<QSslError> &errors);
};

}
