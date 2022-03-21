#pragma once

#include "../qrpc_listen.h"
#include "./p_qrpc_listen_http.h"
#include <QtWebSockets/QWebSocket>
#include <QtWebSockets/QWebSocketServer>

namespace QRpc {

//!
//! \brief The ListenTCP class
//!
class Q_RPC_EXPORT ListenTCP : public Listen
{
    Q_OBJECT
public:
    //!
    //! \brief ListenTCP
    //! \param parent
    //!
    explicit ListenTCP(QObject *parent = nullptr);
    ~ListenTCP();

    //!
    //! \brief start
    //! \return
    //!
    bool start() override;

    //!
    //! \brief stop
    //! \return
    //!
    bool stop() override;

private:
    void *p = nullptr;
};

QRPC_LISTTEN_AUTO_REGISTER(TcpSocket, ListenTCP)

} // namespace QRpc
