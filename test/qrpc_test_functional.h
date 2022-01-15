#pragma once

#include "./qrpc_test.h"
#include "../src/qrpc_server.h"
#include "../src/qrpc_types.h"
#include "../src/qrpc_request.h"
#include "../src/qrpc_listen_protocol.h"
#include "../src/qrpc_controller.h"

namespace QRpc {

class Server : public QRpc::QRPCServer
{
    Q_OBJECT
    Q_DECLARE_INSTANCE(Server)
public:
    Q_INVOKABLE explicit Server(QObject *parent = nullptr):QRpc::QRPCServer(parent)
    {
    }
    ~Server()
    {
    }
private:
    void*p=nullptr;
};

static const QString default_route_rpc="/";
class Q_RPC_Interface : public QRpc::QRPCController
{
    Q_OBJECT
    Q_DECLARE_OBJECT()
    QRPC_DECLARE_ROUTE(InterfaceAgent, default_route_rpc)
    Q_RPC_DECLARE_INTERFACE_METHOD_CHECK()
public:
    Q_INVOKABLE explicit Q_RPC_Interface(QObject *parent=nullptr):QRpc::QRPCController(parent)
    {
    }

    ~Q_RPC_Interface()
    {
    }

    Q_INVOKABLE QVariant check_header()
    {
        QRPC_METHOD_CHECK_HEADER();
        return QDate::currentDate();
    }

    Q_INVOKABLE QVariant check_get()
    {
        QRPC_METHOD_CHECK_GET();
        return QDate::currentDate();
    }

    Q_INVOKABLE QVariant check_post()
    {
        QRPC_METHOD_CHECK_POST();
        return QDate::currentDate();
    }

    Q_INVOKABLE QVariant check_put()
    {
        QRPC_METHOD_CHECK_PUT();
        return QDate::currentDate();
    }

    Q_INVOKABLE QVariant check_delete()
    {
        QRPC_METHOD_CHECK_DELETE();
        return QDate::currentDate();
    }
};

QRPC_CONTROLLER_AUTO_REGISTER(Q_RPC_Interface)

struct PublicRecord{
public:
    int server_port_http=9999;
    QByteArray server_protocol="http";
    QByteArray server_hostname="localhost";
    QByteArray server_broker_database_driver="QPSQL";
    QByteArray server_broker_database_username="erpv1";
    QByteArray server_broker_database_password="erpv1";
    int server_broker_database_port=5432;
};

Q_GLOBAL_STATIC(PublicRecord, __public_record);
static auto&public_record=*__public_record;

class SDKGoogleTestFunctional : public SDKGoogleTest{
public:

    PublicRecord&publicRecord=public_record;
    static QRpc::Server&service()
    {
        auto&instance=QRpc::Server::instance();
        if(!instance.isRunning()){
            instance.setObjectName(QT_STRINGIFY(QRpcServer));
            {
                auto&protocol=instance.colletions().protocol(QRpc::Http);
                protocol.setEnabled(true);
                protocol.setPort(public_record.server_port_http);
            }
        }
        return instance;
    }

    explicit SDKGoogleTestFunctional():SDKGoogleTest()
    {
    }

    virtual bool serviceStart()
    {
        auto __return=this->service().start();
        QThread::sleep(1);
        return __return;
    }

    virtual bool serviceStop()
    {
        return this->service().stop();
    }

public:
};

}
