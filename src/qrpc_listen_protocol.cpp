#include "./qrpc_listen_protocol.h"
#include "./qrpc_listen_colletion.h"
#include "./qrpc_listen.h"
#include <QThreadPool>
#include <QSettings>
#include <QMetaProperty>

namespace QRpc {

static const char* listen_tcp_port="555";
static const char* listen_udp_port="556";
static const char* listen_web_port="8081";
static const char* listen_amqp_port="5672";
static const char* listen_mqtt_port="1883";
static const char* listen_http_port="8080";
static const char* listen_database_port="5432";
static const char* listen_kafka_port="2181";

#define dPvt()\
    auto&p =*reinterpret_cast<QRPCListenProtocolPvt*>(this->p)

class QRPCListenProtocolPvt:public QObject{
public:
    int protocol=-1;
    QMetaObject protocolMetaObject;
    QVariantHash settingsHash;
    QSettings*settings=nullptr;
    QByteArray optionName;
    int minThreads=1;
    int maxThreads=QThreadPool().maxThreadCount();
    int cleanupInterval=5000;
    int maxRequestSize=10000000;//9,536743164 MB
    int maxMultiPartSize=10000000;//9,536743164 MB
    QByteArray hostName;
    QVariantList port;
    QByteArray driver;
    QByteArray userName;
    QByteArray password;
    QByteArray database;
    QByteArray options;
    QVariantList queue;
    bool enabled=false;
    QByteArray sslKeyFile  ;
    QByteArray sslCertFile ;
    bool realMessageOnException=false;
    explicit QRPCListenProtocolPvt(QObject*parent):QObject(parent){
        this->settings=new QSettings(nullptr);
    }

    ~QRPCListenProtocolPvt(){
        delete this->settings;
    }

    QByteArray protocolName()const
    {
        const auto protocol=QRPCProtocol(this->protocol);
        auto __return=QRPCProtocolUrlName.value(protocol).toUtf8();
        return __return;
    }

    void setSettings(const QVariantHash&settings, const QVariantHash&defaultSettings){
        static auto exceptionProperty=QStringList{qsl("protocol"),qsl("protocolname"),qsl("optionname")};
        this->settingsHash=settings.isEmpty()?this->settingsHash:settings;
        const QMetaObject* metaObject = dynamic_cast<QRPCListenProtocol*>(this->parent())->metaObject();
        for(int i = metaObject->propertyOffset() ; i < metaObject->propertyCount() ; i++){
            auto property = metaObject->property(i);
            auto propertyName=QString::fromUtf8(property.name()).toLower();

            if(exceptionProperty.contains(propertyName))
                continue;

            propertyName=QString::fromUtf8(property.name());
            auto value=this->settingsHash.value(propertyName);
            if(!value.isValid()){
                value=defaultSettings.value(propertyName);
            }
            if(!property.write(this->parent(), value)){
                if(property.typeId()==QMetaType::QUuid && property.write(this->parent(), value.toUuid()))
                    continue;

                if((property.typeId()==QMetaType::LongLong || property.typeId()==QMetaType::ULongLong) && property.write(this->parent(), value.toLongLong()))
                    continue;

                if((property.typeId()==QMetaType::Int || property.typeId()==QMetaType::UInt) && property.write(this->parent(), value.toInt()))
                    continue;

                if((property.typeId()==QMetaType::Bool) && property.write(this->parent(), value.toBool()))
                    continue;
            }
        }
        this->makeMap();
    }


