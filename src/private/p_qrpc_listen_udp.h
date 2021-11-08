#pragma once

#include <QWebSocket>
#include <QWebSocketServer>
#include "../qrpc_listen.h"
#include "./p_qrpc_listen_http.h"
#include "httprequesthandler.h"
#include "httplistener.h"

namespace QRpc {

class Q_RPC_EXPORT QRPCListenUDP:public QRPCListen{
    Q_OBJECT
public:

    explicit QRPCListenUDP(QObject*parent=nullptr);
    Q_INVOKABLE ~QRPCListenUDP();

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
