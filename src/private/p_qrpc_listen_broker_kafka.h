#pragma once

#include "../qrpc_listen.h"

namespace QRpc {

//!
//! \brief The QRPCListenBrokerKAFKA class
//!
class Q_RPC_EXPORT QRPCListenBrokerKAFKA:public QRPCListen
{
    Q_OBJECT
public:

    //!
    //! \brief QRPCListenBrokerKAFKA
    //! \param parent
    //!
    Q_INVOKABLE explicit QRPCListenBrokerKAFKA(QObject*parent=nullptr);

    //!
    //! \brief ~QRPCListenBrokerKAFKA
    //!
    Q_INVOKABLE ~QRPCListenBrokerKAFKA();
private:
    void*p=nullptr;
};

}
