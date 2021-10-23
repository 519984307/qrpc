#include "./qrpc_service_thread.h"
#include <QTimer>
#include <QMutex>

namespace QRpc {

#define dPvt()\
    auto&p =*reinterpret_cast<ServiceThreadPvt*>(this->p)

static qlonglong serviceThreadCount=0;

class ServiceThreadPvt: public QObject{
public:
    QMutex mutex;
    QDateTime serviceStarted=QDateTime::currentDateTime();
    ServiceManager manager;
    QString serviceName;
    QRpc::ServiceSetting setting;
    ServiceThread*service=nullptr;
    ServiceThread::Stats stats;

    explicit ServiceThreadPvt(ServiceThread*parent) : QObject(parent), manager(parent), service(parent){
        parent->moveToThread(parent);
    }

    virtual ~ServiceThreadPvt(){
    }

    void setManager(const ServiceManager&manager){
        this->manager.clear();
        this->manager.load(manager);
    }

public slots:
    void onRequestReceived(const QUuid &uuid, const QVariant&v){
        --stats.queue;
        emit service->request_state(uuid, service->Success, v);
        service->received(uuid, v);
    }
    void onRequestSuccess(const QUuid &uuid, const QVariant&detail){
        QMutexLocker locker(&this->mutex);
        ++stats.success;
        emit service->request_state(uuid, service->Success, detail);
    }
    void onRequestError(const QUuid &uuid, const QVariant&detail){
        QMutexLocker locker(&this->mutex);
        ++stats.error;
        emit service->request_state(uuid, service->Error, detail);
    }
    void onRequestDiscarted(const QUuid &uuid, const QVariant&detail){
        QMutexLocker locker(&this->mutex);
        ++stats.discated;
        emit service->request_state(uuid, service->Discarted, detail);
    }
    void onRequestCanceled(const QUuid&uuid, const QVariant&detail){
        QMutexLocker locker(&this->mutex);
        ++stats.canceled;
        emit service->request_state(uuid, service->Canceled, detail);
    }
};


void ServiceThread::Stats::clear()
{
    auto&p=*this;
    p.started=p.started.currentDateTime();
    p.received=0;
    p.success=0;
    p.error=0;
    p.discated=0;
    p.canceled=0;
}

QVariantHash ServiceThread::Stats::toMap()const
{
    return this->toHash();
}

QVariantHash ServiceThread::Stats::toHash() const
{
    auto&p=*this;
    QVariantHash v;
    v.insert(qsl("running" ),(p.service==nullptr)?false:p.service->isRunning());
    v.insert(qsl("started" ), p.started     );
    v.insert(qsl("received"), p.received    );
    v.insert(qsl("success" ), p.success     );
    v.insert(qsl("error"   ), p.error       );
    v.insert(qsl("discated"), p.discated    );
    v.insert(qsl("canceled"), p.canceled    );
    return v;

}

ServiceThread::ServiceThread(QObject *parent) : QThread(nullptr)
{
    Q_UNUSED(parent)
    this->p = new ServiceThreadPvt(this);
    dPvt();
    p.service=this;
}

ServiceThread::ServiceThread(const QString &serviceName, QObject *parent) : QThread(nullptr)
{
    Q_UNUSED(parent)
    this->p = new ServiceThreadPvt(this);
    dPvt();
    p.service=this;
    this->setServiceName(serviceName);
}

ServiceThread::ServiceThread(const ServiceManager &manager, QObject *parent) : QThread(nullptr)
{
    Q_UNUSED(parent)
    this->p = new ServiceThreadPvt(this);
    dPvt();
    p.service=this;
    p.setManager(manager);
}

ServiceThread::ServiceThread(const ServiceManager &manager, const QString &serviceName, QObject *parent) : QThread(nullptr)
{
    Q_UNUSED(parent)
    this->p = new ServiceThreadPvt(this);
    dPvt();
    p.service=this;
    p.setManager(manager);
    this->setServiceName(serviceName);
}

ServiceThread::~ServiceThread()
{
    dPvt();
    delete&p;
}

QRpc::ServiceSetting&ServiceThread::setting()
{
    dPvt();
    if(!p.setting.isValid()){
        p.setting=p.manager.setting(p.serviceName);
    }
    return p.setting;
}

ServiceManager &ServiceThread::manager()
{
    dPvt();
    return p.manager;
}

void ServiceThread::run()
{
    dPvt();
    p.serviceStarted=QDateTime::currentDateTime();

    QObject::connect(this, &ServiceThread::request_send    , &p, &ServiceThreadPvt::onRequestReceived );
    QObject::connect(this, &ServiceThread::request_success , &p, &ServiceThreadPvt::onRequestSuccess  );
    QObject::connect(this, &ServiceThread::request_error   , &p, &ServiceThreadPvt::onRequestError    );
    QObject::connect(this, &ServiceThread::request_discated, &p, &ServiceThreadPvt::onRequestDiscarted );
    QObject::connect(this, &ServiceThread::request_canceled, &p, &ServiceThreadPvt::onRequestCanceled );

    QTimer::singleShot(5,this,[this](){emit ServiceThread::service_started(this);});
    this->exec();

    QObject::disconnect(this, &ServiceThread::request_received, &p, &ServiceThreadPvt::onRequestReceived );
    QObject::disconnect(this, &ServiceThread::request_success , &p, &ServiceThreadPvt::onRequestSuccess  );
    QObject::disconnect(this, &ServiceThread::request_error   , &p, &ServiceThreadPvt::onRequestError    );
    QObject::disconnect(this, &ServiceThread::request_discated, &p, &ServiceThreadPvt::onRequestDiscarted );
    QObject::disconnect(this, &ServiceThread::request_canceled, &p, &ServiceThreadPvt::onRequestCanceled );

    emit this->service_terminate(this);
}

bool ServiceThread::start()
{
    dPvt();
    if(!this->isRunning()){
        auto objectName=this->objectName().trimmed();

        if(objectName.isEmpty()){
            objectName=p.serviceName.trimmed();

            if(objectName.isEmpty())
                objectName=QThread::currentThread()->objectName().trimmed();

            if(objectName.isEmpty())
                objectName=QString::number(qlonglong(QThread::currentThreadId()), 16);

            objectName=qsl("%1_%2").arg(objectName).arg(++serviceThreadCount);
        }
        p.manager.clear();
        QThread::start();
        while(QThread::eventDispatcher()!=nullptr)
            QThread::msleep(1);
    }
    return false;
}

bool ServiceThread::start(const ServiceManager&manager)
{
    dPvt();
    if(!this->isRunning()){
        auto objectName=this->objectName().trimmed();

        if(objectName.isEmpty()){
            objectName=p.serviceName.trimmed();

            if(objectName.isEmpty())
                objectName=QThread::currentThread()->objectName().trimmed();

            if(objectName.isEmpty())
                objectName=QString::number(qlonglong(QThread::currentThreadId()), 16);

            objectName=qsl("%1_%2").arg(objectName).arg(++serviceThreadCount);
        }
        p.setManager(manager);
        QThread::start();
        while(QThread::eventDispatcher()!=nullptr)
            QThread::msleep(1);
    }
    return false;
}

bool ServiceThread::stop()
{
    if(this->isRunning()){
        this->quit();
        return true;
    }
    return false;
}

ServiceThread::Stats&ServiceThread::stats() const
{
    dPvt();
    return p.stats;
}

void ServiceThread::received(const QUuid &uuid, const QVariant &v)
{
    Q_UNUSED(uuid)
    Q_UNUSED(v)
    emit this->request_success(uuid, QVariant());
}

void ServiceThread::dispatcher(const QUuid &uuid, const QVariant &v)
{
    dPvt();
    QMutexLocker locker(&p.mutex);
    ++p.stats.queue;
    emit this->request_send(uuid, v);
}

QVariantHash ServiceThread::toMap() const
{
    dPvt();
    QVariantHash v;
    v.insert(qsl("service"), this->objectName());
    v.insert(qsl("stats"  ), p.stats.toHash()   );
    return v;
}

QString ServiceThread::serviceName() const
{
    dPvt();
    return p.serviceName;
}

void ServiceThread::setServiceName(const QString &value)
{
    dPvt();
    p.serviceName = value.trimmed();
}

}
