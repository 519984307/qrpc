#include "./p_qrpc_request_job_protocol.h"

namespace QRpc {

QRPCRequestJobProtocol::QRPCRequestJobProtocol(QObject *parent):QObject(parent), sslConfiguration(QSslConfiguration::defaultConfiguration())
{
    QObject::connect(this, &QRPCRequestJobProtocol::____verify, this, &QRPCRequestJobProtocol::____onVerify);
}

QRPCRequestJobProtocol::~QRPCRequestJobProtocol()
{
}

QRPCRequestJobProtocol &QRPCRequestJobProtocol::clear()
{
    return*this;
}

bool QRPCRequestJobProtocol::call(QRPCRequestJobResponse *response)
{
    response->response_status_code=QNetworkReply::UnknownServerError;
    response->response_body.clear();
    emit this->callback(QVariant());
    return false;
}

void QRPCRequestJobProtocol::____onVerify(const QVariant &v)
{
    Q_UNUSED(v)
}

}

