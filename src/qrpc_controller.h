#pragma once

#include <QMetaMethod>
#include <QList>
#include <QMetaObject>
#include <QMetaMethod>
#include "../../qnotation/src/qnotation.h"
#include "./qrpc_global.h"
#include "./qrpc_listen_request.h"
#include "./qrpc_controller_setting.h"
#include "./qrpc_controller_notation.h"

namespace QRpc {

typedef QMultiHash<QByteArray, QMetaMethod> ControllerMethods;

class ListenRequest;
class ListenRequestParser;
class Server;

//!
//! \brief The Controller class
//!
class Q_RPC_EXPORT Controller: public QObject, public QRpcPrivate::NotationsExtended
{
    Q_OBJECT
    friend class Server;
    friend class Request;
    friend class ListenQRPC;
    friend class ListenQRPCSlotPvt;

    Q_PROPERTY(QVariant basePath READ basePath CONSTANT)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

    Q_NOTATION(Controller, {})

public:
    //!
    //! \brief Controller
    //! \param parent
    //!
    Q_INVOKABLE explicit Controller(QObject *parent=nullptr);

    //!
    //! \brief ~Controller
    //!
    ~Controller();

    //!
    //! \brief basePath
    //! \return
    //!
    virtual QVariant basePath()const;

    //!
    //! \brief route
    //! \return
    //!
    QT_DEPRECATED_X("Use basePath()")
    virtual QVariant route()const;

    //!
    //! \brief initializeInstalleds
    //! \return
    //!
    virtual Controller &initializeInstalleds();

    //!
    //! \brief initializeInstalleds
    //! \param object
    //! \param metaObject
    //!
    static void initializeInstalleds(QObject *object, const QMetaObject *metaObject);

    //!
    //! \brief module
    //! \return
    //!
    virtual QString module()const;

    //!
    //! \brief moduleUuid
    //! \return
    //!
    virtual QUuid moduleUuid() const;

    //!
    //! \brief redirectCheck
    //! \return
    //!

    virtual bool redirectCheck()const;

    //!
    //! \brief redirectCheckClass
    //! \param className
    //! \return
    //!
    static bool redirectCheckClass(const QByteArray &className);

    //!
    //! \brief redirectCheckBasePath
    //! \param className
    //! \param basePath
    //! \return
    //!
    static bool redirectCheckBasePath(const QByteArray &className, const QByteArray &basePath);

    //!
    //! \brief redirectMethod
    //! \param className
    //! \param routePath
    //! \param method
    //! \return
    //!
    virtual bool redirectMethod(const QByteArray &className, const QByteArray &path, QMetaMethod &method);

    //!
    //! \brief description
    //! \return
    //!
    virtual QString description()const;

    //!
    //! \brief controllerSetting
    //! \return
    //!
    virtual ControllerSetting &setting();

    //!
    //! \brief enabled
    //! \return
    //!
    virtual bool enabled()const;

    //!
    //! \brief setEnabled
    //! \param enabled
    //!
    virtual void setEnabled(bool enabled);

//    //!
//    //! \brief methodExists
//    //! \param methodName
//    //! \return
//    //!
//    virtual bool methodExists(const QByteArray &methodName) const;

//    //!
//    //! \brief routeExists
//    //! \param routePath
//    //! \return
//    //!
//    static bool routeExists(const QByteArray &routePath);

    //!
    //! \brief routeMethods
    //! \param className
    //! \return
    //!
    static ControllerMethods routeMethods(const QByteArray &className);

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
    //! \brief canOperation
    //! \param method
    //! \return
    //!
    virtual bool canOperation(const QMetaMethod &method);

    //!
    //! \brief canAuthorization
    //! \return
    //!
    virtual bool canAuthorization();

    //!
    //! \brief canAuthorization
    //! \param method
    //! \return
    //!
    virtual bool canAuthorization(const QMetaMethod&method);

    //!
    //! \brief beforeAuthorization
    //! \return
    //!
    virtual bool beforeAuthorization();

    //!
    //! \brief authorization
    //! \return
    //!
    virtual bool authorization();

    //!
    //! \brief authorization
    //! \param method
    //! \return
    //!
    virtual bool authorization(const QMetaMethod &method);

    //!
    //! \brief afterAuthorization
    //! \return
    //!
    virtual bool afterAuthorization();

    //!
    //! \brief requestBeforeInvoke
    //! \return
    //!
    virtual bool requestBeforeInvoke();

    //!
    //! \brief requestRedirect
    //! \return
    //!
    virtual bool requestRedirect();

    //!
    //! \brief requestAfterInvoke
    //! \return
    //!
    virtual bool requestAfterInvoke();

    //!
    //! \brief server
    //! \return
    //!
    virtual Server*server();

    //!
    //! \brief install
    //! \param metaObject
    //! \return
    //!
    static int install(const QMetaObject &metaObject);

    //!
    //! \brief installParser
    //! \param metaObject
    //! \return
    //!
    static int installParser(const QMetaObject &metaObject);

    //!
    //! \brief staticApiList
    //! \return
    //!
    static QVector<const QMetaObject *> &staticApiList();

    //!
    //! \brief staticApiParserList
    //! \return
    //!
    static QVector<const QMetaObject *> &staticApiParserList();

protected:
    //!
    //! \brief setServer
    //! \param server
    //!
    virtual QRpc::Controller &setServer(Server*server);

    //!
    //! \brief setRequest
    //! \param request
    //!
    virtual Controller &setRequest(ListenRequest &request);

    //!
    //! \brief routeFlags
    //! \param route
    //! \return
    //!
    QT_DEPRECATED_X("use QRPC_NOTATION or Q_NOTATION")
    virtual QVariantHash routeFlags(const QString&route)const;

    //!
    //! \brief routeFlagsMaker
    //! \param request_path
    //! \param flag
    //! \return
    //!
    QT_DEPRECATED_X("use QRPC_NOTATION or Q_NOTATION")
    static const QVariantHash routeFlagsMaker(const QString&request_path, const QVariant &flag);

signals:
    //!
    //! \brief enabledChanged
    //!
    void enabledChanged();

    //!
    //! \brief notMethodCanAuthorizationChanged
    //!
    void notMethodCanAuthorizationChanged();
private:
    void*p = nullptr;
    QRPCProtocol acceptedProtocols;
};

typedef Controller QRPCController;

}
