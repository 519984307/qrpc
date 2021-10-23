#pragma once

#include "../qrpc_listen.h"

namespace QRpc {

class Q_RPC_EXPORT QRPCListenQRPC:public QRPCListen
{
    Q_OBJECT
    friend class QRPCListenColletionsPvt;
    friend class QRPCListenQRPCSlotPvt;
public:
    Q_INVOKABLE explicit QRPCListenQRPC(QObject*parent=nullptr);
    Q_INVOKABLE virtual ~QRPCListenQRPC();

    /**
     * @brief run
     */
    void run();

protected:
    /**
     * @brief registerListen
     * @param listen
     */
    void registerListen(QRPCListen *listen);

    /**
     * @brief childrenListen
     * @param uuid
     * @return
     */
    virtual QRPCListen*childrenListen(QUuid uuid);


private:
    void*p=nullptr;
};

}
