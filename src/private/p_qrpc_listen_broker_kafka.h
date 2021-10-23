#pragma once

#include "../qrpc_listen.h"

namespace QRpc {

class Q_RPC_EXPORT QRPCListenBrokerKAFKA:public QRPCListen
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit QRPCListenBrokerKAFKA(QObject*parent=nullptr);
    Q_INVOKABLE ~QRPCListenBrokerKAFKA();
private:
    void*p=nullptr;
};

}
