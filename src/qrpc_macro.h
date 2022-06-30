#pragma once

#include <QDebug>
#include <QStm>
#include <QThread>

#define QRPC_METHOD_BACK_LIST {"destroyed", "objectnamechanged", "deletelater", "_q_reregistertimers", "basepathchanged", "enabledchanged", "connection", "connectionid", "connectionclose","connectionclear","connectionfinish"}

#define QRPC_METHOD_CHECK_GET() \
    if (!this->rq().canMethodGet()) { \
        this->rq().co().setNotFound(); \
        return {}; \
    }

#define QRPC_METHOD_CHECK_HEADER() \
    if (!this->rq().canMethodHead()) { \
        this->rq().co().setNotFound(); \
        return {}; \
    }

#define QRPC_METHOD_CHECK_POST() \
    if (!this->rq().canMethodPost()) { \
        this->rq().co().setNotFound(); \
        return {}; \
    }

#define QRPC_METHOD_CHECK_PUT() \
    if (!this->rq().canMethodPut()) { \
        this->rq().co().setNotFound(); \
        return {}; \
    }

#define QRPC_METHOD_CHECK_UPLOAD() \
    if (!this->rq().isMethodUpload()) { \
        this->rq().co().setNotFound(); \
        return {}; \
    }

#define QRPC_METHOD_CHECK_POST_PUT() \
    if (!(this->rq().canMethodPost() || this->rq().canMethodPut())) { \
        this->rq().co().setNotFound(); \
        return {}; \
    }

#define QRPC_METHOD_CHECK_POST_PUT_GET() \
    if (!(this->rq().canMethodPost() || this->rq().canMethodPut() \
                 || this->rq().canMethodGet())) { \
        this->rq().co().setNotFound(); \
        return {}; \
    }

#define QRPC_METHOD_CHECK_DELETE() \
    if (!this->rq().canMethodDelete()) { \
        this->rq().co().setNotFound(); \
        return {}; \
    }

#define QRPC_RETURN_ERROR() \
    { \
        if (this->rq().co().isOK()) \
            this->rq().co().setInternalServerError(); \
        return {}; \
    }

#define QRPC_RETURN_BAD_REQUEST() \
    { \
        this->rq().co().setBadRequest(); \
        return {}; \
    }

#define QRPC_RETURN_INFO() \
    if (this->rq().co().isOK()) \
        this->rq().co().setInternalServerError(); \
    return this->lr().resultVariantInfo();

#define QRPC_RETURN() return {};

#define QRPC_RETURN_VARIANT() return {};

#define QRPC_RETURN_NOT_FOUND() \
    { \
        this->rq().co().setNotFound(); \
        return {}; \
    }

#define QRPC_V_CRUD QVariantHash{{qsl("method"), this->rq().requestMethod()}, {qsl("source"), this->rq().requestParamHash()}}

#define QRPC_V_SET_METHOD(v) auto v = this->rq().requestMethod()

#define QRPC_V_SET_PARAMETERS(v) auto v = this->rq().requestParamHash()

