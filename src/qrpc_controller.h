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
class Q_RPC_EXPORT QRPCController: public QObject
{
    Q_OBJECT
    QRPC_DECLARE_ROUTE(QRPCController, qsl("/"))
    QRPC_DECLARE_MODULE(qsl_null)
    QRPC_REDIRECT_NO_VERIFY()
    friend class QRPCServer;
    friend class QRPCRequest;
    friend class QRPCListenQRPC;
    friend class QRPCListenQRPCSlotPvt;

    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled )

public:
    Q_INVOKABLE explicit QRPCController(QObject *parent=nullptr);
    Q_INVOKABLE virtual ~QRPCController();

    /**
     * @brief controllerSetting
     * @return
     */
    virtual ControllerSetting&controllerSetting();
    /**
     * @brief enabled
     * @return
     */
    virtual bool enabled();
    virtual void setEnabled(bool enabled);


    /**
     * @brief routeMethods
     * @param metaObject
     * @return
     */
    static bool routeRedirectCheckClass(const QByteArray &className);
    static bool routeRedirectCheckRoute(const QByteArray &className, const QByteArray &routePath);

    /**
     * @brief routeRedirectMethod
     * @param className
     * @param routePath
     * @param method
     * @return
     */
    virtual bool routeRedirectMethod(const QByteArray &className, const QByteArray &routePath, QMetaMethod &method);

    /**
     * @brief routeGetMethod
     * @param routePath
     * @return
     */
    virtual bool methodExists(const QByteArray &methodName) const;
    static bool routeExists(const QByteArray &routePath);
    static QRPCControllerMethods routeMethods(const QByteArray &className);

    /**
     * @brief request
     * @return
     */
    virtual QRPCListenRequest&request();

    /**
     * @brief rq
     * @return
     */
    virtual QRPCListenRequest&rq();

    /**
     * @brief canAuthorization
     * @return
     */
    virtual bool canAuthorization();

    /**
     * @brief beforeAuthorization
     * @return
     */
    virtual bool beforeAuthorization();

    /**
     * @brief authorization
     * @return
     */
    virtual bool authorization();

    /**
     * @brief afterAuthorization
     * @return
     */
    virtual bool afterAuthorization();

    /**
     * @brief requestBeforeInvoke
     * @return
     */
    virtual bool requestBeforeInvoke();


    /**
     * @brief requestRedirect
     * @return
     */
    virtual bool requestRedirect();

    /**
     * @brief requestAfterInvoke
     * @return
     */
    virtual bool requestAfterInvoke();

    /**
     * @brief server
     * @return
     */
    virtual QRPCServer*server();

    /**
     * @brief registerInterface
     * @param metaObject
     * @return
     */
    static int registerInterface(const QMetaObject *metaObject);
    static int registerParserRequest(const QMetaObject &metaObject);

    /**
     * @brief staticInterfaceList
     * @return
     */
    static QVector<const QMetaObject *> &staticInterfaceList();
    static QVector<const QMetaObject *> &parserRequestList();

    /**
     * @brief methodBlackList
     * @return
     *
     * ignored method list
     */
    static const QVector<QByteArray> &methodBlackList();



protected:
    /**
     * @brief setServer
     * @param server
     */
    virtual void setServer(QRPCServer*server);

    /**
     * @brief setRequest
     * @param request
     */
    virtual void setRequest(QRPCListenRequest&request);

    /**
     * @brief makeRoute
     * @param object
     * @param metaObject
     */
    static void makeRoute(QObject *object, const QMetaObject *metaObject);


    /**
     * @brief routeFlags
     * @param method
     * @return
     */
    virtual QVariantHash routeFlags(const QString&route)const;

    /**
     * @brief routeFlagsMaker
     * @param method
     * @param flag
     * @return
     */
    static const QVariantHash routeFlagsMaker(const QString&request_path, const QVariant &flag);
private:
    void*p = nullptr;
    QRPCProtocol acceptedProtocols;
};

}
