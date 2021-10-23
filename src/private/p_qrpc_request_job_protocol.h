#pragma once

#include "./p_qrpc_request_job_response.h"
#include <QTemporaryFile>

namespace QRpc {

//typedef std::function<bool(const QVariantHash&vMap)> QRPCRequestJobProtocolCallBack;
class QRPCRequestJobProtocol : public QObject
{
    Q_OBJECT
public:
    QRPCRequest::Action action=QRPCRequest::acRequest;
    QString action_fileName;
    QSslConfiguration sslConfiguration;
    Q_INVOKABLE explicit QRPCRequestJobProtocol(QObject*parent):QObject(parent){
        connect(this, &QRPCRequestJobProtocol::____verify, this, &QRPCRequestJobProtocol::____on_verify);
    }

    Q_INVOKABLE ~QRPCRequestJobProtocol(){
    }

    virtual bool call(QRPCRequestJobResponse*response){
        response->response_status_code=QNetworkReply::UnknownServerError;
        response->response_body.clear();
        emit this->callback(QVariant());
        return false;
    }

signals:
    void callback(const QVariant&vMap);
    void ____verify(const QVariant&v);
public slots:
    virtual void ____on_verify(const QVariant&v){
        Q_UNUSED(v)
    }
};

}
