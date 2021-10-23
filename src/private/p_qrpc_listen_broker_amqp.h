#pragma once

#include "../qrpc_listen.h"

namespace QRpc {

class Q_RPC_EXPORT QRPCListenBrokerAMQP:public QRPCListen
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit QRPCListenBrokerAMQP(QObject*parent=nullptr);
    Q_INVOKABLE ~QRPCListenBrokerAMQP();
private:
    void*p=nullptr;
};

}
