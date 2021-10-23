#pragma once

#include "../qrpc_global.h"
#include <QThread>

namespace QRpc {
    class Q_RPC_EXPORT QRPCThread : public QThread{
        Q_OBJECT
    public:
        Q_INVOKABLE explicit QRPCThread(QObject *parent = nullptr);
        Q_INVOKABLE ~QRPCThread();

        /**
         * @brief run
         */
        void run();

    public slots:
        virtual void eventRun();
        virtual bool start();
        virtual bool stop();
        virtual bool quit();
    private:
        void*p=nullptr;
    };
}
