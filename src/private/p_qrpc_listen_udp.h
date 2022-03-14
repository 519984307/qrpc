#pragma once

#include <QtWebSockets/QWebSocket>
#include <QtWebSockets/QWebSocketServer>
#include "../qrpc_listen.h"
#include "./p_qrpc_listen_http.h"

namespace QRpc {

//!
//! \brief The ListenUDP class
//!
class Q_RPC_EXPORT ListenUDP:public Listen{
    Q_OBJECT
public:

    explicit ListenUDP(QObject*parent=nullptr);
    ~ListenUDP();

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

QRPC_LISTTEN_AUTO_REGISTER(UdpSocket, ListenUDP)

}
