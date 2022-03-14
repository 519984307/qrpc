#pragma once

#include "./qrpc_global.h"
#include "./qrpc_listen_request.h"
#include "./qrpc_controller.h"
#include <QMetaObject>
#include <QMetaMethod>

namespace QRpc {

//!
//! \brief The ListenRequestParser class
//!
class Q_RPC_EXPORT ListenRequestParser : public QObject
{
    Q_OBJECT
    QRPC_PARSER_DECLARE_BASE_PATH(ListenRequestParser,"/")
    friend class ListenQRPCSlotPvt;
public:

    //!
    //! \brief ListenRequestParser
    //! \param parent
    //!
    Q_INVOKABLE explicit ListenRequestParser(QObject *parent = nullptr);

    //!
    //! \brief ~ListenRequestParser
    //!
    ~ListenRequestParser();

    //!
    //! \brief controller
    //! \return
    //!
    virtual Controller &controller();
protected:

    //!
    //! \brief setController
    //! \param value
    //!
    void setController(Controller *value);
public:
    //!
    //! \brief request
    //! \return
    //!
    virtual ListenRequest &request();

    //!
    //! \brief rq
    //! \return
    //!
    virtual ListenRequest &rq();

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
    //! \brief initializeInstalleds
    //! \param metaObject
    //!
    static void initializeInstalleds(const QMetaObject &metaObject);

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
