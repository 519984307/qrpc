#pragma once

#include <QMetaMethod>
#include <QList>
#include <QMetaObject>
#include <QMetaMethod>
#include "./qrpc_global.h"
#include "./qrpc_listen_request.h"
#include "./qrpc_controller_setting.h"
namespace QRpc {

typedef QMultiHash<QByteArray, QMetaMethod> QRPCControllerMethods;

class QRPCListenRequest;
class QRPCListenRequestParser;
class QRPCServer;

//!
//! \brief The QRPCController class
//!
class Q_RPC_EXPORT QRPCController: public QObject
{
    Q_OBJECT
    friend class QRPCServer;
    friend class QRPCRequest;
    friend class QRPCListenQRPC;
    friend class QRPCListenQRPCSlotPvt;
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled )

public:
    //!
    //! \brief QRPCController
    //! \param parent
    //!
    Q_INVOKABLE explicit QRPCController(QObject *parent=nullptr);

    //!
    //! \brief ~QRPCController
    //!
    Q_INVOKABLE virtual ~QRPCController();

    //!
    //! \brief route
    //! \return
    //!
    Q_INVOKABLE virtual QVariant route()const;

    //!
    //! \brief makeRoute
    //!
    Q_INVOKABLE virtual void makeRoute();

    //!
    //! \brief module
    //! \return
    //!
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
    static QRPCControllerMethods routeMethods(const QByteArray &className);

    //!
    //! \brief request
    //! \return
    //!
    virtual QRPCListenRequest&request();

    //!
    //! \brief rq
    //! \return
    //!
    virtual QRPCListenRequest&rq();

    //!
    //! \brief canAuthorization
    //! \return
    //!
    virtual bool canAuthorization();

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
    virtual QRPCServer*server();

    //!
    //! \brief registerInterface
    //! \param metaObject
    //! \return
    //!
    static int registerInterface(const QMetaObject &metaObject);

    //!
    //! \brief registerParserRequest
    //! \param metaObject
    //! \return
    //!
    static int registerParserRequest(const QMetaObject &metaObject);

    //!
    //! \brief staticInterfaceList
    //! \return
    //!
    static QVector<const QMetaObject *> &staticInterfaceList();

    //!
    //! \brief parserRequestList
    //! \return
    //!
    static QVector<const QMetaObject *> &parserRequestList();

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
    virtual void setServer(QRPCServer*server);

    //!
    //! \brief setRequest
    //! \param request
    //!
    virtual void setRequest(QRPCListenRequest&request);

    //!
    //! \brief makeRoute
    //! \param object
    //! \param metaObject
    //!
    static void makeRoute(QObject *object, const QMetaObject *metaObject);

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
private:
    void*p = nullptr;
    QRPCProtocol acceptedProtocols;
};

}
