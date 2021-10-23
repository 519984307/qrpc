#pragma once

#include "../qrpc_listen.h"
#include <QSqlDriver>

namespace QRpc {

class Q_RPC_EXPORT QRPCListenBrokerDataBase:public QRPCListen{
    Q_OBJECT
public:

    Q_INVOKABLE explicit QRPCListenBrokerDataBase(QObject*parent=nullptr);

    /**
     * @brief start
     * @return
     */
    bool start();

    /**
     * @brief stop
     * @return
     */
    bool stop();

private:
    void*p=nullptr;
signals:
    void rpcResponseClient(QSqlDriver*sqlDriver, const QString&requestPath, const QVariantHash&responseBody);
};


}
