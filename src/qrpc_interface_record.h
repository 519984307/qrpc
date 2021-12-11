#pragma once

#include "./qrpc_interface_check.h"

namespace QRpc {

/**
 * @brief The QRPCInterfaceRecord class
 *
 * GET
 * http://localhost/RequestPath/ping
 *
 * POST/PUT
 * http://localhost/RequestPath/fullCheck
 * http://localhost/RequestPath/connectionsCheck
 * http://localhost/RequestPath/businessCheck
 *
 */
class QRPCInterfaceRecord : public QRpc::QRPCController
{
    Q_OBJECT
    Q_RPC_DECLARE_INTERFACE_METHOD_CHECK()
public:
    Q_INVOKABLE explicit QRPCInterfaceRecord(QObject *parent = nullptr);
    Q_INVOKABLE ~QRPCInterfaceRecord();

    Q_INVOKABLE virtual QVariant get();
    Q_INVOKABLE virtual QVariant set();
    Q_INVOKABLE virtual QVariant del();
    Q_INVOKABLE virtual QVariant list();
signals:
};

}
