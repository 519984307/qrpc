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
    return QVariant();\
}\
else if(!this->rq().canMethodGet()){\
    this->rq().co().setNotFound();\
    return QVariant();\
}

#define QRPC_METHOD_CHECK_HEADER()\
if(this->rq().isMethodOptions()){\
    this->rq().co().setOK();\
    return QVariant();\
}\
else if(!this->rq().canMethodHead()){\
    this->rq().co().setNotFound();\
    return QVariant();\
}

#define QRPC_METHOD_CHECK_POST()\
if(this->rq().isMethodOptions()){\
    this->rq().co().setOK();\
    return QVariant();\
}\
else if(!this->rq().canMethodPost()){\
    this->rq().co().setNotFound();\
    return QVariant();\
}

#define QRPC_METHOD_CHECK_PUT()\
if(this->rq().isMethodOptions()){\
    this->rq().co().setOK();\
    return QVariant();\
}\
else if(!this->rq().canMethodPut()){\
    this->rq().co().setNotFound();\
    return QVariant();\
}

#define QRPC_METHOD_CHECK_UPLOAD()\
if(this->rq().isMethodOptions()){\
    this->rq().co().setOK();\
    return QVariant();\
}\
else if(!this->rq().isMethodUpload()){\
    this->rq().co().setNotFound();\
    return QVariant();\
}

#define QRPC_METHOD_CHECK_CRUD(vCRUDBody)\
if(this->rq().isMethodOptions()){\
    this->rq().co().setOK();\
    return QVariant();\
}\
else if(!(this->rq().isMethodGet() || this->rq().isMethodPost() || this->rq().isMethodPut() || this->rq().isMethodDelete())){\
    this->rq().co().setBadGateway();\
    return QVariant();\
}\
QVariantHash QRPC_V_CRUD({{QStringLiteral("method"), this->rq().requestMethod()}, {QStringLiteral("source"),this->rq().requestParamHash()}});


#define QRPC_METHOD_CHECK_POST_PUT()\
if(this->rq().isMethodOptions()){\
    this->rq().co().setOK();\
    return QVariant();\
}\
else if(!(this->rq().canMethodPost() || this->rq().canMethodPut())){\
    this->rq().co().setNotFound();\
    return QVariant();\
}

#define QRPC_METHOD_CHECK_DELETE()\
if(this->rq().isMethodOptions()){\
    this->rq().co().setOK();\
    return QVariant();\
}\
else if(!this->rq().canMethodDelete()){\
    this->rq().co().setNotFound();\
    return QVariant();\
}

#define QRPC_RETURN_ERROR()\
    if(this->rq().co().isOK())\
        this->rq().co().setInternalServerError();\
    return QVariant();

#define QRPC_RETURN()\
return QVariant()

#define QRPC_RETURN_VARIANT()\
return QVariant();

#define QRPC_RETURN_NOT_FOUND()\
{\
    this->rq().co().setNotFound();\
    return QVariant();\
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
    auto v=this->rq().requestParamMap(QByteArrayLiteral(#v)).toHash();\
    this->rq().requestParserProperty()<<QByteArrayLiteral(#v).toLower()

#define QRPC_V_SET_LIST(v)\
    auto v=this->rq().requestParamHash(QByteArrayLiteral(#v)).toList();\
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
        return QVariant();\
    }

#define QRPC_REDIRECT_VIRIFY()\
public:\
    virtual bool redirectCheck(){return true;}

#define QRPC_REDIRECT_NO_VERIFY()\
public:\
    virtual bool redirectCheck(){return false;}

#define QRPC_CONTROLLER_AUTO_REGISTER(Controller)\
static auto Controller##MetaObject=QRpc::QRPCController::registerInterface(&Controller::staticMetaObject);\

#define QRPC_PARSER_AUTO_REGISTER(ParserObject)\
static auto ParserObject##MetaObject=QRpc::QRPCController::registerParserRequest(ParserObject::staticMetaObject);\

#define QRPC_DECLARE_ROUTE(Controller, v1)\
public:\
    Q_INVOKABLE virtual QVariant route()const{\
        return QVariant(v1);\
    }\
    Q_INVOKABLE virtual void makeRoute(){\
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
    virtual QString module()const{\
        static QString __return(vmodule);\
        return __return;\
    }\
    virtual QUuid moduleUuid(){\
        VariantUtil vu;\
        static auto __return=vu.toMd5Uuid(vmodule);\
        return __return;\
    }

#define QRPC_DECLARE_DESCRIPTION(vdesc)\
public:\
    virtual QString description()const{\
        static QString __return(vdesc);\
        return __return;\
    }
