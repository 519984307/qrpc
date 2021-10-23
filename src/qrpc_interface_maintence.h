#pragma once

#include "./qrpc_interface_check.h"

namespace QRpc {

/**
 * @brief The QRPCInterfaceMaintence class
 *
 * GET
 * http://localhost:80/serverStats
 *
 * POST/PUT
 * http://localhost/v1/maintence/serverStart
 * http://localhost/v1/maintence/serverStop
 * http://localhost/v1/maintence/serverRestart
 * http://localhost/v1/maintence/servicesCheck
 * http://localhost/v1/maintence/applicationQuit
 *
 */
class QRPCInterfaceMaintence : public QRPCInterfaceCheck
{
    Q_OBJECT
    QRPC_DECLARE_ROUTE(QRPCInterfaceCheck,"v1/maintence")
public:
    Q_INVOKABLE explicit QRPCInterfaceMaintence(QObject *parent = nullptr);
    Q_INVOKABLE ~QRPCInterfaceMaintence();

    /**
     * @brief serverStart
     * @return
     */
    Q_INVOKABLE virtual QVariant serverStart();

    /**
     * @brief serverStop
     * @return
     */
    Q_INVOKABLE virtual QVariant serverStop();

    /**
     * @brief serverRestart
     * @return
     */
    Q_INVOKABLE virtual QVariant serverRestart();

    /**
     * @brief servicesCheck
     * @return
     */
    Q_INVOKABLE virtual QVariant servicesCheck();

    /**
     * @brief applicationQuit
     * @return
     */
    Q_INVOKABLE virtual QVariant applicationQuit();
signals:

};

QRPC_CONTROLLER_AUTO_REGISTER(QRPCInterfaceMaintence)

} // namespace QRpc
