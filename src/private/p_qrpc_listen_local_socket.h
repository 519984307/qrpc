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
