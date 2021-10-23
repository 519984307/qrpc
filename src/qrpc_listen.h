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
    class Q_RPC_EXPORT QRPCListen: public QThread
    {
        Q_OBJECT
        friend class QRPCListenColletions;
        friend class QRPCListenColletionsPvt;
    public:
        Q_INVOKABLE explicit QRPCListen(QObject *parent=nullptr);
        Q_INVOKABLE virtual ~QRPCListen();

        /**
         * @brief uuid
         * @return
         */
        virtual QUuid uuid() const;

        /**
         * @brief parent
         * @return
         */
        QObject *parent();
        void setParent(QObject *parent);

        /**
         * @brief run
         */
        void run() override;

        /**
         * @brief start
         * @return
         */
        virtual bool start();

        /**
         * @brief stop
         * @return
         */
        virtual bool stop();

        /**
         * @brief server
         * @return
         */
        virtual QRPCServer*server();

        /**
         * @brief colletions
         * @return
         */
        virtual QRPCListenColletions*colletions();

        /**
         * @brief cacheRequest
         * @return
         */
        virtual QRPCListenRequestCache*cacheRequest();

        /**
         * @brief registerListenPool
         * @param pool
         */
        virtual void registerListenPool(QRPCListen*listenPool);

        /**
         * @brief listenPool
         * @return
         */
        virtual QRPCListen&listenPool();
    signals:
        /**
         * @brief rpcRequest
         * @param body
         */
        void rpcRequest(QVariantHash request, const QVariant&uploadedFiles);

        /**
         * @brief rpcResponse
         * @param body
         */
        void rpcResponse(QUuid uuid, const QVariantHash&request);

        /**
         * @brief rpcCheck
         * @param listen
         */
        void rpcCheck();
    protected:
        /**
         * @brief setServer
         * @param server
         */
        void setServer(QRPCServer*server);

        /**
         * @brief setColletions
         * @param colletions
         */
        void setColletions(QRPCListenColletions*colletions);
    private:
        void*p=nullptr;
    };

}
