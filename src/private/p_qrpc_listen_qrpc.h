#pragma once

#include "../qrpc_listen.h"

namespace QRpc {

//!
//! \brief The QRPCListenQRPC class
//!
class Q_RPC_EXPORT QRPCListenQRPC:public QRPCListen
{
    Q_OBJECT
    friend class QRPCListenColletionsPvt;
    friend class QRPCListenQRPCSlotPvt;
public:

    //!
    //! \brief QRPCListenQRPC
    //! \param parent
    //!
    Q_INVOKABLE explicit QRPCListenQRPC(QObject*parent=nullptr);

    //!
    //! \brief ~QRPCListenQRPC
    //!
    Q_INVOKABLE virtual ~QRPCListenQRPC();

    //!
    //! \brief run
    //!
    void run()override;

protected:
    //!
    //! \brief registerListen
    //! \param listen
    //!
    void registerListen(QRPCListen *listen);

    //!
    //! \brief childrenListen
    //! \param uuid
    //! \return
    //!
    virtual QRPCListen*childrenListen(QUuid uuid);

private:
    void*p=nullptr;
};

}
