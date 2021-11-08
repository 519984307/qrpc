#include "./qrpc_listen_request.h"
#include "./qrpc_request.h"
#include "./qrpc_macro.h"

#include <QVariantHash>

//#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
//#define CBOR_INCLUDED
//#endif

#include <QtCborCommon>
#include <QCborStreamReader>
#include <QCborStreamWriter>
#include <QCborArray>
#include <QCborMap>
#include <QCborValue>
#include <QCborParserError>

#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QDateTime>
#include <QCoreApplication>
#include <QProcessEnvironment>
#include <QCryptographicHash>
#include <QEventLoop>
#include <QMetaProperty>
#include <QTimer>

namespace QRpc {

static const QByteArray makeBaseUuid(){
    QStringList lst;
#ifdef Q_OS_IOS
    lst << QString::number(qApp->applicationPid());
    lst << qApp->applicationName();
#else
    QProcess process;
    lst = process.environment();
#endif
    auto bytes=lst.join(qsl(",")).toUtf8()+QDateTime::currentDateTime().toString().toUtf8();
    return QCryptographicHash::hash(bytes, QCryptographicHash::Md5).toHex();
}

static auto const baseUuid=makeBaseUuid();

#define dPvt()\
    auto&p =*reinterpret_cast<QRPCListenRequestPvt*>(this->p)

class QRPCListenRequestPvt:public QObject{
public:
    QHash<QString, QFile *> uploadedFiles;
    QEventLoop eventLoop;
    QVariantHash map;
    QUuid _listenUuid;
    ContentType _requestContentType=QRpc::AppNone;
    QUuid _requestUuid;
    QByteArray _requestProtocol;
    QByteArray _requestPath;
    QByteArray _requestMethod;
    QVariantHash _requestBroker;
    QVariantHash _requestHeader;
    QVariantHash _requestParameter;
    QVariant _requestBody;
    QVariantList _requestParserProperty;
    QVariantHash _requestBodyMap;
    QVariantHash _responseHeader;
    QVariantHash _responseCallback;
    QVariant _responseBody;
    QByteArray _responsePhrase;
    QVariantHash _requestParamCache;
    QRPCListenRequestCode listenCode;
    int _responseCode=0;
    int _requestTimeout=0;
    void*_data=nullptr;
    QRPCListenRequest*parent=nullptr;
    explicit QRPCListenRequestPvt(QRPCListenRequest*parent):QObject(parent), listenCode(parent){
        this->parent=parent;
        QObject::connect(parent, &QRPCListenRequest::finish, this, &QRPCListenRequestPvt::onRequestFinish);
    }
    virtual ~QRPCListenRequestPvt(){
        this->freeFiles();
    }

    QVariantHash&requestParamCache(){
        if(this->_requestParamCache.isEmpty()){
            if(this->parent->isMethodPost() || this->parent->isMethodPut())
                this->_requestParamCache=this->parent->requestBodyHash();
            else
                this->_requestParamCache=this->parent->requestParameter();
        }
        return this->_requestParamCache;
    }



    template<class T>
    T toVMap() const{
        T map;

        const QMetaObject* metaObject = parent->metaObject();
        for(int i = metaObject->propertyOffset() ; i < metaObject->propertyCount() ; i++){
            auto property = metaObject->property(i);
            auto key=property.name();
            auto value=property.read(parent);
            if(value.isValid()){
                if(value.typeId()==QMetaType::QUuid && (!value.toUuid().isNull()))
                    map.insert(key, value);
                else
                    map.insert(key, value);
            }
        }
        return map;
    }

    void freeFiles(){
        for(auto&v:this->uploadedFiles){
            v->close();
            delete v;
        }
        this->uploadedFiles.clear();
    }

    void makeUuid(){
        if(this->_requestUuid.isNull()){
            auto uuidNs = QUuid::createUuid();
            this->_requestUuid=QUuid::createUuidV5(uuidNs, baseUuid);
        }
    }

    void clear(){
        this->eventLoop.quit();
        this->_listenUuid=QUuid();
        this->_data=nullptr;
        this->parent->vu.clear();
        this->_requestParamCache.clear();
        this->clearRequest();
        this->clearResponse();
    }

    void clearRequest(){
        this->_requestUuid=QUuid();
        this->_requestHeader=QVariantHash();
        this->_requestParameter=QVariantHash();
        this->_requestBody=QVariant();
    }

    void clearResponse(){
        this->_responseHeader=QVariantHash();
        this->_responseBody=QVariant();
        this->_responseCode=0;
        this->_responsePhrase=QByteArray();
        this->_data=nullptr;
    }

