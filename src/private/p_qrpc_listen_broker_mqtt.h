#pragma once

#include "../qrpc_listen.h"

namespace QRpc {

class Q_RPC_EXPORT QRPCListenBrokerMQTT:public QRPCListen
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit QRPCListenBrokerMQTT(QObject*parent=nullptr);
    Q_INVOKABLE ~QRPCListenBrokerMQTT();
private:
    void*p=nullptr;
};

}
