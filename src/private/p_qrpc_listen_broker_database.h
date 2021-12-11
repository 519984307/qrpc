#pragma once

#include "../qrpc_listen.h"
#include <QSqlDriver>

namespace QRpc {

//!
//! \brief The QRPCListenBrokerDataBase class
//!
class Q_RPC_EXPORT QRPCListenBrokerDataBase:public QRPCListen{
    Q_OBJECT
public:

    //!
    //! \brief QRPCListenBrokerDataBase
    //! \param parent
    //!
    Q_INVOKABLE explicit QRPCListenBrokerDataBase(QObject*parent=nullptr);

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
signals:
    //!
    //! \brief rpcResponseClient
    //! \param sqlDriver
    //! \param requestPath
    //! \param responseBody
    //!
    void rpcResponseClient(QSqlDriver*sqlDriver, const QString&requestPath, const QVariantHash&responseBody);
};


}
