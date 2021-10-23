#pragma once

#include "./p_qrpc_listen_qrpc.h"

namespace QRpc {

#include <QVariant>
#include <QByteArray>
#include <QVariantHash>
#include <QEventLoop>

class Q_RPC_EXPORT QRPCListenQRPCSlot:public QThread
{
    Q_OBJECT
public:
    explicit QRPCListenQRPCSlot(QRPCListenQRPC *listenQRPC=nullptr);
    Q_INVOKABLE ~QRPCListenQRPCSlot();

    /**
     * @brief run
     */
    void run();

    /**
     * @brief canRequestInvoke
     * @param v
     * @return
     */
    bool canRequestInvoke(QVariantHash &v, const QVariant&uploadedFiles);

    /**
     * @brief start
     */
    void start();

    /**
     * @brief lock
     */
    virtual bool lock();

    /**
     * @brief unlock
     */
    virtual void unlock();

signals:
    void requestInvoke(QVariantHash v, const QVariant&uploadedFiles);
private:
    QObject*p=nullptr;
};

}
