#pragma once

#include "../qrpc_listen.h"

namespace QRpc {

//!
//! \brief The QRPCListenLocalSocket class
//!
class Q_RPC_EXPORT QRPCListenLocalSocket:public QRPCListen{
    Q_OBJECT
public:

    //!
    //! \brief QRPCListenLocalSocket
    //! \param parent
    //!
    explicit QRPCListenLocalSocket(QObject*parent=nullptr);

    //!
    //! \brief ~QRPCListenLocalSocket
    //!
    ~QRPCListenLocalSocket();

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
