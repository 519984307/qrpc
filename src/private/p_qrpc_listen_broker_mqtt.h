#pragma once

#include "../qrpc_listen.h"

namespace QRpc {

//!
//! \brief The QRPCListenBrokerMQTT class
//!
class Q_RPC_EXPORT QRPCListenBrokerMQTT:public QRPCListen
{
    Q_OBJECT
public:
    //!
    //! \brief QRPCListenBrokerMQTT
    //! \param parent
    //!
    Q_INVOKABLE explicit QRPCListenBrokerMQTT(QObject*parent=nullptr);

    //!
    //! \brief ~QRPCListenBrokerMQTT
    //!
    Q_INVOKABLE ~QRPCListenBrokerMQTT();
private:
    void*p=nullptr;
};

}
