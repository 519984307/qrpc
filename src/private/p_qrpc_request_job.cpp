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
Q_GLOBAL_STATIC(QVector<RequestJob*>, requestJobPool)


#define dPvt()\
auto &p =*reinterpret_cast<RequestJobPvt*>(this->p)

class RequestJobPvt:public QObject{
public:
    RequestJob*parent=nullptr;

#ifdef Q_RPC_HTTP
    RequestJobHttp _requestJobHttp;
#endif
#ifdef Q_RPC_WEBSOCKET
    RequestJobWSS _requestJobWSS;
#endif
#ifdef Q_RPC_TCP
    RequestJobTcp _requestJobTcp;
#endif
#ifdef Q_RPC_LOCALSOCKET
    RequestJobLocalSocket _requestJobLocalSocket;
#endif
#ifdef Q_RPC_DATABASE
    RequestJobDataBase _requestJobDataBase;
#endif
    Request::Action action=Request::acRequest;
    QString action_fileName;
    QSslConfiguration sslConfiguration;
    QHash<int,RequestJobProtocol*> _requestJobProtocolHash;
    RequestJobResponse _response;

    explicit RequestJobPvt(RequestJob*parent):QObject{parent},
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
        QHashIterator<int, RequestJobProtocol*> i(_requestJobProtocolHash);
        while (i.hasNext()){
            i.next();
            QObject::connect(i.value(), &RequestJobProtocol::callback, parent, &RequestJob::onRunCallback);
        }
    }

    virtual ~RequestJobPvt()
    {
        QHashIterator<int, RequestJobProtocol*> i(_requestJobProtocolHash);
        while (i.hasNext()){
            i.next();
            QObject::disconnect(i.value(), &RequestJobProtocol::callback, this->parent, &RequestJob::onRunCallback);
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
        RequestJobDataBase _requestJobDataBase;
#endif
        this->_response.clear();
        this->action=Request::acRequest;
        this->action_fileName.clear();
        this->sslConfiguration=QSslConfiguration();
    }
};

RequestJob::RequestJob():QThread{nullptr}
{
    this->p=new RequestJobPvt{this};
    this->moveToThread(this);
    static qlonglong taskCount=0;
    this->setObjectName(qsl("ReqJob%1").arg(++taskCount));
}

RequestJob::~RequestJob()
{
    dPvt();
    delete&p;
}

void RequestJob::run()
{
    this->exec();
}

RequestJob *RequestJob::newJob(Request::Action action, const QString &action_fileName)
{
    RequestJob*job=nullptr;
    if(!requestJobMutex->tryLock(10))
        job=new RequestJob();
    else{
        if(requestJobPool->isEmpty()){
            job=new RequestJob();
        }
        else{
            job=requestJobPool->takeFirst();
        }
        requestJobMutex->unlock();
    }
    auto &p=*static_cast<RequestJobPvt*>(job->p);
    p.action=action;
    p.action_fileName=action_fileName;
    return job;
}

RequestJob *RequestJob::runJob(RequestJob*job)
{
    if(job->isRunning()){
        job->quit();
        job->wait();
    }
    job->start();
    return job;
}

RequestJob &RequestJob::start()
{
    QThread::start();
    while(this->eventDispatcher()==nullptr){
        QThread::msleep(1);
    }
    return*this;
}

RequestJob &RequestJob::release()
{
    QMutexLOCKER locker(requestJobMutex);
    requestJobPool->append(this);
    return *this;
}

RequestJobResponse &RequestJob::response()
{
    dPvt();
    return p._response;
}

void RequestJob::setResponse(RequestJobResponse &value)
{
    dPvt();
    p._response = value;
}

void RequestJob::onRunJob(const QSslConfiguration *sslConfiguration, const QVariantHash &headers, const QVariant &vUrl, const QString &fileName, Request *request)
{
    dPvt();
    p.sslConfiguration=QSslConfiguration(*sslConfiguration);
    auto url=vUrl.toUrl();
    p.action_fileName=fileName;
    RequestJobResponse response(headers, url, *request, this);
    this->setResponse(response);
    this->onRun();
}

void RequestJob::onRunCallback(const QVariant &v)
{
    Q_UNUSED(v)
    this->quit();
}

void RequestJob::onRun()
{
    dPvt();
    const auto &e=this->response().request_exchange.call();
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
