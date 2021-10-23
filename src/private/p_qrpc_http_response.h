#pragma once

#include "./p_qrpc_http_headers.h"
#include <QNetworkReply>

namespace QRpc {

class Q_RPC_EXPORT QRPCHttpResponse: public QObject{
    Q_OBJECT
public:
    Q_INVOKABLE explicit QRPCHttpResponse(QObject*parent=nullptr);
    Q_INVOKABLE virtual ~QRPCHttpResponse();
    virtual QRpc::QRPCHttpHeaders&header() const;
    virtual void setBody(const QVariant&vBody);
    virtual QByteArray&body() const;
    virtual QVariant bodyVariant() const;
    virtual QVariantMap bodyMap() const;
    virtual QVariantHash bodyHash() const;
    virtual QVariantHash bodyObject() const;
    virtual QVariantList bodyList() const;
    virtual QVariantList bodyArray() const;
    virtual QVariantList bodyToList() const;
    virtual int&statusCode() const;
    virtual QString &reasonPhrase() const;
    virtual QNetworkReply::NetworkError &qtStatusCode() const;
    virtual QVariantHash toMap() const;
    virtual QVariantHash toHash() const;
    virtual bool isOk() const;
    virtual bool isCreated() const;
    virtual bool isNotFound() const;
    virtual bool isUnAuthorized() const;

    virtual QRPCHttpResponse&setResponse(QObject *objectResponse);

    QString toString() const;

    operator bool() const;

    /**
     * @brief print
     * @param output
     */
    virtual QRPCHttpResponse &print(const QString&output=QString());

    /**
     * @brief printOut
     * @param output
     * @return
     */
    virtual QStringList printOut(const QString&output=QString());

private:
    void*p = nullptr;
};


}
