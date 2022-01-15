#pragma once

#include "./qrpc_controller.h"

namespace QRpc {

//!
//! \brief The QRPCInterfaceRecord class
//!
class QRPCInterfaceRecord : public QRpc::QRPCController
{
    Q_OBJECT
    Q_RPC_DECLARE_INTERFACE_METHOD_CHECK()
public:
    //!
    //! \brief QRPCInterfaceRecord
    //! \param parent
    //!
    Q_INVOKABLE explicit QRPCInterfaceRecord(QObject *parent = nullptr);
    ~QRPCInterfaceRecord();

    //!
    //! \brief get
    //! \return
    //!
    Q_INVOKABLE virtual QVariant get();

    //!
    //! \brief set
    //! \return
    //!
    Q_INVOKABLE virtual QVariant set();
    //!
    //! \brief del
    //! \return
    //!
    Q_INVOKABLE virtual QVariant del();

    //!
    //! \brief list
    //! \return
    //!
    Q_INVOKABLE virtual QVariant list();
signals:
};

}
