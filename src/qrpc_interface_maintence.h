#pragma once

#include "./qrpc_controller.h"

namespace QRpc {

//!
//! \brief The QRPCInterfaceMaintence class
//!
class QRPCInterfaceMaintence : public QRpc::QRPCController
{
    Q_OBJECT
    QRPC_DECLARE_ROUTE(QRPCInterfaceCheck,"v1/maintence")
public:
    Q_INVOKABLE explicit QRPCInterfaceMaintence(QObject *parent = nullptr);
    ~QRPCInterfaceMaintence();

    //!
    //! \brief serverStart
    //! \return
    //!
    Q_INVOKABLE virtual QVariant serverStart();

    //!
    //! \brief serverStop
    //! \return
    //!
    Q_INVOKABLE virtual QVariant serverStop();


    //!
    //! \brief serverRestart
    //! \return
    //!
    Q_INVOKABLE virtual QVariant serverRestart();

    //!
    //! \brief servicesCheck
    //! \return
    //!
    Q_INVOKABLE virtual QVariant servicesCheck();

    //!
    //! \brief applicationQuit
    //! \return
    //!
    Q_INVOKABLE virtual QVariant applicationQuit();
signals:

};


QRPC_CONTROLLER_AUTO_REGISTER(QRPCInterfaceMaintence)

} // namespace QRpc
