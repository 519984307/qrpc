#pragma once

#include "../qrpc_listen.h"

namespace QRpc {

//!
//! \brief The ListenHTTP class
//!
class Q_RPC_EXPORT ListenHTTP:public Listen{
    Q_OBJECT
public:

    Q_INVOKABLE explicit ListenHTTP(QObject*parent=nullptr);
    ~ListenHTTP();

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

QRPC_LISTTEN_AUTO_REGISTER(Http, ListenHTTP)
//QRPC_LISTTEN_AUTO_REGISTER(Https, ListenHTTP)


}
