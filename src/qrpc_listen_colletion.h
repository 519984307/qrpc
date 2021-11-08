#pragma once

#include "./qrpc_types.h"
#include "./qrpc_listen.h"
#include <QThread>
#include <QVariant>
#include <QVariantHash>

class QSettings;

namespace QRpc {
class QRPCListenProtocol;
class QRPCListenQRPC;
class QRPCServer;
class QRPCListen;

//!
//! \brief QRPCListenProtocols
//!
typedef QHash<int, QRPCListenProtocol*> QRPCListenProtocols;

//!
//! \brief The QRPCListenColletions class
//!
class Q_RPC_EXPORT QRPCListenColletions : public QThread{
    Q_OBJECT
public:
    //!
    //! \brief QRPCListenColletions
    //! \param parent
    //!
    explicit QRPCListenColletions(QRPCServer *parent = nullptr);

    //!
    //! \brief QRPCListenColletions
    //! \param settings
    //! \param server
    //!
    explicit QRPCListenColletions(const QVariantHash &settings, QRPCServer *server = nullptr);

    //!
    //! \brief ~QRPCListenColletions
    //!
    Q_INVOKABLE ~QRPCListenColletions();

    //!
    //! \brief protocol
    //! \return
    //!
    virtual QRPCListenProtocol &protocol();

    //!
    //! \brief protocol
    //! \param protocol
    //! \return
    //!
    virtual QRPCListenProtocol &protocol(const QRpc::QRPCProtocol&protocol);

    //!
    //! \brief protocols
    //! \return
    //!
    virtual QRPCListenProtocols&protocols();

    //!
    //! \brief run
    //!
    void run();

    //!
    //! \brief requestEnabled
    //!
    virtual void requestEnabled();

    //!
    //! \brief requestDisable
    //!
    virtual void requestDisable();

    //!
    //! \brief server
    //! \return
    //!
    virtual QRPCServer *server();

    //!
    //! \brief setSettings
    //! \param settings
    //!
    virtual void setSettings(const QVariantHash&settings) const;

    //!
    //! \brief listenPool
    //! \return
    //!
    virtual QRPCListenQRPC*listenPool();

public slots:

    //!
    //! \brief start
    //! \return
    //!
    virtual bool start();

    //!
    //! \brief stop
    //! \return
    //!
    virtual bool stop();

    //!
    //! \brief quit
    //! \return
    //!
    virtual bool quit();
private:
    QObject*p=nullptr;
signals:
};
}