#define QRPC_V_SET(v) \
    auto v = this->rq().requestParamHash(qbl(#v)); \
    this->rq().requestParserProperty().append(qbl(#v).toLower())

#define QRPC_V_SET_UUID(v) \
    auto v = QStm::VariantUtil{}.toUuid(this->rq().requestParamHash(qbl(#v))); \
    Q_UNUSED(v); \
    this->rq().requestParserProperty().append(qbl(#v).toLower())

#define QRPC_V_SET_BYTE(v) \
    auto v = this->rq().requestParamHash(qbl(#v)).toByteArray().trimmed(); \
    this->rq().requestParserProperty().append(qbl(#v).toLower())

#define QRPC_V_SET_STRING(v) \
    auto v = this->rq().requestParamHash(qbl(#v)).toString().trimmed(); \
    this->rq().requestParserProperty().append(qbl(#v).toLower())

#define QRPC_V_SET_LONGLONG(v) \
    auto v = this->rq().requestParamHash(qbl(#v)).toLongLong(); \
    this->rq().requestParserProperty().append(qbl(#v).toLower())

#define QRPC_V_SET_INT(v) \
    auto v = this->rq().requestParamHash(qbl(#v)).toInt(); \
    this->rq().requestParserProperty().append(qbl(#v).toLower())

#define QRPC_V_SET_DATE(v) \
    auto v = QDate::fromString(this->rq().requestParamHash(qbl(#v)).toString(), \
                               Qt::ISODate); \
    this->rq().requestParserProperty().append(qbl(#v).toLower())

#define QRPC_V_SET_DATETIME(v) \
    auto v = QDateTime::fromString(this->rq().requestParamHash(qbl(#v)).toString(), \
                                   Qt::ISODateWithMs); \
    this->rq().requestParserProperty().append(qbl(#v).toLower())

#define QRPC_V_SET_TIME(v) \
    auto v = QTime::fromString(this->rq().requestParamHash(qbl(#v)).toString(), \
                               Qt::ISODateWithMs); \
    this->rq().requestParserProperty().append(qbl(#v).toLower())

#define QRPC_V_SET_DOUBLE(v) \
    auto v = this->rq().requestParamHash(qbl(#v)).toDouble(); \
    this->rq().requestParserProperty().append(qbl(#v).toLower())

#define QRPC_V_SET_MAP(v) \
    auto v = this->rq().requestParamHash(qbl(#v)).toMap(); \
    this->rq().requestParserProperty().append(qbl(#v).toLower())

#define QRPC_V_SET_HASH(v) \
    auto v = this->rq().requestParamHash(qbl(#v)).toHash(); \
    this->rq().requestParserProperty().append(qbl(#v).toLower())

#define QRPC_V_SET_LIST(v) \
    auto v = this->rq().requestParamHash(qbl(#v)).toList(); \
    this->rq().requestParserProperty().append(qbl(#v).toLower())

#define QRPC_V_SET_BOOL(v) \
    auto v = this->rq().requestParamHash(qbl(#v)).toBool(); \
    this->rq().requestParserProperty().append(qbl(#v).toLower())

#define QRPC_V_SET_BODY(v) auto v = this->rq().requestBody()

#define QRPC_V_SET_BODY_LIST(v) auto v = this->rq().requestBodyList()

#define QRPC_V_SET_BODY_MAP(v) auto v = this->rq().requestBodyMap()

#define QRPC_V_SET_BODY_HASH(v) auto v = this->rq().requestBodyHash()

#define QRPC_V_CHECK_BODY_PARSER() \
    if (!this->rq().requestParserBodyHash()) { \
        if (this->rq().co().isOK()) \
            this->rq().co().setBadRequest(); \
        return {}; \
    }

#define QRPC_CONTROLLER_AUTO_REGISTER(ClassName) \
    static auto ClassName##ApiInstalled = QRpc::Controller::install(ClassName::staticMetaObject);

#define QRPC_PARSER_AUTO_REGISTER(ParserObject) \
    static auto ClassName##ApiParserInstalled = QRpc::Controller::installParser( \
        ParserObject::staticMetaObject);

#define QRPC_LISTTEN_AUTO_REGISTER(Type, Listen) \
    static const auto &Listen##Type##_MetaObject = QRpc::Listen::install(Type, Listen::staticMetaObject);

#define Q_RPC_DECLARE_INTERFACE_METHOD_CHECK() \
public: \
    Q_INVOKABLE virtual QVariant check() \
    { \
        this->rq().co().setOK(); \
        return {}; \
    } \
    Q_INVOKABLE virtual QVariant ping() \
    { \
        this->rq().co().setOK(); \
        return QDateTime::currentDateTime(); \
    } \
    Q_INVOKABLE virtual QVariant fullCheck() \
    { \
        this->rq().co().setOK(); \
        return {}; \
    } \
    Q_INVOKABLE virtual QVariant connectionsCheck() \
    { \
        this->rq().co().setOK(); \
        return {}; \
    } \
    Q_INVOKABLE virtual QVariant businessCheck() \
    { \
        this->rq().co().setNotImplemented(); \
        return {}; \
    }
