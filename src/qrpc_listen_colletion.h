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

    typedef QHash<int, QRPCListenProtocol*> QRPCListenProtocols;
    class Q_RPC_EXPORT QRPCListenColletions : public QThread{
        Q_OBJECT
    public:
        explicit QRPCListenColletions(QRPCServer *parent = nullptr);
        explicit QRPCListenColletions(const QVariantHash &settings, QRPCServer *server = nullptr);
        Q_INVOKABLE ~QRPCListenColletions();

        /**
         * @brief protocol
         * @return
         */
        virtual QRPCListenProtocol &protocol();
        virtual QRPCListenProtocol &protocol(const QRpc::QRPCProtocol&protocol);

        /**
         * @brief protocols
         * @return
         */
        virtual QRPCListenProtocols&protocols();

        /**
         * @brief run
         */
        void run();

        /**
         * @brief requestEnabled
         * @return
         */
        virtual void requestEnabled();

        /**
         * @brief requestDisable
         * @return
         */
        virtual void requestDisable();

        /**
         * @brief server
         * @return
         */
        virtual QRPCServer *server();

        /**
         * @brief setSettings
         * @param settings
         */
        virtual void setSettings(const QVariantHash&settings) const;

        /**
         * @brief listenPool
         * @return
         */
        virtual QRPCListenQRPC*listenPool();

    public slots:
        virtual bool start();
        virtual bool stop();
        virtual bool quit();
    private:
        QObject*p=nullptr;
    signals:
    };
}
