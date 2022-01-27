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
    ~QRPCListenHTTP();

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

QRPC_LISTTEN_AUTO_REGISTER(Http, QRPCListenHTTP)
//QRPC_LISTTEN_AUTO_REGISTER(Https, QRPCListenHTTP)


}
