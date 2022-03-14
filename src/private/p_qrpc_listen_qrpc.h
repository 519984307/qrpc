#pragma once

#include "../qrpc_listen.h"

namespace QRpc {

//!
//! \brief The ListenQRPC class
//!
class Q_RPC_EXPORT ListenQRPC:public Listen
{
    Q_OBJECT
    friend class ListenColletionsPvt;
    friend class ListenQRPCSlotPvt;
public:

    //!
    //! \brief ListenQRPC
    //! \param parent
    //!
    Q_INVOKABLE explicit ListenQRPC(QObject*parent=nullptr);

    //!
    //! \brief ~ListenQRPC
    //!
    ~ListenQRPC();

    //!
    //! \brief run
    //!
    void run()override;

protected:
    //!
    //! \brief registerListen
    //! \param listen
    //!
    void registerListen(Listen *listen);

    //!
    //! \brief childrenListen
    //! \param uuid
    //! \return
    //!
    virtual Listen*childrenListen(QUuid uuid);

private:
    void*p=nullptr;
};

QRPC_LISTTEN_AUTO_REGISTER(0, ListenQRPC)

}
