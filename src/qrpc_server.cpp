#include "./qrpc_server.h"
#include "./private/p_qrpc_server.h"

namespace QRpc {

#define dPvt()\
    auto&p =*reinterpret_cast<ServerPvt*>(this->p)

Server::Server(QObject *parent):QObject(parent)
{
    this->p = new ServerPvt(this);
}

Server::Server(const QVariant&settings, QObject *parent):QObject(parent)
{
    this->p = new ServerPvt(this);
    dPvt();
    p.v_load(settings);
}

Server::~Server()
{

}

ControllerOptions &Server::controllerOptions()
{
    dPvt();
    return p.controllerOptions;
}

bool Server::isFinished() const
{
    dPvt();
    return p.listenColletions->isFinished();
}

bool Server::isRunning()const
{
    dPvt();
    return p.listenColletions->isRunning();
}

bool Server::start()
{
    dPvt();

    this->controllers().clear();
    for(auto&v:Controller::staticApiList())
        this->controllers().append(v);

    this->parsers().clear();
    for(auto&v:Controller::staticApiParserList())
        this->parsers().append(v);

    return p.listenColletions->start();
}

bool Server::stop()
{
    dPvt();
    return p.listenColletions->quit();
}

Server&Server::interfacesStart()
{
    dPvt();
    p.enabledInterfaces=true;
    return*this;
}

Server&Server::interfacesStop()
{
    dPvt();
    p.enabledInterfaces=false;
    return*this;
}

bool Server::interfacesRunning()
{
    dPvt();
    return p.enabledInterfaces;
}

QList<const QMetaObject*> &Server::controllers()
{
    dPvt();
    return p.controllers;
}

QList<const QMetaObject *> &Server::parsers()
{
    dPvt();
    return p.parsers;
}

void Server::requestEnable()
{
    dPvt();
    p.listenColletions->requestEnabled();

}

void Server::requestDisable()
{
    dPvt();
    p.listenColletions->requestEnabled();
}

ListenColletions &Server::colletions()
{
    dPvt();
    return*p.listenColletions;
}

QVariant Server::settingsFileName()
{
    dPvt();
    return p.settingsFileName;
}

bool Server::setSettingsFileName(const QString &fileName)
{
    dPvt();
    return p.v_load(qvsl{fileName});
}

bool Server::setSettings(const QVariant &settings)const
{
    dPvt();
    return p.v_load(settings);
}

bool Server::authorizationRequest(const ListenRequest &request)const
{
    Q_UNUSED(request)
    return true;
}

QString Server::serverName() const
{
    dPvt();
    return p.serverName.isEmpty()?"Undef":p.serverName;
}

void Server::setServerName(const QString &value)
{
    dPvt();
    p.serverName = value;
}

}
