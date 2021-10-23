#pragma once

#include "./qrpc_global.h"

namespace QRpc {

class Q_RPC_EXPORT QRPCHttpHeaders: public QObject{
    Q_OBJECT
public:
    Q_INVOKABLE explicit QRPCHttpHeaders(QObject*parent=nullptr);
    Q_INVOKABLE explicit QRPCHttpHeaders(const QVariant&v,QObject*parent=nullptr);
    Q_INVOKABLE virtual ~QRPCHttpHeaders();

    /**
     * @brief clear
     * @return
     */
    virtual QRPCHttpHeaders&clear();

    /**
     * @brief rawHeader
     * @return
     */
    virtual QVariantHash &rawHeader() const;
    virtual QVariant rawHeader(const QString &headername) const;
    virtual QRPCHttpHeaders&setRawHeader(const QVariantHash&rawHeader);
    virtual QRPCHttpHeaders&setRawHeader(const QString &header, const QVariant &value);
    virtual QRPCHttpHeaders&addRawHeader(const QVariantHash&rawHeader);
    virtual QRPCHttpHeaders&addRawHeader(const QString &header, const QVariant &value);

    /**
     * @brief setContentType
     * @param contentType
     * @return
     */
    virtual QVariant contentType() const;
    virtual QRPCHttpHeaders&setContentType(const int contentType);
    virtual QRPCHttpHeaders&setContentType(const QVariant &v);
    virtual bool isContentType(int contentType)const;
    static QRpc::ContentType defaultContentType();

    /**
     * @brief contentDisposition
     * @return
     */
    virtual QVariant contentDisposition() const;

    /**
     * @brief setAuthorization
     * @param authorization
     * @param type
     * @param credentials
     * @return
     */
    virtual QRPCHttpHeaders&setAuthorization(const QString&authorization, const QString&type, const QVariant &credentials);
    virtual QRPCHttpHeaders&setAuthorization(const QString&authorization, const AuthorizationType&type, const QVariant &credentials);
    virtual QRPCHttpHeaders&setAuthorization(const AuthorizationType&type, const QVariant &credentials);
    virtual QRPCHttpHeaders&setAuthorization(const AuthorizationType&type, const QVariant &userName, const QVariant &passWord);
    //virtual Header&setAuthorization(const QString&type, const QVariant &credentials);

    /**
     * @brief setProxyAuthorization
     * @param type
     * @param credentials
     * @return
     */
    virtual QRPCHttpHeaders&setProxyAuthorization(const AuthorizationType&type, const QVariant &credentials);
    virtual QRPCHttpHeaders&setProxyAuthorization(const QString&type, const QVariant &credentials);

    /**
     * @brief setWWWAuthenticate
     * @param type
     * @param credentials
     * @return
     */
    virtual QRPCHttpHeaders&setWWWAuthenticate(const AuthorizationType&type, const QVariant &credentials);
    virtual QRPCHttpHeaders&setWWWAuthenticate(const QString&type, const QVariant &credentials);

    /**
     * @brief setCookie
     * @param cookie
     * @return
     */
    virtual QVariant cookies() const;
    virtual QRPCHttpHeaders&setCookies(const QVariant&cookie);

    /**
     * @brief authorization
     * @param authorization
     * @param type
     * @return
     */
    virtual QVariant authorization(const QString&authorization, const QString&type);
    virtual QVariant authorization(const QString&authorization, const AuthorizationType &type);
    virtual QVariant authorization(const AuthorizationType&type);
    virtual QVariant authorization(const QString&type);

    /**
     * @brief proxyAuthorization
     * @param type
     * @return
     */
    virtual QVariant proxyAuthorization(const AuthorizationType&type);
    virtual QVariant proxyAuthorization(const QString&type);

    /**
     * @brief wwwAuthenticate
     * @param type
     * @return
     */
    virtual QVariant wwwAuthenticate(const AuthorizationType&type);
    virtual QVariant wwwAuthenticate(const QString&type);

    /**
     * @brief print
     */
    virtual QRPCHttpHeaders&print(const QString&output=QString());

    /**
     * @brief printOut
     * @param output
     * @return
     */
    virtual QStringList printOut(const QString&output=QString());

    QRPCHttpHeaders&operator =(const QVariant&v);
    QRPCHttpHeaders&operator <<(const QVariant&v);
private:
    void*p = nullptr;
};

}
