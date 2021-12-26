#pragma once

#include <QtWebSockets/QWebSocket>
#include <QtWebSockets/QWebSocketServer>
#include "../qrpc_listen.h"
#include "./p_qrpc_listen_http.h"

namespace QRpc {

//!
//! \brief The QRPCListenTCP class
//!
class Q_RPC_EXPORT QRPCListenTCP:public QRPCListen{
    Q_OBJECT
public:

    explicit QRPCListenTCP(QObject*parent=nullptr);
    ~QRPCListenTCP();

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
