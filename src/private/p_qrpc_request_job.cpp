#include "./p_qrpc_request_job.h"
#ifdef Q_RPC_HTTP
#include "./p_qrpc_request_job_http.h"
#endif
#ifdef Q_RPC_AMQP
#include "./p_qrpc_request_job_amqp.h"
#endif
#ifdef Q_RPC_KAFKA
#include "./p_qrpc_request_job_broker_kafka.h"
#endif
#ifdef Q_RPC_LOCALSOCKET
#include "./p_qrpc_request_job_localsocket.h"
#endif
#ifdef Q_RPC_DATABASE
#include "./p_qrpc_request_job_database.h"
#endif
#ifdef Q_RPC_TCP
#include "./p_qrpc_request_job_tcp.h"
#endif
#ifdef Q_RPC_WEBSOCKET
#include "./p_qrpc_request_job_wss.h"
#endif

namespace QRpc {

Q_GLOBAL_STATIC(QMutex, requestJobMutex)
Q_GLOBAL_STATIC(QVector<QRPCRequestJob*>, requestJobPool)


#define dPvt()\
auto&p =*reinterpret_cast<QRPCRequestJobPvt*>(this->p)

class QRPCRequestJobPvt:public QObject{
public:
    QRPCRequestJob*parent=nullptr;

#ifdef Q_RPC_HTTP
    QRPCRequestJobHttp _requestJobHttp;
#endif
#ifdef Q_RPC_WEBSOCKET
    QRPCRequestJobWSS _requestJobWSS;
#endif
#ifdef Q_RPC_TCP
    QRPCRequestJobTcp _requestJobTcp;
#endif
#ifdef Q_RPC_LOCALSOCKET
    QRPCRequestJobLocalSocket _requestJobLocalSocket;
#endif
#ifdef Q_RPC_DATABASE
    QRPCRequestJobDataBase _requestJobDataBase;
#endif
    QRPCRequest::Action action=QRPCRequest::acRequest;
    QString action_fileName;
    QSslConfiguration sslConfiguration;
    QHash<int,QRPCRequestJobProtocol*> _requestJobProtocolHash;
    QRPCRequestJobResponse _response;

    explicit QRPCRequestJobPvt(QRPCRequestJob*parent):QObject(parent),
#ifdef Q_RPC_HTTP
        _requestJobHttp(this),
#endif
#ifdef Q_RPC_WEBSOCKET
        _requestJobWSS(this),
#endif
#ifdef Q_RPC_TCP
        _requestJobTcp(this),
#endif
#ifdef Q_RPC_LOCALSOCKET
        _requestJobLocalSocket(this),
#endif
#ifdef Q_RPC_DATABASE
        _requestJobDataBase(this)
#endif
        _response(parent)
    {
        this->parent=parent;
#ifdef Q_RPC_HTTP
        _requestJobProtocolHash[QRpc::Http]=&this->_requestJobHttp;
        _requestJobProtocolHash[QRpc::Https]=&this->_requestJobHttp;
#endif
#ifdef Q_RPC_WEBSOCKET
        _requestJobProtocolMap[QRpc::WebSocket]=&this->_requestJobWSS;
#endif
#ifdef Q_RPC_TCPSOCKET
        _requestJobProtocolMap[QRpc::TcpSocket]=&this->_requestJobTcp;
#endif
#ifdef Q_RPC_LOCALSOCKET
        _requestJobProtocolMap[QRpc::LocalSocket]=&this->_requestJobLocalSocket;
#endif
#ifdef Q_RPC_DATABASE
        _requestJobProtocolMap[QRpc::DataBase]=&this->_requestJobDataBase;
#endif
        QHashIterator<int, QRPCRequestJobProtocol*> i(_requestJobProtocolHash);
        while (i.hasNext()){
            i.next();
            QObject::connect(i.value(), &QRPCRequestJobProtocol::callback, parent, &QRPCRequestJob::onRunCallback);
        }
    }

