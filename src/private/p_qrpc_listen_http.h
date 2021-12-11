#pragma once

#include "../qrpc_listen.h"

namespace QRpc {

//!
//! \brief The QRPCListenHTTP class
//!
class Q_RPC_EXPORT QRPCListenHTTP:public QRPCListen{
    Q_OBJECT
public:

    Q_INVOKABLE explicit QRPCListenHTTP(QObject*parent=nullptr);
    Q_INVOKABLE ~QRPCListenHTTP();

    //!
    //! \brief start
    //! \return
    //!
    bool start()override;

    //!
    //! \brief stop
    //! \return
    //!
    bool stop()override;
private:
    void*p=nullptr;
};


}
