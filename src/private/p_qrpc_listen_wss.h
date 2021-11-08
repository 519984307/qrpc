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
    bool start()override;

    /**
     * @brief stop
     * @return
     */
    bool stop()override;
private:
    void*p=nullptr;
};

}
