#pragma once

#include "./qrpc_global.h"
#include "./qstm_object.h"
#include <QObject>
#include <QMetaObject>
#include <QMetaMethod>

#define QRPC_CONTROLLER_ROUTER_REGISTER(className)\
const auto __router_register##className=QRpc::QRPCControllerRouter::installDefaultRouter(&className::staticMetaObject);

namespace QRpc {
class QRPCListenRequest;
//!
//! \brief The QRPCControllerRouter class
//!
class Q_RPC_EXPORT QRPCControllerRouter: public QStm::Object
{
    Q_OBJECT
public:
    //!
    //! \brief QRPCControllerRouter
    //! \param parent
    //!
    Q_INVOKABLE explicit QRPCControllerRouter(QObject *parent=nullptr);

    //!
    //! \brief ~QRPCControllerRouter
    //!
    Q_INVOKABLE virtual ~QRPCControllerRouter();

    //!
    //! \brief router
    //! \param request
    //! \param metaMethod
    //! \return
    //!
    virtual ResultValue&router(QRPCListenRequest *request, QMetaMethod&metaMethod);

    //!
    //! \brief installDefaultRouter
    //! \param metaObject
    //! \return
    //!
    Q_INVOKABLE static bool installDefaultRouter(const QMetaObject*metaObject);

    //!
    //! \brief newRouter
    //! \param parent
    //! \return
    //!
    static QRPCControllerRouter*newRouter(QObject *parent);

private:
    void*p=nullptr;
};
}
