#pragma once

#include <QWebSocket>
#include <QWebSocketServer>
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
    Q_INVOKABLE ~QRPCListenTCP();

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
