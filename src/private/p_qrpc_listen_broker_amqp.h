#pragma once

#include "../qrpc_listen.h"

namespace QRpc {

//!
//! \brief The QRPCListenBrokerAMQP class
//!
class Q_RPC_EXPORT QRPCListenBrokerAMQP:public QRPCListen
{
    Q_OBJECT
public:

    //!
    //! \brief QRPCListenBrokerAMQP
    //! \param parent
    //!
    Q_INVOKABLE explicit QRPCListenBrokerAMQP(QObject*parent=nullptr);

    //!
    //! \brief ~QRPCListenBrokerAMQP
    //!
    Q_INVOKABLE ~QRPCListenBrokerAMQP();
private:
    void*p=nullptr;
};

}
