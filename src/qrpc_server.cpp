#include "./qrpc_server.h"
#include "./private/p_qrpc_server.h"

namespace QRpc {

#define dPvt()\
    auto&p =*reinterpret_cast<QRPCServerPvt*>(this->p)

QRPCServer::QRPCServer(QObject *parent):QObject(parent)
{
    this->p = new QRPCServerPvt(this);
}

QRPCServer::QRPCServer(const QVariant&settings, QObject *parent):QObject(parent)
{
    this->p = new QRPCServerPvt(this);
    dPvt();
    p.v_load(settings);
}

QRPCServer::~QRPCServer()
{

}

ControllerOptions &QRPCServer::controllerOptions()
{
    dPvt();
    return p.controllerOptions;
}

bool QRPCServer::isFinished() const
{
    dPvt();
    return p.listenColletions->isFinished();
}

bool QRPCServer::isRunning()const
{
    dPvt();
    return p.listenColletions->isRunning();
}

bool QRPCServer::start()
{
    dPvt();

    this->controllers().clear();
    for(auto&v:QRPCController::staticInterfaceList())
        this->controllers().append(v);

    this->parsers().clear();
    for(auto&v:QRPCController::parserRequestList())
        this->parsers().append(v);

    return p.listenColletions->start();
}

bool QRPCServer::stop()
{
    dPvt();
    return p.listenColletions->quit();
}

QRPCServer&QRPCServer::interfacesStart()
{
    dPvt();
    p.enabledInterfaces=true;
    return*this;
}

QRPCServer&QRPCServer::interfacesStop()
{
    dPvt();
    p.enabledInterfaces=false;
    return*this;
}

bool QRPCServer::interfacesRunning()
{
    dPvt();
    return p.enabledInterfaces;
}

QList<const QMetaObject*> &QRPCServer::controllers()
{
    dPvt();
    return p.controllers;
}

QList<const QMetaObject *> &QRPCServer::parsers()
{
    dPvt();
    return p.parsers;
}

void QRPCServer::requestEnable()
{
    dPvt();
    p.listenColletions->requestEnabled();

}

void QRPCServer::requestDisable()
{
    dPvt();
    p.listenColletions->requestEnabled();
}

QRPCListenColletions &QRPCServer::colletions()
{
    dPvt();
    return*p.listenColletions;
}

QVariant QRPCServer::settingsFileName()
{
    dPvt();
    return p.settingsFileName;
}

bool QRPCServer::setSettings(const QVariant &settings)const
{
    dPvt();
    return p.v_load(settings);
}

bool QRPCServer::authorizationRequest(const QRPCListenRequest &request)const
{
    Q_UNUSED(request)
    return true;
}

QString QRPCServer::serverName() const
{
    dPvt();
    return p.serverName.isEmpty()?"Undef":p.serverName;
}

void QRPCServer::setServerName(const QString &value)
{
    dPvt();
    p.serverName = value;
}

}
