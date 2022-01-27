#pragma once

#include "./qrpc_last_error.h"
#include "./qrpc_global.h"
#include "./qrpc_types.h"
#include "../../qrpc/src/qrpc_service_setting.h"
#include "../../qrpc/src/qrpc_request_exchange.h"
#include "./private/p_qrpc_http_headers.h"
#include "./private/p_qrpc_http_response.h"
#include <QStm>
#include <QFile>
#include <QSslConfiguration>

namespace QRpc {

#define QRPC_DECLARE_CONTENT_TYPE(type)\
public:\
    Q_INVOKABLE virtual QRpc::ContentType contentType(){\
        return QRpc::ContentType(type);\
    }\

#define QRPC_DECLARE_BASE_ROUTE(route)\
public:\
    Q_INVOKABLE virtual QByteArray baseRoute(){\
        return #route;\
    }\

#define QRPC_DECLARE_CONSTRUCTOR(classBase)\
public:\
    Q_INVOKABLE explicit classBase(QObject *parent=nullptr):QRpc::QRPCRequest(parent){\
    }\
    ~classBase(){\
    }

#define QRPC_DECLARE_REQUEST_CLASS(classBase,ContentType,route)\
class classBase:public QRpc::QRPCRequest{\
public:\
    QRPC_DECLARE_CONTENT_TYPE(ContentType)\
    QRPC_DECLARE_BASE_ROUTE(route)\
    QRPC_DECLARE_CONSTRUCTOR(classBase)\
};

#define QRPC_DECLARE_METHOD(route)\
    Q_INVOKABLE virtual QByteArray baseRoute(){\
        return #route;\
    }\


#define QRPC_DECLARE_REQUEST_ROUTE(Controller, v1)\
public:\
    Q_INVOKABLE virtual QByteArray route(){\
        return QByteArrayLiteral(#v1).replace(QByteArrayLiteral("\""), QByteArrayLiteral(""));\
    }\

class QRPCListenRequest;

//!
//! \brief The QRPCRequest class
//!
class Q_RPC_EXPORT QRPCRequest: public QObject
{
    Q_OBJECT
public:

    enum Action {
        acRequest=1, acDownload=2, acUpload=3
    };

    Q_ENUM(Action)

public:
    QRPC_DECLARE_CONTENT_TYPE(QRpc::AppJson)
    QRPC_DECLARE_BASE_ROUTE(/)

    //!
    //! \brief The Body class
    //!
    class Q_RPC_EXPORT Body: public QObject{
        friend class QRPCRequestPvt;
    public:

        //!
        //! \brief Body
        //! \param parent
        //!
        explicit Body(QObject*parent=nullptr);

        virtual ~Body();

        //!
        //! \brief body
        //! \return
        //!
        virtual QVariant &body() const;

        //!
        //! \brief setBody
        //! \param value
        //!
        virtual void setBody(const QVariant&value);

        //!
        //! \brief toString
        //! \return
        //!
        virtual QString toString() const;

        //!
        //! \brief toMap
        //! \return
        //!
        virtual QVariantMap toMap() const;

        //!
        //! \brief toHash
        //! \return
        //!
        virtual QVariantHash toHash() const;

        //!
        //! \brief toList
        //! \return
        //!
        virtual QVariantList toList() const;

        //!
        //! \brief rq
        //! \return
        //!
        QRPCRequest&rq();
    private:
        void*p = nullptr;
    };

public:
    Q_INVOKABLE explicit QRPCRequest(QObject *parent=nullptr);
    //explicit QRPCRequest(const ServiceSetting &setting, QObject *parent=nullptr);
    ~QRPCRequest();

    //!
    //! \brief startsWith
    //! \param requestPath
    //! \param requestPathBase
    //! \return
    //!
    static bool startsWith(const QString &requestPath, const QVariant &requestPathBase);
    virtual bool startsWith(const QString &requestPath);

    //!
    //! \brief isEqual
    //! \param requestPath
    //! \return
    //!
    virtual bool isEqual(const QString &requestPath);
    virtual bool isEqual(const QString &requestPath, const QVariant &requestPathBase);

    //!
    //! \brief canRequest
    //! \return
    //!
    virtual bool canRequest() const;

    //!
    //! \brief setSettings
    //! \param setting
    //! \return
    //!
    virtual QRPCRequest&setSettings(const ServiceSetting&setting);
    virtual QRPCRequest&setSettings(const QVariantHash&setting);

    //!
    //! \brief url
    //! \return
    //!
    virtual QString url() const;
    virtual QString url(const QString&path) const;

    //!
    //! \brief protocol
    //! \return
    //!
    virtual QRPCProtocol protocol() const;
    virtual QString protocolName() const;
    virtual QRPCRequest&setProtocol(const QVariant &value);

    //!
    //! \brief method
    //! \return
    //!
    virtual QRPCRequestMethod method() const;
    virtual QRPCRequest&setMethod(const QString &value);
    virtual QRPCRequest&setMethod(const QByteArray &value);
    virtual QRPCRequest&setMethod(const int &value);

    //!
    //! \brief driver
    //! \return
    //!
    virtual QString driver() const;
    virtual QRPCRequest&setDriver(const QString &value);

    //!
    //! \brief hostName
    //! \return
    //!
    virtual QString hostName() const;
    virtual QRPCRequest&setHostName(const QString &value);

    //!
    //! \brief userName
    //! \return
    //!
    virtual QString userName() const;
    virtual QRPCRequest&setUserName(const QString &value);

    //!
    //! \brief password
    //! \return
    //!
    virtual QString password() const;
    virtual QRPCRequest&setPassword(const QString &value);

    //!
    //! \brief route
    //! \return
    //!
    virtual QString &route() const;
    virtual QRPCRequest&setRoute(const QVariant &value);

    //!
    //! \brief body
    //! \return
    //!
    virtual QVariant body() const;
    virtual QRPCRequest&setBody(const QVariant &value);

    //!
    //! \brief port
    //! \return
    //!
    virtual QVariant port() const;
    virtual QRPCRequest&setPort(const QVariant &value);

    //!
    //! \brief activityLimit
    //! \return
    //!
    virtual qlonglong activityLimit() const;
    virtual QRPCRequest&setActivityLimit(const QVariant &value);

    //!
    //! \brief exchange
    //! \return
    //!
    virtual QRPCRequestExchange&exchange();

    //!
    //! \brief header
    //! \return
    //!
    virtual QRPCHttpHeaders&header();

    //!
    //! \brief body
    //! \return
    //!
    virtual QRPCRequest::Body&body();

    //!
    //! \brief response
    //! \return
    //!
    virtual QRPCHttpResponse &response();

    //!
    //! \brief lastError
    //! \return
    //!
    virtual LastError&lastError();

    //!
    //! \brief call
    //! \return
    //!
    virtual QRPCHttpResponse&call();

    //!
    //! \brief call
    //! \param route
    //! \return
    //!
    virtual QRPCHttpResponse&call(const QVariant &route);

    //!
    //! \brief call
    //! \param route
    //! \param body
    //! \return
    //!
    virtual QRPCHttpResponse&call(const QVariant &route, const QVariant&body);

    //!
    //! \brief call
    //! \param method
    //! \param route
    //! \param body
    //! \return
    //!
    virtual QRPCHttpResponse&call(const QRPCRequestMethod&method, const QString&route, const QVariant&body);

    //!
    //! \brief call
    //! \param method
    //! \return
    //!
    virtual QRPCHttpResponse&call(const QRPCRequestMethod&method);

    //!
    //! \brief call
    //! \param route
    //! \param body
    //! \param method
    //! \param parameter
    //! \return
    //!
    virtual QRPCHttpResponse&call(const QVariant &route, const QVariant&body, const QString &method, const QVariantHash parameter);

    //!
    //! \brief call
    //! \param route
    //! \param objectBody
    //! \return
    //!
    virtual QRPCHttpResponse&call(const QVariant &route, const QObject&objectBody);

    //!
    //! \brief call
    //! \param method
    //! \param route
    //! \param objectBody
    //! \return
    //!
    virtual QRPCHttpResponse&call(const QRPCRequestMethod&method, const QString&route, const QObject&objectBody);

    //!
    //! \brief call
    //! \param route
    //! \param ioDeviceBody
    //! \return
    //!
    virtual QRPCHttpResponse&call(const QVariant &route, QIODevice&ioDeviceBody);

    //!
    //! \brief call
    //! \param method
    //! \param route
    //! \return
    //!
    virtual QRPCHttpResponse&call(const QRPCRequestMethod&method, const QString&route);

    //!
    //! \brief call
    //! \param method
    //! \param route
    //! \param ioDeviceBody
    //! \return
    //!
    virtual QRPCHttpResponse&call(const QRPCRequestMethod&method, const QString&route, QIODevice&ioDeviceBody);

    //!
    //! \brief operator =
    //! \param value
    //! \return
    //!
    QRPCRequest&operator=(const ServiceSetting &value);

    //!
    //! \brief upload
    //! \param file
    //! \return
    //!
    virtual QRPCHttpResponse&upload(QFile &file);

    //!
    //! \brief upload
    //! \param route
    //! \param buffer
    //! \return
    //!
    virtual QRPCHttpResponse&upload(const QVariant &route, const QByteArray &buffer);

    //!
    //! \brief upload
    //! \param route
    //! \param file
    //! \return
    //!
    virtual QRPCHttpResponse&upload(const QVariant &route, QFile &file);

    //!
    //! \brief upload
    //! \param route
    //! \param fileName
    //! \param file
    //! \return
    //!
    virtual QRPCHttpResponse&upload(const QVariant &route, QString&fileName, QFile &file);

    //!
    //! \brief download
    //! \param fileName
    //! \return
    //!
    virtual QRPCHttpResponse&download(QString &fileName);

    //!
    //! \brief download
    //! \param route
    //! \param fileName
    //! \return
    //!
    virtual QRPCHttpResponse&download(const QVariant &route, QString&fileName);

    //!
    //! \brief download
    //! \param route
    //! \param fileName
    //! \param parameter
    //! \return
    //!
    virtual QRPCHttpResponse&download(const QVariant &route, QString&fileName, const QVariant &parameter);

    //!
    //! \brief autoSetCookie
    //! \return
    //!
    virtual QRPCRequest&autoSetCookie();

    //!
    //! \brief toString
    //! \return
    //!
    virtual QString toString() const;

    //!
    //! \brief toResponse
    //! \return
    //!
    virtual QVariantHash toResponse() const;

    //!
    //! \brief sslConfiguration
    //! \return
    //!
    virtual QSslConfiguration&sslConfiguration();
    virtual QRPCRequest &setSslConfiguration(const QSslConfiguration &value);

    //!
    //! \brief print
    //! \return
    //!
    virtual QRPCRequest &print();

    //!
    //! \brief printOut
    //! \return
    //!
    virtual QStringList printOut();

private:
    void*p = nullptr;
};

}
