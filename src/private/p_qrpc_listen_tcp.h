#pragma once

#include <QWebSocket>
#include <QWebSocketServer>
#include "../qrpc_listen.h"
#include "./p_qrpc_listen_http.h"

namespace QRpc {

class Q_RPC_EXPORT QRPCListenTCP:public QRPCListen{
    Q_OBJECT
public:

    explicit QRPCListenTCP(QObject*parent=nullptr);
    Q_INVOKABLE ~QRPCListenTCP();

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
