#include "./qrpc_request_exchange_setting.h"
#include "./qstm_util_meta_object.h"
#include "./qstm_util_variant.h"
#include <QMetaProperty>

namespace QRpc {

#define dPvt()\
    auto&p =*reinterpret_cast<QRPCRequestExchangeSettingPvt*>(this->p)

const static auto defaultLimit=60000;

class QRPCRequestExchangeSettingPvt{
public:
    QRPCRequestMethod method=QRPCRequestMethod::Post;
    QRPCProtocol protocol=QRPCProtocol::Http;
    QString vHost="/";
    QString hostName="localhost";
    QString driver;
    QString userName="guest";
    QString passWord="guest";
    QString route;
    QString topic;
    QVariantHash parameter;
    int port=8080;
    QVariant activityLimit=defaultLimit;
    QRPCRequestExchangeSetting*parent=nullptr;
    explicit QRPCRequestExchangeSettingPvt(QRPCRequestExchangeSetting*parent){
        this->parent=parent;
    }

    virtual ~QRPCRequestExchangeSettingPvt(){
    }

    QString protocolName()const{
        const auto&v=QRPCProtocolName.value(this->protocol);
        return v;
    }

    QString protocolUrlName()const{
        const auto&v=QRPCProtocolUrlName.value(this->protocol);
        return v;
    }
};

QRPCRequestExchangeSetting::QRPCRequestExchangeSetting(QObject *parent):QObject(parent)
{
    this->p = new QRPCRequestExchangeSettingPvt(this);
}

QRPCRequestExchangeSetting::QRPCRequestExchangeSetting(QRPCRequestExchangeSetting &e, QObject *parent):QObject(parent)
{
    this->p = new QRPCRequestExchangeSettingPvt(this);
    *this=e;
}

QRPCRequestExchangeSetting::~QRPCRequestExchangeSetting()
{
    dPvt();delete&p;
}

QRPCRequestExchangeSetting&QRPCRequestExchangeSetting::operator=(const QRPCRequestExchangeSetting &e)
{
    QStm::MetaObjectUtil util(*e.metaObject());
    auto values=util.toHash(&e);\
    util.writeHash(this, values);
    return*this;
}

QRPCRequestExchangeSetting &QRPCRequestExchangeSetting::operator=(const QVariantHash &e)
{
    QStm::MetaObjectUtil util;
    util.writeHash(this, e);
    return*this;
}

QVariantMap QRPCRequestExchangeSetting::toMap() const
{
    QVariantMap map;
    auto&e=*this;
    for(int i = 0; i < e.metaObject()->propertyCount(); ++i) {
        auto property=e.metaObject()->property(i);
        if(QByteArray(property.name()) == qbl("objectName"))
            continue;
        else{
            auto v=property.read(&e);
            if(v.isValid())
                map.insert(property.name(), v);
        }
    }
    return map;
}

QVariantHash QRPCRequestExchangeSetting::toHash() const
{
    QVariantHash map;
    auto&e=*this;
    for(int i = 0; i < e.metaObject()->propertyCount(); ++i) {
        auto property=e.metaObject()->property(i);
        if(QByteArray(property.name()) == qbl("objectName"))
            continue;
        else{
            auto v=property.read(&e);
            if(v.isValid())
                map.insert(property.name(), v);
        }
    }
    return map;
}

QString QRPCRequestExchangeSetting::url() const
{
    auto __return = qsl("%1:||%2:%3/%4").arg(this->protocolName(),this->hostName(),QString::number(this->port()),this->route());
    while(__return.contains(qsl("//")))
        __return=__return.replace(qsl("//"), qsl("/"));
    __return=__return.replace(qsl("||"), qsl("//"));

    return __return;
}

bool QRPCRequestExchangeSetting::isValid() const
{
    if(this->protocol()==QRpc::Amqp && !this->topic().isEmpty())
        return true;
    else
        return false;

}

QRPCRequestExchangeSetting &QRPCRequestExchangeSetting::print(const QString &output)
{
    for(auto&v:this->printOut(output))
        sInfo()<<v;
    return*this;
}

QStringList QRPCRequestExchangeSetting::printOut(const QString &output)
{
    auto space=output.trimmed().isEmpty()?qsl_null:qsl("    ");
    QStringList out;
    VariantUtil vu;
    auto vMap=this->toHash();
    if(!vMap.isEmpty()){
        out<<qsl("%1%2 attributes").arg(space, output).trimmed();
        QHashIterator<QString, QVariant> i(vMap);
        while (i.hasNext()){
            i.next();
            if(i.key().toLower()==qsl("password"))
                continue;
            out<<qsl("%1%2     %2:%3").arg(space, output, i.key(), vu.toStr(i.value()));
        }
    }
    return out;
}


QRPCRequestMethod QRPCRequestExchangeSetting::method()const
{
    dPvt();
    return p.method;
}

void QRPCRequestExchangeSetting::setMethod(const int &value)
{
    dPvt();
    auto method=QRPCRequestMethod(value);
    method=(method<Head || method>MaxMethod)?Post:method;
    p.method=method;
}

void QRPCRequestExchangeSetting::setMethod(const QString &value)
{
    dPvt();
    const auto vv=value.trimmed().toLower();
    for (const auto&v : QRPCRequestMethodNameList){
        if(v.trimmed().toLower()==vv){
            const auto&i = QRPCRequestMethodName.key(v);
            p.method=QRPCRequestMethod(i);
            return;
        }
    }
    p.method=QRpc::Post;
}

QString QRPCRequestExchangeSetting::methodName() const
{
    dPvt();
    auto name=QRPCRequestMethodName[p.method];
    return name;
}

QRPCProtocol QRPCRequestExchangeSetting::protocol() const
{
    dPvt();
    return p.protocol;
}

QString QRPCRequestExchangeSetting::protocolName() const
{
    dPvt();
    return p.protocolName();
}

QString QRPCRequestExchangeSetting::protocolUrlName() const
{
    dPvt();
    return p.protocolUrlName();
}

void QRPCRequestExchangeSetting::setProtocol(const QRPCProtocol &value)
{
    dPvt();
    p.protocol=value;
}

void QRPCRequestExchangeSetting::setProtocol(const QVariant &value)
{
    dPvt();
    auto&v=p.protocol;
    if(value.isNull() || !value.isValid())
        v=QRPCProtocol::Http;
    else if(QString::number(value.toInt())==value)
        v=QRPCProtocol(value.toInt());
    else if(value.toString().trimmed().isEmpty())
        v=QRPCProtocol::Http;
    else
        v=QRPCProtocol(QRPCProtocolType.value(value.toString().trimmed()));

    v=(v>rpcProtocolMax)?rpcProtocolMax:v;
    v=(v<rpcProtocolMin)?rpcProtocolMin:v;

}

QString QRPCRequestExchangeSetting::driver() const
{
    dPvt();
    return p.driver;
}

void QRPCRequestExchangeSetting::setDriver(const QString &value)
{
    dPvt();
    p.driver=value;
}

QString QRPCRequestExchangeSetting::hostName() const
{
    dPvt();
    return p.hostName;
}

void QRPCRequestExchangeSetting::setHostName(const QString &value)
{
    dPvt();
    p.hostName=value;
}

QString QRPCRequestExchangeSetting::vHost() const
{
    dPvt();
    return p.vHost;
}

void QRPCRequestExchangeSetting::setVHost(const QString &value)
{
    dPvt();
    p.vHost=value;
}

QString QRPCRequestExchangeSetting::userName() const
{
    dPvt();
    return p.userName;
}

void QRPCRequestExchangeSetting::setUserName(const QString &value)
{
    dPvt();
    p.userName=value;
}

QString QRPCRequestExchangeSetting::passWord() const
{
    dPvt();
    return p.passWord;
}

void QRPCRequestExchangeSetting::setPassWord(const QString &value)
{
    dPvt();
    p.passWord=value;
}

QString &QRPCRequestExchangeSetting::route() const
{
    dPvt();
    return p.route;
}

void QRPCRequestExchangeSetting::setRoute(const QVariant &value)
{
    dPvt();
    p.route = value.toString();
}

QString QRPCRequestExchangeSetting::topic() const
{
    dPvt();
    return p.topic;
}

void QRPCRequestExchangeSetting::setTopic(const QString &value)
{
    dPvt();
    p.topic=value;
}

int QRPCRequestExchangeSetting::port() const
{
    dPvt();
    return p.port;
}

void QRPCRequestExchangeSetting::setPort(int port)
{
    dPvt();
    p.port = port;
}

qlonglong QRPCRequestExchangeSetting::activityLimit() const
{
    dPvt();
    auto l=p.activityLimit.toLongLong();
    l=(l<1000)?defaultLimit:l;
    return l;
}

void QRPCRequestExchangeSetting::setActivityLimit(const QVariant &value)
{
    dPvt();
    p.activityLimit=value;
}

QVariantHash QRPCRequestExchangeSetting::parameter() const
{
    dPvt();
    return p.parameter;
}

void QRPCRequestExchangeSetting::setParameter(const QVariantHash &parameter)
{
    dPvt();
    p.parameter = parameter;
}



}
