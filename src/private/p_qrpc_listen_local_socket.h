#pragma once

#include "../qrpc_listen.h"

namespace QRpc {

class Q_RPC_EXPORT QRPCListenLocalSocket:public QRPCListen{
    Q_OBJECT
public:

    explicit QRPCListenLocalSocket(QObject*parent=nullptr);
    Q_INVOKABLE ~QRPCListenLocalSocket();

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
