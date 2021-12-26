#pragma once

#include <QtWebSockets/QWebSocket>
#include <QtWebSockets/QWebSocketServer>
#include "../qrpc_listen.h"
#include "./p_qrpc_listen_http.h"

namespace QRpc {

//!
//! \brief The QRPCListenUDP class
//!
class Q_RPC_EXPORT QRPCListenUDP:public QRPCListen{
    Q_OBJECT
public:

    explicit QRPCListenUDP(QObject*parent=nullptr);
    Q_INVOKABLE ~QRPCListenUDP();

    //!
    //! \brief start
    //! \return
    //!
    bool start()override;

    //!
    //! \brief stop
    //! \return
    //!
    bool stop()override;
private:
    void*p=nullptr;
};

}