    virtual ~QRPCRequestJobPvt()
    {
        QHashIterator<int, QRPCRequestJobProtocol*> i(_requestJobProtocolHash);
        while (i.hasNext()){
            i.next();
            QObject::disconnect(i.value(), &QRPCRequestJobProtocol::callback, this->parent, &QRPCRequestJob::onRunCallback);
        }
    }

    void clear()
    {
#ifdef Q_RPC_HTTP
        _requestJobHttp.clear();
#endif
#ifdef Q_RPC_WEBSOCKET
        _requestJobWSS.clear();
#endif
#ifdef Q_RPC_TCP
        _requestJobTcp.clear();
#endif
#ifdef Q_RPC_LOCALSOCKET
        _requestJobLocalSocket.clear();
#endif
#ifdef Q_RPC_DATABASE
        QRPCRequestJobDataBase _requestJobDataBase;
#endif
        this->_response.clear();
        this->action=QRPCRequest::acRequest;
        this->action_fileName.clear();
        this->sslConfiguration=QSslConfiguration();
    }
};

QRPCRequestJob::QRPCRequestJob():QThread(nullptr)
{
    this->p=new QRPCRequestJobPvt(this);
    this->moveToThread(this);
    static qlonglong taskCount=0;
    this->setObjectName(qsl("ReqJob%1").arg(++taskCount));
}

QRPCRequestJob::~QRPCRequestJob()
{
    dPvt();
    delete&p;
}

void QRPCRequestJob::run()
{
    this->exec();
}

QRPCRequestJob *QRPCRequestJob::newJob(QRPCRequest::Action action, const QString &action_fileName)
{
    QRPCRequestJob*job=nullptr;
    if(!requestJobMutex->tryLock(10))
        job=new QRPCRequestJob();
    else{
        if(requestJobPool->isEmpty()){
            job=new QRPCRequestJob();
        }
        else{
            job=requestJobPool->takeFirst();
            job->quit();
            job->wait();
        }
        requestJobMutex->unlock();
    }
    auto&p=*static_cast<QRPCRequestJobPvt*>(job->p);
    p.action=action;
    p.action_fileName=action_fileName;
    job->start();
    return job;
}

QRPCRequestJob &QRPCRequestJob::start()
{
    QThread::start();
    while(this->eventDispatcher()==nullptr){
        QThread::msleep(1);
    }
    return*this;
}

QRPCRequestJob &QRPCRequestJob::release()
{
    QMutexLocker locker(requestJobMutex);
    requestJobPool->append(this);
    return *this;
}

QRPCRequestJobResponse &QRPCRequestJob::response()
{
    dPvt();
    return p._response;
}

void QRPCRequestJob::setResponse(QRPCRequestJobResponse &value)
{
    dPvt();
    p._response = value;
}

void QRPCRequestJob::onRunJob(const QSslConfiguration *sslConfiguration, const QVariantHash &headers, const QVariant &vUrl, const QString &fileName, QRPCRequest *request)
{
    dPvt();
    p.sslConfiguration=QSslConfiguration(*sslConfiguration);
    auto url=vUrl.toUrl();
    p.action_fileName=fileName;
    QRPCRequestJobResponse response(headers, url, *request, this);
    this->setResponse(response);
    this->onRun();
}

void QRPCRequestJob::onRunCallback(const QVariant &v)
{
    Q_UNUSED(v)
    this->quit();
}

void QRPCRequestJob::onRun()
{
    dPvt();
    const auto&e=this->response().request_exchange.call();
    const auto iprotocol=e.protocol();
    auto protocol=p._requestJobProtocolHash[iprotocol];

    if(protocol==nullptr){
        this->response().response_qt_status_code = QNetworkReply::ProtocolUnknownError;
        this->response().response_status_code = QNetworkReply::ProtocolUnknownError;
        this->quit();
        return;
    }

    protocol->sslConfiguration=p.sslConfiguration;
    protocol->action=p.action;
    protocol->action_fileName=p.action_fileName;
    protocol->call(&this->response());
}

}
