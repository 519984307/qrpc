#pragma once

#include "./qrpc_global.h"
#include "./qrpc_listen_request.h"
#include "./qrpc_controller.h"
#include <QMetaObject>
#include <QMetaMethod>

namespace QRpc {

//!
//! \brief The QRPCListenRequestParser class
//!
class Q_RPC_EXPORT QRPCListenRequestParser : public QObject
{
    Q_OBJECT
    QRPC_PARSER_DECLARE_ROUTE(QRPCListenRequestParser,"/")
    friend class QRPCListenQRPCSlotPvt;
public:
    Q_INVOKABLE explicit QRPCListenRequestParser(QObject *parent = nullptr);
    Q_INVOKABLE ~QRPCListenRequestParser();

    //!
    //! \brief controller
    //! \return
    //!
    virtual QRPCController &controller();
protected:

    //!
    //! \brief setController
    //! \param value
    //!
    void setController(QRPCController *value);
public:
    //!
    //! \brief request
    //! \return
    //!
    virtual QRPCListenRequest &request();

    //!
    //! \brief rq
    //! \return
    //!
    virtual QRPCListenRequest &rq();

    //!
    //! \brief canRoute
    //! \param metaObject
    //! \param route
    //! \return
    //!test route is valid
    static bool canRoute(const QMetaObject &metaObject, const QString&route);

    //!
    //! \brief routeToMethod
    //! \param metaObject
    //! \param route
    //! \param outMethod
    //! \return
    //!return method to invoke
    static bool routeToMethod(const QMetaObject &metaObject, const QString&route, QMetaMethod&outMethod);

    //!
    //! \brief makeRoute
    //! \param metaObject
    //!
    static void makeRoute(const QMetaObject &metaObject);

protected:
    //!
    //! \brief parse
    //! \param metaMethod
    //! \return
    //!default method to parser request
    virtual bool parse(const QMetaMethod&metaMethod);

private:
    void*p=nullptr;
};

} // namespace QRpc
