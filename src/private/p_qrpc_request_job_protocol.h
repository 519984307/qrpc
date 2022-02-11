#pragma once

#include "./p_qrpc_request_job_response.h"
#include <QTemporaryFile>

namespace QRpc {

//!
//! \brief The QRPCRequestJobProtocol class
//!
class QRPCRequestJobProtocol : public QObject
{
    Q_OBJECT
public:
    QRPCRequest::Action action=QRPCRequest::acRequest;
    QString action_fileName;
    QSslConfiguration sslConfiguration;

    Q_INVOKABLE explicit QRPCRequestJobProtocol(QObject*parent);

    ~QRPCRequestJobProtocol();

    virtual QRPCRequestJobProtocol&clear();

    virtual bool call(QRPCRequestJobResponse*response);

signals:
    void callback(const QVariant&vHash);
    void ____verify(const QVariant&v);
public slots:
    virtual void ____onVerify(const QVariant&v);
};

}
