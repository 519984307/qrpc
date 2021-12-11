#pragma once

#include "./qrpc_last_error.h"
#include "./qrpc_global.h"
#include "./qrpc_types.h"
#include "./qstm_url.h"
#include "./qrpc_service_setting.h"
#include "./qrpc_request_exchange.h"
#include "./private/p_qrpc_http_headers.h"
#include "./private/p_qrpc_http_response.h"
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
    Q_INVOKABLE ~classBase(){\
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

    class Q_RPC_EXPORT Body: public QObject{
        friend class QRPCRequestPvt;
    public:
        explicit Body(QObject*parent=nullptr);
        virtual ~Body();
        virtual QVariant &body() const;
        virtual void setBody(const QVariant&value);
        virtual QString toString() const;
        virtual QVariantMap toMap() const;
        virtual QVariantHash toHash() const;
        virtual QVariantList toList() const;

        QRPCRequest&rq();
    private:
        void*p = nullptr;
    };

public:
    Q_INVOKABLE explicit QRPCRequest(QObject *parent=nullptr);
    //explicit QRPCRequest(const ServiceSetting &setting, QObject *parent=nullptr);
    Q_INVOKABLE virtual ~QRPCRequest();

    /**
     * @brief startsWith
     * @param requestPath
     * @return
     */
    static bool startsWith(const QString &requestPath, const QVariant &requestPathBase);
    virtual bool startsWith(const QString &requestPath);
    virtual bool isEqual(const QString &requestPath);
    virtual bool isEqual(const QString &requestPath, const QVariant &requestPathBase);

    /**
     * @brief canRequest
     * @return
     */
    virtual bool canRequest() const;
    /**
     * @brief setSettings
     * @param setting
     */
    virtual QRPCRequest&setSettings(const ServiceSetting&setting);
    virtual QRPCRequest&setSettings(const QVariantHash&setting);

    virtual QString url() const;
    virtual QString url(const QString&path) const;

    virtual QRPCProtocol protocol() const;
    virtual QString protocolName() const;
    virtual QRPCRequest&setProtocol(const QVariant &value);

    virtual QRPCRequestMethod method() const;
    virtual QRPCRequest&setMethod(const QString &value);
    virtual QRPCRequest&setMethod(const QByteArray &value);
    virtual QRPCRequest&setMethod(const int &value);

    /**
     * @brief driver
     * @return
     */
    virtual QString driver() const;
    virtual QRPCRequest&setDriver(const QString &value);

    /**
     * @brief hostName
     * @return
     */
    virtual QString hostName() const;
    virtual QRPCRequest&setHostName(const QString &value);

    /**
     * @brief userName
     * @return
     */
    virtual QString userName() const;
    virtual QRPCRequest&setUserName(const QString &value);

    /**
     * @brief password
     * @return
     */
    virtual QString password() const;
    virtual QRPCRequest&setPassword(const QString &value);

    /**
     * @brief route
     * @return
     */
    virtual QString &route() const;
    virtual QRPCRequest&setRoute(const QVariant &value);

    /**
     * @brief body
     * @return
     */
    virtual QVariant body() const;
    virtual QRPCRequest&setBody(const QVariant &value);

    /**
     * @brief port
     * @return
     */
    virtual QVariant port() const;
    virtual QRPCRequest&setPort(const QVariant &value);

    /**
     * @brief activityLimit
     * @return
     */
    virtual qlonglong activityLimit() const;
    virtual QRPCRequest&setActivityLimit(const QVariant &value);

    /**
     * @brief exchange
     * @return
     */
    virtual QRPCRequestExchange&exchange();

    /**
     * @brief header
     * @return
     */
    virtual QRPCHttpHeaders&header();

    /**
     * @brief body
     * @return
     */
    virtual QRPCRequest::Body&body();

    /**
     * @brief response
     * @return
     */
    virtual QRPCHttpResponse &response();

    /**
     * @brief lastError
     * @return
     */
    virtual LastError&lastError();

    /**
     * @brief call
     * @return
     */
    virtual QRPCHttpResponse&call();

    virtual QRPCHttpResponse&call(const QVariant &route);
    virtual QRPCHttpResponse&call(const QVariant &route, const QVariant&body);

    virtual QRPCHttpResponse&call(const QRPCRequestMethod&method, const QString&route, const QVariant&body);
    virtual QRPCHttpResponse&call(const QRPCRequestMethod&method);

    virtual QRPCHttpResponse&call(const QVariant &route, const QVariant&body, const QString &method, const QVariantHash parameter);

    virtual QRPCHttpResponse&call(const QVariant &route, const QObject&objectBody);
    virtual QRPCHttpResponse&call(const QRPCRequestMethod&method, const QString&route, const QObject&objectBody);

    virtual QRPCHttpResponse&call(const QVariant &route, QIODevice&ioDeviceBody);
    virtual QRPCHttpResponse&call(const QRPCRequestMethod&method, const QString&route);
    virtual QRPCHttpResponse&call(const QRPCRequestMethod&method, const QString&route, QIODevice&ioDeviceBody);

    /**
     * @brief operator =
     * @param value
     * @return
     */
    QRPCRequest&operator=(const ServiceSetting &value);

    /**
     * @brief upload
     * @param file
     * @return
     */
    virtual QRPCHttpResponse&upload(QFile &file);
    virtual QRPCHttpResponse&upload(const QString &route, const QByteArray &buffer);
    virtual QRPCHttpResponse&upload(const QString &route, QFile &file);

    /**
     * @brief download
     * @param fileName
     * @return
     */
    virtual QRPCHttpResponse&download(QString &fileName);
    virtual QRPCHttpResponse&download(const QString &route, QString&fileName);
    virtual QRPCHttpResponse&download(const QUrl&route, QString&fileName);

    /**
     * @brief autoSetCookie
     * @return
     */
    virtual QRPCRequest&autoSetCookie();

    /**
     * @brief toString
     * @return
     */
    virtual QString toString() const;

    //!
    //! \brief toResponse
    //! \return
    //!
    virtual QVariantHash toResponse() const;

    /**
     * @brief sslConfiguration
     * @return
     */
    virtual QSslConfiguration&sslConfiguration();
    virtual QRPCRequest &setSslConfiguration(const QSslConfiguration &value);

    /**
     * @brief print
     * @return
     */
    virtual QRPCRequest &print();

    /**
     * @brief printOut
     * @param output
     * @return
     */
    virtual QStringList printOut();

private:
    void*p = nullptr;
};

}
