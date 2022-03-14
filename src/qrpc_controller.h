#pragma once

#include <QMetaMethod>
#include <QList>
#include <QMetaObject>
#include <QMetaMethod>
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
class Q_RPC_EXPORT Controller: public QObject, public NotationsExtended
{
    Q_OBJECT
    friend class Server;
    friend class Request;
    friend class ListenQRPC;
    friend class ListenQRPCSlotPvt;

    Q_PROPERTY(QVariant basePath READ basePath CONSTANT)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

    QRPC_NOTATION_CLASS({})

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
    Q_INVOKABLE virtual QVariant basePath()const;

    //!
    //! \brief route
    //! \return
    //!
    QT_DEPRECATED_X("Use basePath;")
    Q_INVOKABLE virtual QVariant route()const;

    //!
    //! \brief apiInitialize
    //! \return
    //!
    virtual Controller &apiInitialize();

    //!
    //! \brief apiInitialize
    //! \param object
    //! \param metaObject
    //!
    static void apiInitialize(QObject *object, const QMetaObject *metaObject);

    //!
    //! \brief module
    //! \return
    //!
    QRPC_NOTATION_METHOD(module, {})
    Q_INVOKABLE virtual QString module()const;

    //!
    //! \brief moduleUuid
    //! \return
    //!
    Q_INVOKABLE virtual QUuid moduleUuid() const;

    //!
    //! \brief redirectCheck
    //! \return
    //!
    Q_INVOKABLE virtual bool redirectCheck()const;

    //!
    //! \brief description
    //! \return
    //!
    Q_INVOKABLE virtual QString description()const;

    //!
    //! \brief controllerSetting
    //! \return
    //!
    virtual ControllerSetting&controllerSetting();

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


    //!
    //! \brief routeRedirectCheckClass
    //! \param className
    //! \return
    //!
    static bool routeRedirectCheckClass(const QByteArray &className);
    static bool routeRedirectCheckRoute(const QByteArray &className, const QByteArray &routePath);

    //!
    //! \brief routeRedirectMethod
    //! \param className
    //! \param routePath
    //! \param method
    //! \return
    //!
    virtual bool routeRedirectMethod(const QByteArray &className, const QByteArray &routePath, QMetaMethod &method);

    //!
    //! \brief methodExists
    //! \param methodName
    //! \return
    //!
    virtual bool methodExists(const QByteArray &methodName) const;

    //!
    //! \brief routeExists
    //! \param routePath
    //! \return
    //!
    static bool routeExists(const QByteArray &routePath);

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
    //! \brief interfaceRegister
    //! \param metaObject
    //! \return
    //!
    static int interfaceRegister(const QMetaObject &metaObject);

    //!
    //! \brief parserRequestRegister
    //! \param metaObject
    //! \return
    //!
    static int parserRequestRegister(const QMetaObject &metaObject);

    //!
    //! \brief staticInterfaceList
    //! \return
    //!
    static QVector<const QMetaObject *> &staticInterfaceList();

    //!
    //! \brief staticParserRequestList
    //! \return
    //!
    static QVector<const QMetaObject *> &staticParserRequestList();

    //!
    //! \brief methodBlackList
    //! \return
    //!ignored method list
    static const QVector<QByteArray> &methodBlackList();


protected:
    //!
    //! \brief setServer
    //! \param server
    //!
    virtual void setServer(Server*server);

    //!
    //! \brief setRequest
    //! \param request
    //!
    virtual void setRequest(ListenRequest &request);

    //!
    //! \brief routeFlags
    //! \param route
    //! \return
    //!
    virtual QVariantHash routeFlags(const QString&route)const;

    //!
    //! \brief routeFlagsMaker
    //! \param request_path
    //! \param flag
    //! \return
    //!
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
