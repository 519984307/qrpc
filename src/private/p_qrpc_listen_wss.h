#pragma once

#include "../qrpc_listen.h"

namespace QRpc {

class Q_RPC_EXPORT QRPCListenWebSocket:public QRPCListen{
    Q_OBJECT
public:

    Q_INVOKABLE explicit QRPCListenWebSocket(QObject *parent=nullptr);
    Q_INVOKABLE ~QRPCListenWebSocket();

    /**
     * @brief start
     * @return
     */
    bool start();

    /**
     * @brief stop
     * @return
     */
    bool stop();
private:
    void*p=nullptr;
};

}
