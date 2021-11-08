#pragma once

#include "./qrpc_global.h"
#include <QThread>
#include <QUuid>
#include <QVariantHash>
#include <QTemporaryFile>

namespace QRpc {
class QRPCServer;
class QRPCListenQRPC;
class QRPCListenColletions;
class QRPCListenRequestCache;

//!
//! \brief The QRPCListen class
//!
class Q_RPC_EXPORT QRPCListen: public QThread
{
    Q_OBJECT
    friend class QRPCListenColletions;
    friend class QRPCListenColletionsPvt;
public:

    //!
    //! \brief QRPCListen
    //! \param parent
    //!
    Q_INVOKABLE explicit QRPCListen(QObject *parent=nullptr);

    //!
    //! \brief ~QRPCListen
    //!
    Q_INVOKABLE virtual ~QRPCListen();

    //!
    //! \brief uuid
    //! \return
    //!
    virtual QUuid uuid() const;

    //!
    //! \brief parent
    //! \return
    //!
    QObject *parent();
    //!
    //! \brief setParent
    //! \param parent
    //!
    void setParent(QObject *parent);

    //!
    //! \brief run
    //!
    void run() override;

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
    //! \brief server
    //! \return
    //!
    virtual QRPCServer*server();

    //!
    //! \brief colletions
    //! \return
    //!
    virtual QRPCListenColletions*colletions();

    //!
    //! \brief cacheRequest
    //! \return
    //!
    virtual QRPCListenRequestCache*cacheRequest();

    //!
    //! \brief registerListenPool
    //! \param listenPool
    //!
    virtual void registerListenPool(QRPCListen*listenPool);

    //!
    //! \brief listenPool
    //! \return
    //!
    virtual QRPCListen&listenPool();
signals:

    //!
    //! \brief rpcRequest
    //! \param request
    //! \param uploadedFiles
    //!
    void rpcRequest(QVariantHash request, const QVariant&uploadedFiles);

    //!
    //! \brief rpcResponse
    //! \param uuid
    //! \param request
    //!
    void rpcResponse(QUuid uuid, const QVariantHash&request);

    //!
    //! \brief rpcCheck
    //!
    void rpcCheck();
protected:
    //!
    //! \brief setServer
    //! \param server
    //!
    void setServer(QRPCServer*server);

    //!
    //! \brief setColletions
    //! \param colletions
    //!
    void setColletions(QRPCListenColletions*colletions);
private:
    void*p=nullptr;
};

}