    void setRequestHeader(const QVariantHash &value){
        this->_requestHeader=value;
        this->_requestContentType=QRpc::AppXwwwForm;
        static auto headerName=ContentTypeName.toLower();
        QHashIterator<QString, QVariant> i(value);
        while (i.hasNext()) {
            i.next();
            auto key=i.key().toLower();
            auto value=i.value().toString().toLower().trimmed();
            if(key.toLower()==headerName){
                QHashIterator<QString, ContentType> i(ContentTypeHeaderToHeaderType);
                while (i.hasNext()) {
                    i.next();
                    if(value.contains(i.key())){
                        this->_requestContentType=i.value();
                        break;
                    }
                }
            }
        }
    }

    QByteArray authorizationParser(const QByteArray&type) const
    {
        auto&p=*this;
        const auto stype=type.toLower();
        //TODO ADAPTER FOR MULT AUTHORIZATION
        QHashIterator<QString, QVariant> i(p._requestHeader);
        while (i.hasNext()) {
            i.next();
            auto k=i.key().trimmed().toLower();
            auto v=i.value().toString()
                         .replace(qsl("Basic"),qsl("basic"))
                         .replace(qsl("Bearer"),qsl("bearer"))
                         .replace(qsl("Service"),qsl("service"));
#if Q_RPC_LOG_SUPER_VERBOSE
            sWarning()<<qsl("%1: %2").arg(QString(k)).arg(QString(v));
#endif
            if(k==qsl("authorization")){
                if(v.startsWith(stype)){
                    auto authList=v.split(stype);
                    auto value=authList.last().trimmed();
#if Q_RPC_LOG_SUPER_VERBOSE
                    sWarning()<<type<<":1: "<<authList;
                    sWarning()<<type<<":2: "<<value;
#endif
                    return value.toUtf8();
                }
            }
        }
        return QByteArray();
    }

public slots:
    void onRequestFinish(){
        this->eventLoop.quit();
    }
};

QRPCListenRequest::QRPCListenRequest(QObject *parent):QObject(parent)
{
    this->p = new QRPCListenRequestPvt(this);
}

QRPCListenRequest::QRPCListenRequest(const QVariant &requestBody, QObject *parent):QObject(parent)
{
    this->p = new QRPCListenRequestPvt(this);
    VariantUtil vu;
    this->mergeMap(vu.toMap(requestBody));
}

QRPCListenRequest::QRPCListenRequest(const QVariant &requestBody, const ControllerSetting &setting, QObject *parent):QObject(parent)
{
    this->p = new QRPCListenRequestPvt(this);
    if(setting.isValid())
        this->setControllerSetting(setting);
    VariantUtil vu;
    this->mergeMap(vu.toMap(requestBody));
}

QRPCListenRequest::~QRPCListenRequest()
{
    dPvt();delete&p;
}

bool QRPCListenRequest::isMethodHead() const
{
    dPvt();
    return (p._requestMethod==qbl("head"));
}

bool QRPCListenRequest::isMethodGet()const
{
    dPvt();
    return (p._requestMethod==qbl("get"));
}

bool QRPCListenRequest::isMethodPost()const
{
    dPvt();
    return (p._requestMethod==qbl("post"));
}

bool QRPCListenRequest::isMethodPut() const
{
    dPvt();
    return (p._requestMethod==qbl("put"));
}

bool QRPCListenRequest::isMethodDelete() const
{
    dPvt();
    return (p._requestMethod==qbl("delete"));
}

bool QRPCListenRequest::isMethodUpload() const
{
    dPvt();
    return !p.uploadedFiles.isEmpty();
}

bool QRPCListenRequest::isMethodOptions() const
{
    dPvt();
    return (p._requestMethod==qbl("options"));
}

bool QRPCListenRequest::canMethodHead()
{
    if(!this->isMethodHead())
        return this->co().setBadGateway().isOK();
    else
        return true;
}

bool QRPCListenRequest::canMethodGet()
{
    if(!this->isMethodGet())
        return this->co().setBadGateway().isOK();
    else
        return true;
}

bool QRPCListenRequest::canMethodPost()
{
    if(!this->isMethodPost())
        return this->co().setBadGateway().isOK();
    else
        return true;
}

bool QRPCListenRequest::canMethodPut()
{
    if(!this->isMethodPut())
        return this->co().setBadGateway().isOK();
    else
        return true;
}

bool QRPCListenRequest::canMethodDelete()
{
    if(!this->isMethodDelete())
        return this->co().setBadGateway().isOK();
    else
        return true;
}

bool QRPCListenRequest::canMethodUpload()
{
    if(!this->isMethodUpload())
        return this->co().setBadGateway().isOK();
    else
        return true;
}

bool QRPCListenRequest::canMethodOptions()
{
    if(!this->isMethodOptions())
        return this->co().setBadGateway().isOK();
    else
        return true;
}

void QRPCListenRequest::start()
{
    dPvt();
    auto onTimeOut=[this, &p]{
        this->co().setRequestTimeout();
        p.eventLoop.quit();
    };
    if(this->requestTimeout()>0){
        QTimer::singleShot(this->requestTimeout(), this, onTimeOut);
    }
    p.eventLoop.exec();
}

void *QRPCListenRequest::data() const
{
    dPvt();
    return p._data;
}

void QRPCListenRequest::setData(void *data)
{
    dPvt();
    p._data = data;
}

QRPCListenRequestCode &QRPCListenRequest::codeOption()
{
    dPvt();
    return p.listenCode;
}

QRPCListenRequestCode &QRPCListenRequest::co()
{
    dPvt();
    return p.listenCode;
}

QRPCListenRequestCode &QRPCListenRequest::co(const QVariant &v)
{
    dPvt();
    p.listenCode=v;
    return p.listenCode;
}

QByteArray QRPCListenRequest::hash() const
{
    dPvt();
    auto bytes=p._requestPath;
    return QCryptographicHash::hash(bytes, QCryptographicHash::Md5).toHex();
}

bool QRPCListenRequest::isValid() const
{
    dPvt();
    return !p._requestUuid.isNull();
}

bool QRPCListenRequest::isEmpty() const
{
    dPvt();
    if(p._requestPath.trimmed().isEmpty())
        return false;
    else if(p._requestMethod.trimmed().isEmpty())
        return false;
    else
        return true;
}

void QRPCListenRequest::clear()
{
    dPvt();
    p.clear();
}

bool QRPCListenRequest::fromRequest(const QRPCListenRequest &request)
{
    if(!this->fromHash(request.toHash()))
        return false;
    else{
        auto v=request.data();
        this->setData(v);
        return true;
    }
}

bool QRPCListenRequest::fromMap(const QVariantMap &vRequest)
{
    dPvt();
    p.clear();
    return this->mergeMap(vRequest);
}

bool QRPCListenRequest::mergeMap(const QVariantMap &vRequest)
{
    auto&metaObject =*this->metaObject();
    for(auto i = metaObject.propertyOffset() ; i < metaObject.propertyCount() ; i++){
        auto property = metaObject.property(i);
        auto key = property.name();
        auto vVal= property.read(this);
        auto vNew=vRequest.value(key);
        auto type=property.typeId();
        if(!vNew.isValid() || vNew.isNull()){
            continue;
        }
        else if(type==QMetaType::QVariantMap || type==QMetaType::QVariantHash){
            auto vMap=vVal.toHash();
            if(!vMap.isEmpty()){
                auto mNew=vNew.toHash();
                QHashIterator<QString, QVariant> it(mNew);
                while (it.hasNext()) {
                    it.next();
                    vMap.insert(it.key(), it.value());
                }
                vNew=vMap;
            }
            if(!property.write(this, vNew)){
                this->clear();
                break;
            }
        }
        else if(type==QMetaType::QVariantList || type==QMetaType::QStringList){
            auto vLst=vVal.toList();
            if(!vLst.isEmpty()){
                auto mNew=vNew.toList();
                for(auto&v:mNew){
                    if(vLst.contains(v))
                        continue;
                    else
                        vLst<<v;
                }
                vNew=vLst;
            }
            if(!property.write(this, vNew)){
                this->clear();
                break;
            }
        }
        else if(type==QMetaType::QUuid){
            if(!property.write(this, vNew.toUuid())){
                this->clear();
                break;
            }
        }
        else if(type==QMetaType::QUrl){
            if(!property.write(this, vNew.toUrl())){
                this->clear();
                break;
            }
        }
        else if(type==QMetaType::UInt || type==QMetaType::Int){
            if(!property.write(this, vNew.toInt())){
                this->clear();
                break;
            }
        }
        else if(type==QMetaType::ULongLong || type==QMetaType::LongLong){
            if(!property.write(this, vNew.toLongLong())){
                this->clear();
                break;
            }
        }
        else if(type==QMetaType::Double){
            if(!property.write(this, vNew.toDouble())){
                this->clear();
                break;
            }
        }
        else if(!property.write(this, vNew)){
            if(type!=QMetaType::User){
                this->clear();
                break;
            }
            else if(!property.write(this, vNew.toInt())){
                this->clear();
                break;
            }
        }

    }
    return this->isValid();
}

bool QRPCListenRequest::fromHash(const QVariantHash &vRequest)
{
    dPvt();
    p.clear();
    return this->mergeHash(vRequest);
}

bool QRPCListenRequest::mergeHash(const QVariantHash &vRequest)
{
    auto&metaObject =*this->metaObject();
    for(auto i = metaObject.propertyOffset() ; i < metaObject.propertyCount() ; i++){
        auto property = metaObject.property(i);
        auto key = property.name();
        auto vVal= property.read(this);
        auto vNew=vRequest.value(key);
        auto type=property.typeId();
        if(!vNew.isValid() || vNew.isNull()){
            continue;
        }
        else if(type==QMetaType::QVariantMap || type==QMetaType::QVariantHash){
            auto vMap=vVal.toHash();
            if(!vMap.isEmpty()){
                auto mNew=vNew.toHash();
                QHashIterator<QString, QVariant> it(mNew);
                while (it.hasNext()) {
                    it.next();
                    vMap.insert(it.key(), it.value());
                }
                vNew=vMap;
            }
            if(!property.write(this, vNew)){
                this->clear();
                break;
            }
        }
        else if(type==QMetaType::QVariantList || type==QMetaType::QStringList){
            auto vLst=vVal.toList();
            if(!vLst.isEmpty()){
                auto mNew=vNew.toList();
                for(auto&v:mNew){
                    if(vLst.contains(v))
                        continue;
                    else
                        vLst<<v;
                }
                vNew=vLst;
            }
            if(!property.write(this, vNew)){
                this->clear();
                break;
            }
        }
        else if(type==QMetaType::QUuid){
            if(!property.write(this, vNew.toUuid())){
                this->clear();
                break;
            }
        }
        else if(type==QMetaType::QUrl){
            if(!property.write(this, vNew.toUrl())){
                this->clear();
                break;
            }
        }
        else if(type==QMetaType::UInt || type==QMetaType::Int){
            if(!property.write(this, vNew.toInt())){
                this->clear();
                break;
            }
        }
        else if(type==QMetaType::ULongLong || type==QMetaType::LongLong){
            if(!property.write(this, vNew.toLongLong())){
                this->clear();
                break;
            }
        }
        else if(type==QMetaType::Double){
            if(!property.write(this, vNew.toDouble())){
                this->clear();
                break;
            }
        }
        else if(!property.write(this, vNew)){
            if(type!=QMetaType::User){
                this->clear();
                break;
            }
            else if(!property.write(this, vNew.toInt())){
                this->clear();
                break;
            }
        }

    }
    return this->isValid();
}

bool QRPCListenRequest::fromResponseMap(const QVariantHash &vRequest)
{
    dPvt();
    p.clearResponse();
    const QMetaObject* metaObject = this->metaObject();
    for(int i = metaObject->propertyOffset() ; i < metaObject->propertyCount() ; i++){
        auto property = metaObject->property(i);
        auto key = QByteArray(property.name());
        if(key.startsWith(qbl("requestUuid")) || key.startsWith(qbl("response"))){
            auto value=vRequest.value(key);
            if(!property.write(this, value)){
                auto t=property.typeId();
                if(t==QMetaType::QByteArray)
                    value=value.toByteArray();
                else if(t==QMetaType::QString)
                    value=value.toString();
                else if(t==QMetaType::QChar)
                    value=value.toChar();
                else if(t==QMetaType::QBitArray)
                    value=value.toByteArray();
                else if(t==QMetaType::LongLong || t==QMetaType::ULongLong)
                    value=value.toLongLong();
                else if(t==QMetaType::Int || t==QMetaType::UInt)
                    value=value.toInt();
                else if(t==QMetaType::Double)
                    value=value.toDouble();
                else if(t==QMetaType::Bool)
                    value=value.toBool();
                else if(t==QMetaType::QDateTime)
                    value=value.toDateTime();
                else if(t==QMetaType::QDate)
                    value=value.toDate();
                else if(t==QMetaType::QTime)
                    value=value.toTime();
                else if(t==QMetaType::QVariantMap || t==QMetaType::QVariantHash)
                    value=value.toHash();
                else if(t==QMetaType::QVariantList)
                    value=value.toList();
                else if(t==QMetaType::QStringList)
                    value=value.toList();
                else if(t==QMetaType::QUuid)
                    value=value.toUuid();
                else if(t==QMetaType::QUrl)
                    value=value.toUrl();
            }
        }
    }
    return this->isValid();
}

bool QRPCListenRequest::fromResponseHash(const QVariantHash &vRequest)
{
    dPvt();
    p.clearResponse();
    const QMetaObject* metaObject = this->metaObject();
    for(int i = metaObject->propertyOffset() ; i < metaObject->propertyCount() ; i++){
        auto property = metaObject->property(i);
        auto key = QByteArray(property.name());
        if(key.startsWith(qbl("requestUuid")) || key.startsWith(qbl("response"))){
            auto value=vRequest.value(key);
            if(!property.write(this, value)){
                auto t=property.typeId();
                if(t==QMetaType::QByteArray)
                    value=value.toByteArray();
                else if(t==QMetaType::QString)
                    value=value.toString();
                else if(t==QMetaType::QChar)
                    value=value.toChar();
                else if(t==QMetaType::QBitArray)
                    value=value.toByteArray();
                else if(t==QMetaType::LongLong || t==QMetaType::ULongLong)
                    value=value.toLongLong();
                else if(t==QMetaType::Int || t==QMetaType::UInt)
                    value=value.toInt();
                else if(t==QMetaType::Double)
                    value=value.toDouble();
                else if(t==QMetaType::Bool)
                    value=value.toBool();
                else if(t==QMetaType::QDateTime)
                    value=value.toDateTime();
                else if(t==QMetaType::QDate)
                    value=value.toDate();
                else if(t==QMetaType::QTime)
                    value=value.toTime();
                else if(t==QMetaType::QVariantMap || t==QMetaType::QVariantHash)
                    value=value.toHash();
                else if(t==QMetaType::QVariantList)
                    value=value.toList();
                else if(t==QMetaType::QStringList)
                    value=value.toList();
                else if(t==QMetaType::QUuid)
                    value=value.toUuid();
                else if(t==QMetaType::QUrl)
                    value=value.toUrl();
            }
        }
    }
    return this->isValid();
}

bool QRPCListenRequest::fromJson(const QByteArray &bytes)
{
    QJsonParseError*parser=nullptr;
    auto v=QJsonDocument::fromJson(bytes, parser).object().toVariantHash();
    return this->fromHash(v);
}

QByteArray QRPCListenRequest::toJson() const
{
    auto vMap=this->toHash();
    auto bytes=QJsonDocument::fromVariant(vMap).toJson(QJsonDocument::Compact);
    return bytes;
}

QVariantMap QRPCListenRequest::toMap() const
{
    dPvt();
    return p.toVMap<QVariantMap>();
}

QVariantHash QRPCListenRequest::toHash() const
{
    dPvt();
    return p.toVMap<QVariantHash>();
}

void QRPCListenRequest::requestStart()
{
    dPvt();
    emit
        QTimer::singleShot(p._requestTimeout, &p.eventLoop, &QEventLoop::quit);
    p.eventLoop.exec();
}

QUuid &QRPCListenRequest::listenUuid() const
{
    dPvt();
    return p._listenUuid;
}

void QRPCListenRequest::setListenUuid(const QUuid &value)
{
    dPvt();
    p._listenUuid=value;
}

QUuid &QRPCListenRequest::requestUuid() const
{
    dPvt();
    return p._requestUuid;
}

void QRPCListenRequest::setRequestUuid(const QUuid &value)
{
    dPvt();
    p._requestUuid = value;
}

QByteArray &QRPCListenRequest::requestProtocol() const
{
    dPvt();
    return p._requestProtocol;
}

void QRPCListenRequest::setRequestProtocol(const QVariant &value)
{
    dPvt();
    auto ivalue=value.toLongLong();
    if(ivalue>0){
        auto name=QRPCProtocolUrlName.value(QRPCProtocol(ivalue)).toUtf8();
        if(name.isEmpty())
            p._requestProtocol = value.toString().toUtf8();
        else
            p._requestProtocol = name;
    }
}

QByteArray &QRPCListenRequest::requestPath() const
{
    dPvt();
    return p._requestPath;
}

void QRpc::QRPCListenRequest::setRequestPath(const QVariant &value)
{
    dPvt();
    p._requestPath=value.toByteArray();
}

QByteArray &QRPCListenRequest::requestMethod() const
{
    dPvt();
    return p._requestMethod;
}

void QRPCListenRequest::setRequestMethod(const QVariant &value)
{
    dPvt();
    auto ivalue=value.toLongLong();
    if(ivalue>0){
        p._requestMethod=QRPCRequestMethodName.value(ivalue).toUtf8();
    }
    else{
        p._requestMethod=value.toByteArray().toLower();
    }
}

QVariantHash &QRPCListenRequest::requestHeader() const
{
    dPvt();
    return p._requestHeader;
}

void QRPCListenRequest::setRequestHeader(const QVariantHash &value)
{
    dPvt();
    p.setRequestHeader(value);
}

QVariantHash &QRPCListenRequest::requestParameter() const
{
    dPvt();
    return p._requestParameter;
}

void QRPCListenRequest::setRequestParameter(const QVariantHash &value)
{
    dPvt();
    p._requestParamCache.clear();
    p._requestParameter=value;
}

QVariant &QRPCListenRequest::requestBody() const
{
    dPvt();
    return p._requestBody;
}

QVariantMap QRPCListenRequest::requestBodyMap() const
{
    dPvt();
    if(p._requestBody.typeId()==QMetaType::QVariantMap || p._requestBody.typeId()==QMetaType::QVariantHash)
        return p._requestBody.toMap();
    else if(this->isMethodGet() || this->isMethodDelete())
        return QVariant(p._requestParameter).toMap();
    else
        return QVariantMap();
}

QVariantHash QRPCListenRequest::requestBodyHash() const
{
    dPvt();
    if(p._requestBody.typeId()==QMetaType::QVariantMap || p._requestBody.typeId()==QMetaType::QVariantHash)
        return p._requestBody.toHash();
    else if(this->isMethodGet() || this->isMethodDelete())
        return p._requestParameter;
    else
        return {};
}

bool QRPCListenRequest::requestParserBodyMap()
{
    dPvt();
    QVariantMap body;
    if(!this->requestParserBodyMap(p._requestParserProperty, body))
        return this->co().setBadRequest().isOK();
    else
        return true;
}

bool QRPCListenRequest::requestParserBodyMap(const QVariant &property)
{
    dPvt();
    if(property.isValid()){
        p._requestParserProperty.clear();
        if(property.typeId()==QMetaType::QVariantList){
            for(auto&v:property.toList()){
                auto s=v.toString().toLower().trimmed();
                if(!p._requestParserProperty.contains(s))
                    p._requestParserProperty<<s;
            }
        }
        else if(property.typeId()==QMetaType::QStringList){
            for(auto&v:property.toStringList()){
                auto s=v.toLower().trimmed();
                if(!p._requestParserProperty.contains(s))
                    p._requestParserProperty<<s;
            }
        }
        else if(property.typeId()==QMetaType::QVariantMap || property.typeId()==QMetaType::QVariantHash){
            auto vKeys=property.toHash().keys();
            for(auto&v:vKeys){
                auto s=v.toLower().trimmed();
                if(!p._requestParserProperty.contains(s))
                    p._requestParserProperty<<s;
            }
        }
    }
    static QVariantMap body;
    return this->requestParserBodyMap(property, body);
}

bool QRPCListenRequest::requestParserBodyMap(const QVariant &property, QVariantMap &body) const
{
    dPvt();
    bool __return=true;
    if(property.isValid()){
        p._requestParserProperty.clear();
        if(property.typeId()==QMetaType::QVariantList){
            for(auto&v:property.toList()){
                auto s=v.toString().toLower().trimmed();
                if(!p._requestParserProperty.contains(s))
                    p._requestParserProperty<<s;
            }
        }
        else if(property.typeId()==QMetaType::QStringList){
            for(auto&v:property.toStringList()){
                auto s=v.toLower().trimmed();
                if(!p._requestParserProperty.contains(s))
                    p._requestParserProperty<<s;
            }
        }
        else if(property.typeId()==QMetaType::QVariantMap || property.typeId()==QMetaType::QVariantHash){
            auto vKeys=property.toHash().keys();
            for(auto&v:vKeys){
                auto s=v.toLower().trimmed();
                if(!p._requestParserProperty.contains(s))
                    p._requestParserProperty<<s;
            }
        }

        if(!p._requestParserProperty.isEmpty()){
            auto vBody=p.requestParamCache();
            QVariantList vPropBody;
            auto vKeys=vBody.keys();
            for(auto&v:vKeys){
                vPropBody<<v.toLower().trimmed();
            }
            for(auto&prop:p._requestParserProperty){
                if(!vPropBody.contains(prop)){
                    sInfo()<<qsl("parameter not found: ")+prop.toString();
                    __return=false;
                }
            }
            body=this->requestBodyMap();
        }
    }
    return __return;
}

bool QRPCListenRequest::requestParserBodyHash(const QVariant &property, QVariantHash &body) const
{
    dPvt();
    bool __return=true;
    if(property.isValid()){
        p._requestParserProperty.clear();
        if(property.typeId()==QMetaType::QVariantList){
            for(auto&v:property.toList()){
                auto s=v.toString().toLower().trimmed();
                if(!p._requestParserProperty.contains(s))
                    p._requestParserProperty<<s;
            }
        }
        else if(property.typeId()==QMetaType::QStringList){
            for(auto&v:property.toStringList()){
                auto s=v.toLower().trimmed();
                if(!p._requestParserProperty.contains(s))
                    p._requestParserProperty<<s;
            }
        }
        else if(property.typeId()==QMetaType::QVariantMap || property.typeId()==QMetaType::QVariantHash){
            auto vKeys=property.toHash().keys();
            for(auto&v:vKeys){
                auto s=v.toLower().trimmed();
                if(!p._requestParserProperty.contains(s))
                    p._requestParserProperty<<s;
            }
        }

        if(!p._requestParserProperty.isEmpty()){
            auto vBody=this->requestParamMap();
            QVariantList vPropBody;
            auto vKeys=vBody.keys();
            for(auto&v:vKeys){
                vPropBody<<v.toLower().trimmed();
            }
            for(auto&prop:p._requestParserProperty){
                if(!vPropBody.contains(prop)){
                    sInfo()<<qsl("parameter not found: ")+prop.toString();
                    __return=false;
                }
            }
            body=this->requestBodyHash();
        }
    }
    return __return;
}

QVariantList &QRPCListenRequest::requestParserProperty()
{
    dPvt();
    return p._requestParserProperty;
}

QVariant QRPCListenRequest::requestParamMap(const QByteArray &key) const
{
    return this->requestParamHash(key);
}

QVariantMap QRPCListenRequest::requestParamMap()const
{
    if(this->isMethodPost() || this->isMethodPut())
        return this->requestBodyMap();
    else
        return QVariant(this->requestParameter()).toMap();
}

QVariant QRPCListenRequest::requestParamHash(const QByteArray &key) const
{
    dPvt();
    const auto&map=p.requestParamCache();
    auto akey=key.trimmed().toLower();
    QHashIterator<QString, QVariant> i(map);
    while (i.hasNext()) {
        i.next();
        auto bKey=i.key().trimmed().toLower().toUtf8();
        if(akey==bKey){
            auto&v=i.value();
            if(v.isValid() && (v.typeId()==QMetaType::QString || v.typeId()==QMetaType::QByteArray ||  v.typeId()==QMetaType::QChar ||  v.typeId()==QMetaType::QBitArray)){
                auto s=v.toByteArray();
                return(s.isEmpty())?QVariant():s;
            }
            return v;
        }
    }

    return {};
}

QVariantHash QRPCListenRequest::requestParamHash() const
{
    dPvt();
    return p.requestParamCache();
}

QVariantList QRPCListenRequest::requestBodyList() const
{
    dPvt();
    if(p._requestBody.typeId()==QMetaType::QVariantList || p._requestBody.typeId()==QMetaType::QStringList)
        return p._requestBody.toList();

    if(this->isMethodGet() || this->isMethodDelete())
        return QVariantList{p._requestParameter};

    return {};
}

void QRPCListenRequest::setRequestBody(const QVariant &value)
{
    dPvt();
    QVariant _body;
    if(value.typeId()==QMetaType::QVariantList || value.typeId()==QMetaType::QStringList)
        _body=value;
    else if(value.typeId()==QMetaType::QVariantMap|| value.typeId()==QMetaType::QVariantHash)
        _body=value.toHash();
    else if(p._requestContentType==AppJson)
        _body=QJsonDocument::fromJson(value.toByteArray()).toVariant();
    else if(p._requestContentType==AppCBOR || p._requestContentType==AppOctetStream)
        _body=QCborValue::fromVariant(value).toVariant();
    else{
        _body=QVariant();
        if(!_body.isValid())
            _body=QJsonDocument::fromJson(value.toByteArray()).toVariant();
        if(!_body.isValid())
            _body=QCborValue::fromVariant(value).toVariant();
        if(!_body.isValid())
            _body=value;
    }
    p._requestParamCache.clear();
    p._requestBody=_body;
}

int QRPCListenRequest::requestTimeout() const
{
    dPvt();
    return p._requestTimeout;
}

void QRPCListenRequest::setRequestTimeout(int value)
{
    dPvt();
    p._requestTimeout = value;
}

QString QRPCListenRequest::uploadedFile() const
{
    dPvt();
    auto files=p.uploadedFiles.keys();
    return (files.empty())?qsl_null:files.first();
}

QHash<QString, QFile *> &QRPCListenRequest::uploadedFiles() const
{
    dPvt();
    return p.uploadedFiles;
}

void QRPCListenRequest::setUploadedFiles(const QVariant &vFiles)
{
    dPvt();
    auto files=vFiles.toStringList();
    p.freeFiles();
    for(const auto&v:files){
        auto file=new QFile(v);
        if(!file->open(QFile::ReadOnly))
            delete file;
        else{
            auto filename=file->fileName().split(qbl("/")).last();
            p.uploadedFiles.insert(filename, file);
        }
    }
}

QFile *QRPCListenRequest::temporaryFile(const QString &fileName)
{
    static QTemporaryFile f;
    dPvt();
    return p.uploadedFiles.value(fileName);
}

QVariantHash &QRPCListenRequest::responseHeader() const
{
    dPvt();
    return p._responseHeader;
}

void QRPCListenRequest::setResponseHeader(const QVariantHash &value)
{
    dPvt();
    p._responseHeader.clear();
    Q_V_HASH_ITERATOR(value){
        i.next();
        auto&v=i.value();
        auto s=(v.typeId()==QMetaType::QStringList || v.typeId()==QMetaType::QVariantList)?v.toStringList().join(' '):v.toString();
        p._responseHeader.insert(i.key(), s);
    }
}

void QRPCListenRequest::setResponseHeader(const QRPCHttpHeaders &value)
{
    this->setResponseHeader(value.rawHeader());
}

QVariantHash &QRPCListenRequest::responseCallback() const
{
    dPvt();
    return p._responseCallback;
}

void QRPCListenRequest::setResponseCallback(const QVariantHash &value)
{
    dPvt();
    p._responseCallback=value;
}

QVariant&QRPCListenRequest::responseBody() const
{
    dPvt();
    return p._responseBody;
}

QByteArray QRPCListenRequest::responseBodyBytes() const
{
    dPvt();
    QVariant v;
    const auto &response = p._responseBody;
    if(response.typeId()==QMetaType::QVariantList || response.typeId()==QMetaType::QStringList || response.typeId()==QMetaType::QVariantMap || response.typeId()==QMetaType::QVariantHash){
        if(p._requestContentType==AppJson)
            v=QJsonDocument::fromVariant(response).toJson(QJsonDocument::Compact);
        else if(p._requestContentType==AppCBOR)
            v=QCborValue::fromVariant(response).toCbor();
        else if(p._requestContentType==AppXML)
            v=QJsonDocument::fromVariant(response).toJson(QJsonDocument::Compact);
        else if(p._requestContentType==AppOctetStream)
            v=QCborValue::fromVariant(response).toCbor();
        else
            v=QJsonDocument::fromVariant(response).toJson(QJsonDocument::Compact);
    }
    else{
        v=QVariant::fromValue(response);
    }

    if(v.isValid()){
        QString body;
        if(v.typeId()==QMetaType::QDate)
            body=v.toDate().toString(Qt::ISODate);
        else if(v.typeId()==QMetaType::QTime)
            body=v.toTime().toString(Qt::ISODateWithMs);
        else if(v.typeId()==QMetaType::QDateTime)
            body=v.toDateTime().toString(Qt::ISODateWithMs);
        else if(v.typeId()==QMetaType::QUuid)
            body=v.toUuid().toString();
        else if(v.typeId()==QMetaType::QUrl)
            body=v.toUrl().toString();
        else
            body=v.toString();
        return body.toUtf8();
    }

    return QByteArray();
}

void QRPCListenRequest::setResponseBody(const QVariant &value)
{
    dPvt();
    if(value.typeId()==QMetaType::QByteArray || value.typeId()==QMetaType::QString || value.typeId()==QMetaType::QChar){
        if(value.toString().trimmed().isEmpty()){
            p._responseBody=QVariant();
            return;
        }
    }
    p._responseBody=value;
}

int QRPCListenRequest::responseCode(int code) const
{
    dPvt();
    return (p._responseCode>0)?p._responseCode:code;
}

void QRPCListenRequest::setResponseCode(const int &value)
{
    dPvt();
    p._responseCode = value;
}

QByteArray QRPCListenRequest::responsePhrase() const
{
    dPvt();
    if(p._responsePhrase.isEmpty())
        return QRPCListenRequestCode::reasonPhrase(this->responseCode()).toUtf8();
    else
        return p._responsePhrase;
}

QByteArray QRPCListenRequest::responsePhrase(const int code)const
{
    auto c=(code>0)?code:this->responseCode();
    return QRPCListenRequestCode::reasonPhrase(c).toUtf8();
}

void QRPCListenRequest::setResponsePhrase(const QByteArray &value)
{
    dPvt();
    p._responsePhrase = value;
}

const QUuid QRPCListenRequest::makeUuid()
{
    dPvt();
    p.makeUuid();
    return p._requestUuid;
}

int QRPCListenRequest::requestContentType() const
{
    dPvt();
    return p._requestContentType;
}

void QRPCListenRequest::setRequestContentType(const QVariant &value)
{
    dPvt();
    auto content_type=ContentType(value.toInt());
    if(!QRPCContentTypeHeaderTypeToHeader.contains(content_type))
        p._requestContentType = QRpc::AppNone;
    else
        p._requestContentType = content_type;
}

void QRPCListenRequest::setRequestResponse(QObject *request)
{
    auto writeResponse=[this](QRPCHttpResponse*response){
        if(response!=nullptr){
            this->setResponseHeader(response->header());
            this->setResponseBody(response->body());
            this->co().setCode(response->statusCode());
        }
    };

    QRPCHttpResponse*response=nullptr;
    if(request->metaObject()->className()==QRPCRequest::staticMetaObject.className())
        response=&dynamic_cast<QRPCRequest*>(request)->response();
    else
        response=dynamic_cast<QRPCHttpResponse*>(request);
    writeResponse(response);
}

void QRPCListenRequest::setControllerSetting(const ControllerSetting&setting)
{
    if(setting.enabled()){
        auto&set=setting;
        auto&rq=*this;
        VariantUtil vu;
        {
            auto vDes=rq.requestHeader();
            auto vSrc=set.headers();
            auto vMap=vu.vUnion(vSrc, vDes).toHash();
            rq.setRequestHeader(vMap);
        }
        {
            auto vDes=rq.requestParameter();
            auto vSrc=set.parameters();
            auto vMap=vu.vUnion(vSrc, vDes).toHash();
            rq.setRequestParameter(vMap);
        }
    }
}

QString QRpc::QRPCListenRequest::authorizationBasic() const
{
    dPvt();
    return p.authorizationParser(qbl("Basic"));
}

QString QRPCListenRequest::authorizationBearer() const
{
    dPvt();
    return p.authorizationParser(qbl("Bearer"));
}

QString QRPCListenRequest::authorizationService() const
{
    dPvt();
    return p.authorizationParser(qbl("Service"));
}

}

