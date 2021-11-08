#include "./p_qrpc_http_headers.h"
#include "./qstm_util_variant.h"
#include "./p_qrpc_util.h"
#include "../qrpc_macro.h"
#include <QJsonDocument>

namespace QRpc {

static const auto staticDefaultContentType=QRpc::AppJson;

#define dPvt()\
    auto&p =*reinterpret_cast<QRPCHttpHeadersPvt*>(this->p)

class QRPCHttpHeadersPvt{
public:

    QObject*parent=nullptr;
    QVariantHash  header;

    explicit QRPCHttpHeadersPvt(QRPCHttpHeaders*parent){
        this->parent=parent;
    }

    virtual ~QRPCHttpHeadersPvt(){
    }

    QVariant header_v(const QString&key)const{
        auto vkey=key.trimmed().toLower();
        QHashIterator<QString, QVariant> i(this->header);
        QStringList vList;
        while (i.hasNext()) {
            i.next();
            if(i.key().trimmed().toLower()==vkey){
                auto&v=i.value();
                if(v.typeId()==QMetaType::QStringList){
                    vList=v.toStringList();
                }
                else if(v.typeId()==QMetaType::QVariantList){
                    for(auto&v:v.toList()){
                        vList<<v.toString();
                    }
                }
                else{
                    vList<<v.toString();
                }
            }
        }
        return vList.isEmpty()?QVariant():vList;
    }
};

QRPCHttpHeaders::QRPCHttpHeaders(QObject *parent):QObject(parent){
    this->p = new QRPCHttpHeadersPvt(this);
    dPvt();
    p.parent=parent;
}

QRPCHttpHeaders::QRPCHttpHeaders(const QVariant &v, QObject *parent):QObject(parent)
{
    this->p = new QRPCHttpHeadersPvt(this);
    dPvt();
    p.parent=parent;
    VariantUtil vu;
    p.header=vu.toHash(v);
}

QRPCHttpHeaders::~QRPCHttpHeaders(){
    dPvt();
    delete&p;
}

QRPCHttpHeaders &QRPCHttpHeaders::clear()
{
    dPvt();
    p.header.clear();
    return*this;
}

QVariantHash&QRPCHttpHeaders::rawHeader()const
{
    dPvt();
    return p.header;
}

QVariant QRPCHttpHeaders::rawHeader(const QString &headername)const
{
    dPvt();
    QStringList returnList;
    QHashIterator<QString, QVariant> i(p.header);
    while (i.hasNext()) {
        i.next();
        if(i.key().toLower()==headername.toLower()){
            auto&v=i.value();
            QStringList vList;
            if(v.typeId()==QMetaType::QVariantList || v.typeId()==QMetaType::QStringList){
                vList=v.toStringList();
            }
            else{
                vList=v.toString().split(qsl(";"));
            }
            for(auto&header:vList)
                returnList<<header;
        }
    }
    return QVariant(returnList);
}

QRPCHttpHeaders &QRPCHttpHeaders::setRawHeader(const QVariantHash &rawHeader)
{
    dPvt();
    auto&header=p.header;
    header.clear();
    QHashIterator<QString, QVariant> i(rawHeader);
    while (i.hasNext()) {
        i.next();
        const auto key=QRpc::Util::headerFormatName(i.key());
        const auto value=i.value();
        this->setRawHeader(key,value);
    }
    return*this;
}

QRPCHttpHeaders &QRPCHttpHeaders::setRawHeader(const QString &header, const QVariant &value)
{
    QVariantList list;
    if(value.typeId()==QMetaType::QStringList){
        for(auto&v:value.toStringList())
            list<<v;
    }
    else if(value.typeId()==QMetaType::QVariantList){
        for(auto&v:value.toList())
            list<<v;
    }
    else{
        list<<value;
    }

    dPvt();
    const auto headerName=QRpc::Util::headerFormatName(header);
    auto vList=p.header[headerName].toStringList();
    vList.clear();
    for(auto&v:list){
        auto vv=v.toByteArray().trimmed();
        if(!vv.isEmpty()){
            if(!vList.contains(vv))
                vList<<vv.trimmed();
        }
    }

    for(auto&v:vList){
        auto x=v;
        if(x.startsWith(qbl_fy(Basic)) || x.startsWith(qbl_fy(basic))){
            auto x_toUtf8 = x.replace(qbl_fy(Basic), qsl_null).replace(qbl_fy(basic), qsl_null).trimmed().toUtf8();
            auto x_from=QByteArray::fromBase64(x_toUtf8);
            if(x_from.toBase64()!=x_toUtf8){
                v=qbl_fy(Basic)+qsl_space+x_toUtf8.toBase64();
            }
        }
    }

    p.header[headerName]=vList;
    return*this;
}

QRPCHttpHeaders &QRPCHttpHeaders::addRawHeader(const QVariantHash &rawHeader)
{
    VariantUtil vu;
    QHashIterator<QString, QVariant> i(rawHeader);
    while (i.hasNext()) {
        i.next();
        this->addRawHeader(i.key(), i.value());
    }
    return*this;
}

QRPCHttpHeaders &QRPCHttpHeaders::addRawHeader(const QString &header, const QVariant &value)
{
    QVariantList list;
    if(value.typeId()==QMetaType::QStringList){
        for(auto&v:value.toStringList())
            list<<v;
    }
    else if(value.typeId()==QMetaType::QVariantList){
        for(auto&v:value.toList())
            list<<v;
    }
    else{
        list<<value;
    }

    dPvt();
    const auto headerName=QRpc::Util::headerFormatName(header);
    auto vList=p.header.value(headerName).toStringList();
    vList.clear();
    for(auto&v:list){
        auto vv=v.toByteArray().trimmed();
        if(!vv.isEmpty()){
            if(!vList.contains(vv))
                vList<<vv.trimmed();
        }
    }
    this->setRawHeader(headerName, vList);
    return*this;
}

QRPCHttpHeaders &QRPCHttpHeaders::setContentType(const int contentType)
{
    QString header;
    auto content_type=ContentType(contentType);
    if(QRPCContentTypeHeaderTypeToHeader.contains(content_type)){
        header=QRPCContentTypeHeaderTypeToHeader.value(content_type);
    }
    else{
        header=QRPCContentTypeHeaderTypeToHeader.value(QRpc::AppOctetStream);
    }
    return this->setContentType(header);
}

QRPCHttpHeaders &QRPCHttpHeaders::setContentType(const QVariant &v)
{
    dPvt();
    p.header.remove(ContentTypeName);
    p.header.remove(ContentTypeName.toLower());
    QVariant value=v;
    if(v.typeId()==QMetaType::QUrl){
        value=QVariant();
        auto url=v.toUrl();
        if(url.isLocalFile()){
            auto ext=url.toLocalFile().split(qsl(".")).last().trimmed();
            if(!QRPCContentTypeExtensionToHeader.contains(ext.toLower()))
                ext.clear();
            auto type=QRPCContentTypeExtensionToHeader.value(ext.toLower());
            value=QRPCContentTypeHeaderTypeToHeader.value(type);
        }
    }
    if(value.isValid())
        p.header.insert(ContentTypeName,value);
    return*this;
}

bool QRPCHttpHeaders::isContentType(int contentType) const
{
    dPvt();
    const auto contenttype=p.header_v(ContentTypeName).toStringList();
    for(auto&ct:contenttype){
        int cType=-1;
        if(!ContentTypeHeaderToHeaderType.contains(ct))
            continue;
        else{
            cType=ContentTypeHeaderToHeaderType.value(ct);
            if(contentType==cType)
                return true;
        }
    }
    return false;
}

QVariant QRPCHttpHeaders::contentType() const
{
    dPvt();
    auto v=p.header_v(ContentTypeName);
    return v;
}

ContentType QRPCHttpHeaders::defaultContentType()
{
    return staticDefaultContentType;
}

QVariant QRPCHttpHeaders::contentDisposition() const
{
    dPvt();
    auto v=p.header_v(ContentDispositionName);
    return v;
}

QRPCHttpHeaders &QRPCHttpHeaders::setAuthorization(const QString &authorization, const QString &type, const QVariant &credentials)
{

    QString scredentials;
    if(credentials.typeId()==QMetaType::QVariantMap){
        QStringList params;
        QHashIterator<QString, QVariant> i(credentials.toHash());
        while (i.hasNext()) {
            i.next();
            params<<qsl("%1=%2").arg(i.key(),i.value().toString());
        }
        scredentials=params.join(qsl_space);
    }
    else if(credentials.typeId()==QMetaType::QVariantHash){
        QStringList params;
        QHashIterator<QString, QVariant> i(credentials.toHash());
        while (i.hasNext()) {
            i.next();
            params<<qsl("%1=%2").arg(i.key(),i.value().toString());
        }
        scredentials=params.join(' ');
    }
    else{
        scredentials=credentials.toString().trimmed();
    }

    if(!type.isEmpty() && !scredentials.isEmpty())
        this->setRawHeader(authorization.toUtf8(), qsl("%1 %2").arg(type,scredentials).toUtf8());
    return*this;
}

QRPCHttpHeaders &QRPCHttpHeaders::setAuthorization(const QString &authorization, const AuthorizationType &type, const QVariant &credentials)
{
    QString stype;
    if(type==Basic)
        stype=qsl("Basic");
    else if(type==Bearer)
        stype=qsl("Bearer");
    else if(type==Digest)
        stype=qsl("Digest");
    else if(type==HOBA)
        stype=qsl("HOBA");
    else if(type==Mutual)
        stype=qsl("Mutual");
    else if(type==AWS4_HMAC_SHA256)
        stype=qsl("AWS4_HMAC_SHA256");
    else if(type==Service)
        stype=qsl("Service");

    if(type!=Basic)
        return this->setAuthorization(authorization, stype, credentials);
    else{
        auto v=QByteArray::fromBase64(credentials.toByteArray());
        if(!v.isEmpty()){
            return this->setAuthorization(authorization, stype, credentials);
        }
        else{
            v=credentials.toByteArray().toBase64();
            return this->setAuthorization(authorization, stype, v);
        }
    }
}


QRPCHttpHeaders &QRPCHttpHeaders::setAuthorization(const AuthorizationType &type, const QVariant &credentials)
{
    return this->setAuthorization(qsl("Authorization"), type, credentials);
}

QRPCHttpHeaders &QRPCHttpHeaders::setAuthorization(const AuthorizationType &type, const QVariant &userName, const QVariant &passWord)
{
    auto credentials=qsl("%1:%2").arg(userName.toString(), passWord.toString()).toUtf8().toBase64();
    return this->setAuthorization(type, credentials);
}

QRPCHttpHeaders &QRPCHttpHeaders::setProxyAuthorization(const AuthorizationType &type, const QVariant &credentials)
{
    return this->setAuthorization(qsl("Proxy-Authorization"), type, credentials);
}

QRPCHttpHeaders &QRPCHttpHeaders::setProxyAuthorization(const QString &type, const QVariant &credentials)
{
    return this->setAuthorization(qsl("Proxy-Authorization"), type, credentials);
}

QRPCHttpHeaders &QRPCHttpHeaders::setWWWAuthenticate(const AuthorizationType &type, const QVariant &credentials)
{
    return this->setAuthorization(qsl("WWW-Authenticate"), type, credentials);
}

QRPCHttpHeaders &QRPCHttpHeaders::setWWWAuthenticate(const QString &type, const QVariant &credentials)
{
    return this->setAuthorization(qsl("WWW-Authenticate"), type, credentials);
}

QVariant QRPCHttpHeaders::cookies()const
{
    return this->rawHeader(qsl("Cookie"));
}

QRPCHttpHeaders &QRPCHttpHeaders::setCookies(const QVariant &cookie)
{
    return this->setRawHeader(qsl("Cookie"), cookie);
}

QVariant QRPCHttpHeaders::authorization(const QString &authorization, const QString &type)
{
    dPvt();
    QVariantList returnList;
    QHashIterator<QString, QVariant> i(p.header);
    while (i.hasNext()) {
        i.next();
        if(i.key().toLower()==authorization.toLower()){
            auto list=i.value().toString().split(qsl_space);
            if(list.size()<=1){
                continue;
            }
            else if(type.toLower()==list.first()){
                list.takeFirst();
                for(auto&v:list){
                    if(v.contains(qsl("="))){
                        auto sp=v.split(qsl("="));
                        QVariantHash map;
                        map.insert(sp.at(0),sp.at(1));
                        returnList<<map;
                    }
                    else if(!returnList.contains(v)){
                        returnList<<v;
                    }
                }
            }

        }
    }
    return returnList.size()==1?returnList.first():returnList;
}

QVariant QRPCHttpHeaders::authorization(const QString &authorization, const AuthorizationType &type)
{
    QString stype;
    if(type==Basic)
        stype=qsl("Basic");
    else if(type==Bearer)
        stype=qsl("Bearer");
    else if(type==Digest)
        stype=qsl("Digest");
    else if (type==HOBA)
        stype=qsl("HOBA");
    else if(type==Mutual)
        stype=qsl("Mutual");
    else if(type==AWS4_HMAC_SHA256)
        stype=qsl("AWS4_HMAC_SHA256");
    return this->authorization(authorization, stype);
}

QVariant QRPCHttpHeaders::authorization(const AuthorizationType &type)
{
    return this->authorization(qsl("Authorization"), type);
}

QVariant QRPCHttpHeaders::authorization(const QString &type)
{
    return this->authorization(qsl("Authorization"), type);
}

QVariant QRPCHttpHeaders::proxyAuthorization(const AuthorizationType &type)
{
    return this->authorization(qsl("Proxy-Authorization"), type);
}

QVariant QRPCHttpHeaders::proxyAuthorization(const QString &type)
{
    return this->authorization(qsl("Proxy-Authorization"), type);
}

QVariant QRPCHttpHeaders::wwwAuthenticate(const AuthorizationType &type)
{
    return this->authorization(qsl("WWW-Authorization"), type);
}

QVariant QRPCHttpHeaders::wwwAuthenticate(const QString &type)
{
    return this->authorization(qsl("WWW-Authorization"), type);
}

QRPCHttpHeaders &QRPCHttpHeaders::print(const QString &output)
{
    for(auto&v:this->printOut(output))
        sInfo()<<v;
    return*this;
}

QStringList QRPCHttpHeaders::printOut(const QString &output)
{
    auto space=output.trimmed().isEmpty()?qsl_null:qsl("    ");
    QStringList __return;
    VariantUtil vu;
    auto vMap=this->rawHeader();
    if(!vMap.isEmpty()){
        __return<<qsl("%1%2 headers").arg(space, output).trimmed();
        QHashIterator<QString, QVariant> i(vMap);
        while (i.hasNext()){
            i.next();
            __return<<qsl("%1     %2:%3").arg(space, i.key(), vu.toStr(i.value()));
        }
    }
    return __return;
}

QRPCHttpHeaders &QRPCHttpHeaders::operator=(const QVariant &v)
{
    dPvt();
    p.header.clear();
    QVariantHash vMap;
    if(v.typeId()==QMetaType::QString || v.typeId()==QMetaType::QByteArray)
        vMap=QJsonDocument::fromJson(v.toByteArray()).toVariant().toHash();
    else
        vMap=v.toHash();
    return this->setRawHeader(vMap);
}

QRPCHttpHeaders &QRPCHttpHeaders::operator<<(const QVariant &v)
{
    QVariantHash vMap;
    if(v.typeId()==QMetaType::QString || v.typeId()==QMetaType::QByteArray)
        vMap=QJsonDocument::fromJson(v.toByteArray()).toVariant().toHash();
    else
        vMap=v.toHash();
    QHashIterator<QString, QVariant> i(vMap);
    while (i.hasNext()) {
        i.next();
        this->addRawHeader(i.key(), i.value());
    }
    return*this;
}

}
