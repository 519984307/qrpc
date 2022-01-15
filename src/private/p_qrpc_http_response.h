#pragma once

#include "./p_qrpc_http_headers.h"
#include <QNetworkReply>

namespace QRpc {

//!
//! \brief The QRPCHttpResponse class
//!
class Q_RPC_EXPORT QRPCHttpResponse: public QObject{
    Q_OBJECT
public:
    //!
    //! \brief QRPCHttpResponse
    //! \param parent
    //!
    Q_INVOKABLE explicit QRPCHttpResponse(QObject*parent=nullptr);
    ~QRPCHttpResponse();

    //!
    //! \brief header
    //! \return
    //!
    virtual QRpc::QRPCHttpHeaders&header() const;

    //!
    //! \brief setBody
    //! \param vBody
    //!
    virtual void setBody(const QVariant&vBody);

    //!
    //! \brief body
    //! \return
    //!
    virtual QByteArray&body() const;

    //!
    //! \brief bodyVariant
    //! \return
    //!
    virtual QVariant bodyVariant() const;

    //!
    //! \brief bodyMap
    //! \return
    //!
    virtual QVariantMap bodyMap() const;

    //!
    //! \brief bodyHash
    //! \return
    //!
    virtual QVariantHash bodyHash() const;

    //!
    //! \brief bodyObject
    //! \return
    //!
    virtual QVariantHash bodyObject() const;

    //!
    //! \brief bodyList
    //! \return
    //!
    virtual QVariantList bodyList() const;

    //!
    //! \brief bodyArray
    //! \return
    //!
    virtual QVariantList bodyArray() const;

    //!
    //! \brief bodyToList
    //! \return
    //!
    virtual QVariantList bodyToList() const;

    //!
    //! \brief statusCode
    //! \return
    //!
    virtual int&statusCode() const;

    //!
    //! \brief reasonPhrase
    //! \return
    //!
    virtual QString &reasonPhrase() const;

    //!
    //! \brief qtStatusCode
    //! \return
    //!
    virtual QNetworkReply::NetworkError &qtStatusCode() const;

    //!
    //! \brief toMap
    //! \return
    //!
    virtual QVariantHash toMap() const;

    //!
    //! \brief toHash
    //! \return
    //!
    virtual QVariantHash toHash() const;

    //!
    //! \brief toResponse
    //! \return
    //!
    virtual QVariantHash toResponse() const;

    //!
    //! \brief isOk
    //! \return
    //!
    virtual bool isOk() const;

    //!
    //! \brief isCreated
    //! \return
    //!
    virtual bool isCreated() const;

    //!
    //! \brief isNotFound
    //! \return
    //!
    virtual bool isNotFound() const;

    //!
    //! \brief isUnAuthorized
    //! \return
    //!
    virtual bool isUnAuthorized() const;

    //!
    //! \brief setResponse
    //! \param objectResponse
    //! \return
    //!
    virtual QRPCHttpResponse&setResponse(QObject *objectResponse);

    //!
    //! \brief toString
    //! \return
    //!
    QString toString() const;

    //!
    //! \brief operator bool
    //!
    operator bool() const;

    //!
    //! \brief print
    //! \param output
    //! \return
    //!
    virtual QRPCHttpResponse &print(const QString&output=QString());

    //!
    //! \brief printOut
    //! \param output
    //! \return
    //!
    virtual QStringList printOut(const QString&output=QString());

private:
    void*p = nullptr;
};


}
