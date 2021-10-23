#include "./p_qrpc_http_response.h"
#include "./p_qrpc_listen_request_code.h"
#include "./p_qrpc_request_job_response.h"
#include "./qstm_util_variant.h"
#include "./p_qrpc_util.h"
#include "../qrpc_macro.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QtCborCommon>
#include <QCborStreamReader>
#include <QCborStreamWriter>
#include <QCborArray>
#include <QCborMap>
#include <QCborValue>
#include <QCborParserError>

namespace QRpc {

#define dPvt()\
    auto&p =*reinterpret_cast<QRPCHttpResponsePvt*>(this->p)

class QRPCHttpResponsePvt{
public:
    QRPCHttpResponse*parent=nullptr;
    QRpc::QRPCHttpHeaders response_header;
    int response_status_code=0;
    QNetworkReply::NetworkError response_qt_status_code=QNetworkReply::NoError;
    QString response_reason_phrase;
    QByteArray response_body;
    explicit QRPCHttpResponsePvt(QRPCHttpResponse*parent): response_header(parent){
    }

    virtual ~QRPCHttpResponsePvt(){
    }
};


QRPCHttpResponse::QRPCHttpResponse(QObject *parent):QObject(parent)
{
    this->p = new QRPCHttpResponsePvt(this);
}

QRPCHttpResponse::~QRPCHttpResponse()
{
    dPvt();
    delete&p;
}

QRPCHttpHeaders &QRPCHttpResponse::header() const
{
    dPvt();
    return p.response_header;
}

void QRPCHttpResponse::setBody(const QVariant &vBody)
{
    dPvt();
    auto t=vBody.type();
    if(t==QVariant::Map || t==QVariant::Hash || t==QVariant::List || t==QVariant::String){
        if(this->header().isContentType(AppJson)){
            auto doc=QJsonDocument::fromVariant(vBody);
            p.response_body=doc.toJson(doc.Compact);
        }
        else if(this->header().isContentType(AppCBOR) || this->header().isContentType(AppOctetStream)){
            auto doc=QCborValue::fromVariant(vBody);
            p.response_body=doc.toByteArray();
        }
        else{//default json
            auto doc=QJsonDocument::fromVariant(vBody);
            p.response_body=doc.toJson(doc.Compact);
        }
    }
    else{
        p.response_body=vBody.toByteArray();
    }
}

QByteArray &QRPCHttpResponse::body() const
{
    dPvt();
    return p.response_body;
}

QVariant QRPCHttpResponse::bodyVariant() const
{
    dPvt();
    //    QVariant v;
    QJsonParseError*error=nullptr;
    auto body=p.response_body.trimmed();
    if(body.isEmpty()){
        return QVariant();
    }
    else if(this->header().isContentType(AppJson)){
        auto vdoc=QJsonDocument::fromJson(p.response_body, error).toVariant();
        if(!vdoc.isNull() && !vdoc.isValid())
            return vdoc;
    }
    else if(this->header().isContentType(AppCBOR) || this->header().isContentType(AppOctetStream)){
        auto vdoc=QCborValue::fromVariant(p.response_body).toVariant();
        if(!vdoc.isNull() && !vdoc.isValid())
            return vdoc;
    }

    {
        auto body=p.response_body.trimmed();
        if(body.at(0)=='[' || body.at(0)=='{'){
            auto doc=QJsonDocument::fromJson(p.response_body, error);
            if(!doc.isNull() && (doc.isArray() || doc.isObject()))
                return doc.toVariant();
        }
        else{
            auto doc=QCborValue::fromVariant(p.response_body);
            if(!doc.isNull() && (doc.isArray() || doc.isMap()))
                return doc.toVariant();
        }
    }
    return p.response_body;
}

QVariantMap QRPCHttpResponse::bodyMap() const
{
    return this->bodyVariant().toMap();
}

QVariantHash QRPCHttpResponse::bodyHash() const
{
    return this->bodyVariant().toHash();
}

QVariantHash QRPCHttpResponse::bodyObject() const
{
    return this->bodyVariant().toHash();
}

QVariantList QRPCHttpResponse::bodyList() const
{
    return this->bodyVariant().toList();
}

QVariantList QRPCHttpResponse::bodyArray() const
{
    return this->bodyVariant().toList();
}

QVariantList QRPCHttpResponse::bodyToList() const
{
    auto v=this->bodyVariant();
    if(v.type()==QVariant::List || v.type()==QVariant::StringList)
        return v.toList();
    else if(v.isValid())
        return QVariantList()<<v;
    else
        return QVariantList();
}

int &QRPCHttpResponse::statusCode() const
{
    dPvt();
    return p.response_status_code;
}

QString &QRPCHttpResponse::reasonPhrase() const
{
    dPvt();
    return p.response_reason_phrase;
}

QNetworkReply::NetworkError &QRPCHttpResponse::qtStatusCode() const
{
    dPvt();
    return p.response_qt_status_code;
}

QVariantHash QRPCHttpResponse::toMap() const
{
    dPvt();
    return QJsonDocument::fromJson(p.response_body).object().toVariantHash();
}

QVariantHash QRPCHttpResponse::toHash() const
{
    dPvt();
    return QJsonDocument::fromJson(p.response_body).object().toVariantHash();
}

bool QRPCHttpResponse::isOk() const
{
    dPvt();
    if(p.response_status_code==200)
        return true;
    else if(p.response_qt_status_code==QNetworkReply::NoError)
        return true;
    else
        return false;
}

bool QRPCHttpResponse::isCreated() const
{
    dPvt();
    if(p.response_qt_status_code!=QNetworkReply::NoError)
        return false;
    else if(p.response_status_code!=0 && p.response_status_code!=201)
        return false;
    else
        return true;
}

bool QRPCHttpResponse::isNotFound() const
{
    dPvt();
    if(p.response_status_code==404)
        return true;
    else
        return false;
}

bool QRPCHttpResponse::isUnAuthorized() const
{
    dPvt();
    if(p.response_status_code==401)
        return true;
    else
        return false;
}

QRPCHttpResponse &QRPCHttpResponse::setResponse(QObject *objectResponse)
{
    dPvt();
    if(objectResponse!=nullptr){
        if(QRpc::QRPCRequestJobResponse::staticMetaObject.cast(objectResponse)){
            auto&response=*dynamic_cast<QRpc::QRPCRequestJobResponse*>(objectResponse);
            p.response_header.setRawHeader(response.responseHeader);
            p.response_status_code=response.response_status_code;
            p.response_qt_status_code=response.response_qt_status_code;
            p.response_reason_phrase=response.response_status_reason_phrase;
            p.response_body=response.response_body;
        }
    }
    return*this;
}

QString QRPCHttpResponse::toString() const
{
    dPvt();\
    auto&response=*this;
    auto qt_text=QRPCListenRequestCode::qt_network_error_phrase(p.response_qt_status_code);
    auto msg=qsl("QtStatus: Status:%1, %2, %3").arg(QString::number(response.qtStatusCode()), response.reasonPhrase(),qt_text);
    return msg;
}

QRPCHttpResponse::operator bool() const
{
    dPvt();
    if(p.response_qt_status_code==QNetworkReply::NoError)
        return true;
    else if(p.response_status_code==200)
        return true;
    else
        return false;
}

QRPCHttpResponse&QRPCHttpResponse::print(const QString &output)
{
    for(auto&v:this->printOut(output))
        sInfo()<<v;
    return*this;
}

QStringList QRPCHttpResponse::printOut(const QString &output)
{
    auto space=output.trimmed().isEmpty()?qsl_null:qsl("    ");
    VariantUtil vu;
    auto out=this->header().printOut(output);
    out<<qsl("%1%2.     %3").arg(space, output, this->toString().trimmed());
    return out;
}

}
