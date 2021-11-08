#include "./qrpc_interface_check.h"
#include <QDate>

namespace QRpc {

QRPCInterfaceCheck::QRPCInterfaceCheck(QObject *parent) : QRpc::QRPCController(parent)
{

}

QRPCInterfaceCheck::~QRPCInterfaceCheck()
{

}

QVariant QRPCInterfaceCheck::ping()
{
    this->rq().co().setOK();
    return QDateTime::currentDateTime();
}

QVariant QRPCInterfaceCheck::fullCheck()
{
    this->rq().co().setOK();
    return {};
}

QVariant QRPCInterfaceCheck::connectionsCheck()
{
    this->rq().co().setOK();
    return {};
}

QVariant QRPCInterfaceCheck::businessCheck()
{
    this->rq().co().setNotImplemented();
    return {};
}

} // namespace QRpc