    QRPCListenProtocolPvt&makeMap(){
        this->settingsHash.clear();
        this->settings->clear();
        const QMetaObject* metaObject = dynamic_cast<QRPCListenProtocol*>(this->parent())->metaObject();
        for(int i = metaObject->propertyOffset() ; i < metaObject->propertyCount() ; i++){
            auto property = metaObject->property(i);
            auto key=property.name();
            auto value=property.read(this->parent());
            this->settingsHash.insert(key, value);
            this->settings->setValue(key, value);
        }
        return*this;
    }

public slots:
    void changeMap(){
        this->makeMap();
    }


};

QRPCListenProtocol::QRPCListenProtocol(QObject *parent):QObject(parent)
{
    this->p = new QRPCListenProtocolPvt(this);
}

QRPCListenProtocol::QRPCListenProtocol(int protocol, const QMetaObject&metaObject, QObject *parent):QObject(parent)
{
    this->p = new QRPCListenProtocolPvt(this);
    dPvt();
    QObject::connect(this, &QRPCListenProtocol::changeProperty, &p, &QRPCListenProtocolPvt::changeMap);
    p.protocol = protocol;
    p.optionName = p.protocolName();
    p.protocolMetaObject = metaObject;
    p.makeMap();
}

bool QRPCListenProtocol::isValid() const
{
    dPvt();
    return p.protocol>=0;
}

QRPCListen*QRPCListenProtocol::makeListen()
{
    dPvt();
    auto object=p.protocolMetaObject.newInstance();
    if(object!=nullptr){
        auto listen=dynamic_cast<QRPCListen*>(object);
        if(listen==nullptr){
            delete object;
            object=nullptr;
        }
        else{
            object->setObjectName(qsl("lis_%1").arg(QString::fromUtf8(this->protocolName())));
        }
        return listen;
    }
    return nullptr;
}

int QRPCListenProtocol::protocol()
{
    dPvt();
    return p.protocol;
}

void QRPCListenProtocol::setProtocol(const int &value)
{
    dPvt();
    p.protocol=value;
}

QByteArray QRPCListenProtocol::protocolName()
{
    dPvt();
    return p.protocolName();
}

QByteArray QRPCListenProtocol::optionName()
{
    dPvt();
    return p.optionName;
}

void QRPCListenProtocol::setSettings(const QVariantHash &settings, const QVariantHash &defaultSettings)
{
    dPvt();
    p.setSettings(settings, defaultSettings);
}

void QRPCListenProtocol::setOptionName(const QByteArray &value)
{
    dPvt();
    p.optionName=value;
}

int QRPCListenProtocol::minThreads() const
{
    dPvt();
    return p.minThreads;
}

void QRPCListenProtocol::setMinThreads(int value)
{
    dPvt();
    p.minThreads=value;
}

int QRPCListenProtocol::maxThreads() const
{
    dPvt();
    return p.maxThreads;
}

void QRPCListenProtocol::setMaxThreads(int value)
{
    dPvt();
    p.maxThreads=value;
}

int QRPCListenProtocol::cleanupInterval() const
{
    dPvt();
    return p.cleanupInterval;
}

void QRPCListenProtocol::setCleanupInterval(int value)
{
    dPvt();
    p.cleanupInterval=value;
}

int QRPCListenProtocol::maxRequestSize() const
{
    dPvt();
    return p.maxRequestSize;
}

void QRPCListenProtocol::setMaxRequestSize(int value)
{
    dPvt();
    p.maxRequestSize = value;
}

int QRPCListenProtocol::maxMultiPartSize() const
{
    dPvt();
    return p.maxMultiPartSize;
}

void QRPCListenProtocol::setMaxMultiPartSize(int value)
{
    dPvt();
    p.maxMultiPartSize = value;
}

QByteArray QRPCListenProtocol::driver() const
{
    dPvt();
    return p.driver;
}

void QRPCListenProtocol::setDriver(const QByteArray &value)
{
    dPvt();
    p.driver=value;
}


QByteArray QRPCListenProtocol::hostName() const
{
    dPvt();
    return p.hostName;
}

void QRPCListenProtocol::setHostName(const QByteArray &value)
{
    dPvt();
    p.hostName=value;
}

QByteArray QRPCListenProtocol::userName() const
{
    dPvt();
    return p.userName;
}

void QRPCListenProtocol::setUserName(const QByteArray &value)
{
    dPvt();
    p.userName=value;
}

QByteArray QRPCListenProtocol::password() const
{
    dPvt();
    return p.password;
}

void QRPCListenProtocol::setPassword(const QByteArray &value)
{
    dPvt();
    p.password=value;
}

QByteArray QRPCListenProtocol::database() const
{
    dPvt();
    return p.database;
}

void QRPCListenProtocol::setDatabase(const QByteArray &value)
{
    dPvt();
    p.database=value;
}

QByteArray QRPCListenProtocol::options() const
{
    dPvt();
    return p.options;
}

void QRPCListenProtocol::setOptions(const QByteArray &value)
{
    dPvt();
    p.options=value;
}

QVariantList&QRPCListenProtocol::queue()
{
    dPvt();
    return p.queue;
}

void QRPCListenProtocol::setQueue(const QByteArray &value)
{
    dPvt();
    p.queue.clear();
    p.queue<<value;
}

void QRPCListenProtocol::setQueue(const QVariantList &value)
{
    dPvt();
    p.queue=value;
}

QVariantList QRPCListenProtocol::port() const
{
    dPvt();
    if(p.port.isEmpty()){
        QByteArray port;
        if(p.protocol==QRPCProtocol::TcpSocket)
            port=listen_tcp_port;
        else if(p.protocol==QRPCProtocol::UdpSocket)
            port=listen_udp_port;
        else if(p.protocol==QRPCProtocol::WebSocket)
            port=listen_web_port;
        else if(p.protocol==QRPCProtocol::Http || p.protocol==QRPCProtocol::Https)
            port=listen_http_port;
        else if(p.protocol==QRPCProtocol::Amqp)
            port=listen_amqp_port;
        else if(p.protocol==QRPCProtocol::Mqtt)
            port=listen_mqtt_port;
        else if(p.protocol==QRPCProtocol::DataBase)
            port=listen_database_port;
        else if(p.protocol==QRPCProtocol::Kafka)
            port=listen_kafka_port;
        else
            port=qbl_null;

        if(!port.isEmpty())
            p.port<<QByteArray(port).trimmed();
    }

    return p.port;
}

void QRPCListenProtocol::setPort(const QVariant &value)
{
    dPvt();
    QVariantList l;
    if(value.typeId()==QMetaType::QStringList || value.typeId()==QMetaType::QVariantList){
        l=value.toList();
    }
    else{
        l<<value;
    }
    p.port=l;
}

QVariantMap QRPCListenProtocol::toMap() const
{
    dPvt();
    return QVariant(p.makeMap().settingsHash).toMap();
}

QVariantHash &QRPCListenProtocol::toHash() const
{
    dPvt();
    return p.makeMap().settingsHash;
}

QSettings &QRPCListenProtocol::settings() const
{   
    dPvt();
    return*p.makeMap().settings;
}

QSettings *QRPCListenProtocol::makeSettings(QObject*parent)
{
    dPvt();
    auto settings=new QSettings(parent);
    Q_V_HASH_ITERATOR(p.makeMap().settingsHash){
        i.next();
        settings->setValue(i.key().toLower(), i.value());
    }
    return settings;
}

QVariantHash QRPCListenProtocol::makeSettingsHash()const
{
    dPvt();
    return p.makeMap().settingsHash;
}

bool QRPCListenProtocol::enabled() const
{
    dPvt();
    return p.enabled;
}

void QRPCListenProtocol::setEnabled(bool value)
{
    dPvt();
    p.enabled=value;
}

QByteArray QRPCListenProtocol::sslKeyFile() const
{
    dPvt();
    return p.sslKeyFile;
}

void QRPCListenProtocol::setSslKeyFile(const QByteArray &value)
{
    dPvt();
    p.sslKeyFile = value;
}

QByteArray QRPCListenProtocol::sslCertFile() const
{
    dPvt();
    return p.sslCertFile;
}

void QRPCListenProtocol::setSslCertFile(const QByteArray &value)
{
    dPvt();
    p.sslCertFile = value;
}

bool QRPCListenProtocol::realMessageOnException() const
{
    dPvt();
    return p.realMessageOnException;
}

void QRPCListenProtocol::setRealMessageOnException(bool value)
{
    dPvt();
    p.realMessageOnException = value;
}

}
