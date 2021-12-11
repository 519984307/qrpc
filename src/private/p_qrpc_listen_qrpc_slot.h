#pragma once

#include "./p_qrpc_listen_qrpc.h"

namespace QRpc {

#include <QVariant>
#include <QByteArray>
#include <QVariantHash>
#include <QEventLoop>

//!
//! \brief The QRPCListenQRPCSlot class
//!
class Q_RPC_EXPORT QRPCListenQRPCSlot:public QThread
{
    Q_OBJECT
public:
    explicit QRPCListenQRPCSlot(QRPCListenQRPC *listenQRPC=nullptr);
    Q_INVOKABLE ~QRPCListenQRPCSlot();

    //!
    //! \brief run
    //!
    void run()override;

    //!
    //! \brief canRequestInvoke
    //! \param v
    //! \param uploadedFiles
    //! \return
    //!
    bool canRequestInvoke(QVariantHash &v, const QVariant&uploadedFiles);

    //!
    //! \brief start
    //!
    void start();

    //!
    //! \brief lock
    //! \return
    //!
    virtual bool lock();

    //!
    //! \brief unlock
    //!
    virtual void unlock();

signals:

    //!
    //! \brief requestInvoke
    //! \param v
    //! \param uploadedFiles
    //!
    void requestInvoke(QVariantHash v, const QVariant&uploadedFiles);
private:
    QObject*p=nullptr;
};

}
