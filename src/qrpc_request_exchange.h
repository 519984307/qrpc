#pragma once

#include "./qrpc_global.h"
#include "./qrpc_request_exchange_setting.h"

namespace QRpc {

class Q_RPC_EXPORT QRPCRequestExchange: public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit QRPCRequestExchange(QObject *parent=nullptr);
    explicit QRPCRequestExchange(QRPCRequestExchange&exchange, QObject *parent);
    Q_INVOKABLE virtual ~QRPCRequestExchange();

    /**
     * @brief operator =
     * @param v
     * @return
     */
    QRPCRequestExchange &operator =(QRPCRequestExchange &e);

    /**
     * @brief call
     * @return
     */
    virtual QRPCRequestExchangeSetting&call();

    /**
     * @brief back
     * @return
     */
    virtual QRPCRequestExchangeSetting&back();

    /**
     * @brief toMap
     * @return
     */
    virtual QVariantMap toMap() const;

    /**
     * @brief toHash
     * @return
     */
    virtual QVariantHash toHash() const;

    /**
     * @brief print
     */
    virtual QRPCRequestExchange&print(const QString&output=QString());

    /**
     * @brief printOut
     * @param output
     * @return
     */
    virtual QStringList printOut(const QString&output=QString());

    /**
     * @brief setProtocol
     * @param value
     */
    virtual void setProtocol(const QVariant &value);
    virtual void setProtocol(const QRPCProtocol &value);

    /**
     * @brief setMethod
     * @param value
     *
     * default method post
     */
    virtual void setMethod(const QString &value);
    virtual void setMethod(const QRPCRequestMethod &value);

    virtual void setHostName(const QString &value);
    virtual void setRoute(const QVariant &value);
    virtual void setPort(const int &value);

private:
    void*p = nullptr;
};

}
