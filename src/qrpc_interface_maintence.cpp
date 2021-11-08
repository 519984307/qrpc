#include "qrpc_interface_maintence.h"
#include "qrpc_server.h"
#include <QCoreApplication>

namespace QRpc {

QRPCInterfaceMaintence::QRPCInterfaceMaintence(QObject *parent) : QRPCInterfaceCheck(parent)
{
    this->setEnabled(true);
}

QRPCInterfaceMaintence::~QRPCInterfaceMaintence()
{

}

QVariant QRPCInterfaceMaintence::serverStart()
{
    this->server()->interfacesStart();
    this->rq().co().setOK();
    return {};
}

QVariant QRPCInterfaceMaintence::serverStop()
{
    this->server()->interfacesStop();
    this->rq().co().setOK();
    return {};
}

QVariant QRPCInterfaceMaintence::serverRestart()
{
    this->server()->start();
    this->server()->stop();
    this->rq().co().setOK();
    return {};
}

QVariant QRPCInterfaceMaintence::servicesCheck()
{
    this->server()->start();
    this->server()->stop();
    this->rq().co().setOK();
    return {};
}

QVariant QRPCInterfaceMaintence::applicationQuit()
{
    this->server()->stop();
    qApp->quit();
    this->rq().co().setOK();
    return {};
}

} // namespace QRpc
