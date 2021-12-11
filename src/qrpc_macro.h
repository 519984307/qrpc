#pragma once

#include <QThread>
#include <QDebug>
#include "./qstm_macro.h"

#define Q_RPC_ROUTE_FLAGS(route, r_flags)\
private:\
    QVariantHash _flags_##route()const { return routeFlagsMaker(QStringLiteral(#route), r_flags);};\
    Q_PROPERTY(QVariantHash flags_##route READ _flags_##route)\
public:\

#define QRPC_METHOD_CHECK_GET()\
if(this->rq().isMethodOptions()){\
    this->rq().co().setOK();\
    return {};\
}\
else if(!this->rq().canMethodGet()){\
    this->rq().co().setNotFound();\
    return {};\
}

#define QRPC_METHOD_CHECK_HEADER()\
if(this->rq().isMethodOptions()){\
    this->rq().co().setOK();\
    return {};\
}\
else if(!this->rq().canMethodHead()){\
    this->rq().co().setNotFound();\
    return {};\
}

#define QRPC_METHOD_CHECK_POST()\
if(this->rq().isMethodOptions()){\
    this->rq().co().setOK();\
    return {};\
}\
else if(!this->rq().canMethodPost()){\
    this->rq().co().setNotFound();\
    return {};\
}

#define QRPC_METHOD_CHECK_PUT()\
if(this->rq().isMethodOptions()){\
    this->rq().co().setOK();\
    return {};\
}\
else if(!this->rq().canMethodPut()){\
    this->rq().co().setNotFound();\
    return {};\
}

#define QRPC_METHOD_CHECK_UPLOAD()\
if(this->rq().isMethodOptions()){\
    this->rq().co().setOK();\
    return {};\
}\
else if(!this->rq().isMethodUpload()){\
    this->rq().co().setNotFound();\
    return {};\
}

#define QRPC_METHOD_CHECK_CRUD(vCRUDBody)\
if(this->rq().isMethodOptions()){\
    this->rq().co().setOK();\
    return {};\
}\
else if(!(this->rq().isMethodGet() || this->rq().isMethodPost() || this->rq().isMethodPut() || this->rq().isMethodDelete())){\
    this->rq().co().setBadGateway();\
    return {};\
}\
QVariantHash QRPC_V_CRUD({{QStringLiteral("method"), this->rq().requestMethod()}, {QStringLiteral("source"),this->rq().requestParamHash()}});


#define QRPC_METHOD_CHECK_POST_PUT()\
if(this->rq().isMethodOptions()){\
        this->rq().co().setOK();\
        return {};\
}\
    else if(!(this->rq().canMethodPost() || this->rq().canMethodPut())){\
        this->rq().co().setNotFound();\
        return {};\
}

#define QRPC_METHOD_CHECK_POST_PUT_GET()\
if(this->rq().isMethodOptions()){\
        this->rq().co().setOK();\
        return {};\
}\
    else if(!(this->rq().canMethodPost() || this->rq().canMethodPut() || this->rq().canMethodGet())){\
        this->rq().co().setNotFound();\
        return {};\
}

#define QRPC_METHOD_CHECK_DELETE()\
if(this->rq().isMethodOptions()){\
    this->rq().co().setOK();\
    return {};\
}\
else if(!this->rq().canMethodDelete()){\
    this->rq().co().setNotFound();\
    return {};\
}

#define QRPC_RETURN_ERROR()\
{\
if(this->rq().co().isOK())\
        this->rq().co().setInternalServerError();\
    return {};\
}

#define QRPC_RETURN_BAD_REQUEST() \
{\
    this->rq().co().setBadRequest();\
    return {};\
}

#define QRPC_RETURN_INFO()\
if(this->rq().co().isOK())\
    this->rq().co().setInternalServerError();\
return this->lr().resultVariantInfo();\

#define QRPC_RETURN()\
return {};

#define QRPC_RETURN_VARIANT()\
return {};

#define QRPC_RETURN_NOT_FOUND()\
{\
    this->rq().co().setNotFound();\
    return {};\
}

#define QRPC_V_CRUD\
    vCRUDBody

#define QRPC_V_SET_METHOD(v)\
    auto v=this->rq().requestMethod()

#define QRPC_V_SET_PARAMETERS(v)\
    auto v=this->rq().requestParamHash()

#define QRPC_V_SET(v)\
    auto v=this->rq().requestParamHash(QByteArrayLiteral(#v));\
    this->rq().requestParserProperty()<<QByteArrayLiteral(#v).toLower()

#define QRPC_V_SET_UUID(v)\
    auto v=this->rq().vu.clear().toUuid(this->rq().requestParamHash(QByteArrayLiteral(#v)));\
    Q_UNUSED(v);\
    this->rq().requestParserProperty()<<QByteArrayLiteral(#v).toLower()

#define QRPC_V_SET_BYTE(v)\
    auto v=this->rq().requestParamHash(QByteArrayLiteral(#v)).toByteArray().trimmed();\
    this->rq().requestParserProperty()<<QByteArrayLiteral(#v).toLower()

#define QRPC_V_SET_STRING(v)\
    auto v=this->rq().requestParamHash(QByteArrayLiteral(#v)).toString().trimmed();\
    this->rq().requestParserProperty()<<QByteArrayLiteral(#v).toLower()

#define QRPC_V_SET_LONGLONG(v)\
    auto v=this->rq().requestParamHash(QByteArrayLiteral(#v)).toLongLong();\
    this->rq().requestParserProperty()<<QByteArrayLiteral(#v).toLower()

#define QRPC_V_SET_INT(v)\
    auto v=this->rq().requestParamHash(QByteArrayLiteral(#v)).toInt();\
    this->rq().requestParserProperty()<<QByteArrayLiteral(#v).toLower()

#define QRPC_V_SET_DATE(v)\
    auto v=QDate::fromString(this->rq().requestParamHash(QByteArrayLiteral(#v)).toString(),Qt::ISODate);\
    this->rq().requestParserProperty()<<QByteArrayLiteral(#v).toLower()

#define QRPC_V_SET_DATETIME(v)\
    auto v=QDateTime::fromString(this->rq().requestParamHash(QByteArrayLiteral(#v)).toString(),Qt::ISODateWithMs);\
    this->rq().requestParserProperty()<<QByteArrayLiteral(#v).toLower()

#define QRPC_V_SET_TIME(v)\
    auto v=QTime::fromString(this->rq().requestParamHash(QByteArrayLiteral(#v)).toString(),Qt::ISODateWithMs);\
    this->rq().requestParserProperty()<<QByteArrayLiteral(#v).toLower()

#define QRPC_V_SET_DOUBLE(v)\
    auto v=this->rq().requestParamHash(QByteArrayLiteral(#v)).toDouble();\
    this->rq().requestParserProperty()<<QByteArrayLiteral(#v).toLower()

#define QRPC_V_SET_MAP(v)\
auto v=this->rq().requestParamMap(QByteArrayLiteral(#v)).toMap();\
    this->rq().requestParserProperty()<<QByteArrayLiteral(#v).toLower()

#define QRPC_V_SET_HASH(v)\
auto v=this->rq().requestParamMap(QByteArrayLiteral(#v)).toHash();\
    this->rq().requestParserProperty()<<QByteArrayLiteral(#v).toLower()

#define QRPC_V_SET_LIST(v)\
    auto v=this->rq().requestParamHash(QByteArrayLiteral(#v)).toList();\
    this->rq().requestParserProperty()<<QByteArrayLiteral(#v).toLower()

#define QRPC_V_SET_BOOL(v)\
    auto v=this->rq().requestParamHash(QByteArrayLiteral(#v)).toBool();\
    this->rq().requestParserProperty()<<QByteArrayLiteral(#v).toLower()

#define QRPC_V_SET_BODY(v)\
    auto v=this->rq().requestBody()

#define QRPC_V_SET_BODY_LIST(v)\
    auto v=this->rq().requestBodyList()

#define QRPC_V_SET_BODY_MAP(v)\
    auto v=this->rq().requestBodyMap()

#define QRPC_V_SET_BODY_HASH(v)\
    auto v=this->rq().requestBodyHash()

#define QRPC_V_CHECK_BODY_PARSER()\
    if(!this->rq().requestParserBodyMap()){\
        if(this->rq().co().isOK())\
            this->rq().co().setBadRequest();\
        return {};\
    }

#define QRPC_REDIRECT_VIRIFY()\
public:\
    virtual bool redirectCheck() const override{return true;}

#define QRPC_REDIRECT_NO_VERIFY()\
public:\
    virtual bool redirectCheck()const override{return false;}

#define QRPC_CONTROLLER_AUTO_REGISTER(Controller)\
static auto Controller##MetaObject=QRpc::QRPCController::registerInterface(Controller::staticMetaObject);\

#define QRPC_PARSER_AUTO_REGISTER(ParserObject)\
static auto ParserObject##MetaObject=QRpc::QRPCController::registerParserRequest(ParserObject::staticMetaObject);\

#define QRPC_DECLARE_ROUTE(Controller, v1)\
public:\
Q_INVOKABLE virtual QVariant route()const override\
{\
    return QVariant(v1);\
}\
Q_INVOKABLE virtual void makeRoute() override{\
    QRPCController::makeRoute(this, this->metaObject());\
}

#define QRPC_METHOD_PROPERTIES(method, properties)\
public:\
Q_INVOKABLE virtual QVariant __rpc_properties_##method()const{\
    static QVariantHash ___return({{qsl("method"), #method}, {qsl("properties"), QVariantHash(properties)}});\
    return ___return;\
}

#define QRPC_PARSER_DECLARE_ROUTE(Controller, v1)\
public:\
Q_INVOKABLE virtual QByteArray route()const{\
    return QByteArray(v1).replace(QByteArrayLiteral("\""), QByteArrayLiteral(""));\
}\
Q_INVOKABLE virtual void makeRoute(){\
    QRPCListenRequestParser::makeRoute(this->staticMetaObject);\
}

#define QRPC_DECLARE_MODULE(vmodule)\
public:\
virtual QString module()const override{\
    static QString __return(vmodule);\
    return __return;\
}\
virtual QUuid moduleUuid()const override{\
    VariantUtil vu;\
    static auto __return=vu.toMd5Uuid(vmodule);\
    return __return;\
}

#define QRPC_DECLARE_DESCRIPTION(vdesc)\
public:\
virtual QString description()const override{\
    static QString __return(vdesc);\
    return __return;\
}

#define Q_RPC_DECLARE_INTERFACE_METHOD_CHECK()      \
public:                                             \
Q_INVOKABLE virtual QVariant check()                \
{                                                   \
    QRPC_RETURN_VARIANT();                          \
}                                                   \
Q_INVOKABLE virtual QVariant ping()                 \
{                                                   \
    this->rq().co().setOK();                        \
    return QDateTime::currentDateTime();            \
}                                                   \
Q_INVOKABLE virtual QVariant fullCheck()            \
{                                                   \
    this->rq().co().setOK();                        \
    return {};                                      \
}                                                   \
Q_INVOKABLE virtual QVariant connectionsCheck()     \
{                                                   \
    this->rq().co().setOK();                        \
    return {};                                      \
}                                                   \
Q_INVOKABLE virtual QVariant businessCheck()        \
{                                                   \
    this->rq().co().setNotImplemented();            \
    return {};                                      \
}
