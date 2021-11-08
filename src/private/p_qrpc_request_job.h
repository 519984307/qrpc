#pragma once

#include <QSslCertificate>
#include <QSslConfiguration>
#include "../qrpc_request_exchange_setting.h"
#include "./p_qrpc_request_job_http.h"
#ifdef COMPILE_AMQP
#include "./p_qrpc_request_job_amqp.h"
#endif
#ifdef COMPILE_KAFKA
#include "./p_qrpc_request_job_broker_kafka.h"
#endif
#include "./p_qrpc_request_job_localsocket.h"
#include "./p_qrpc_request_job_database.h"
#include "./p_qrpc_request_job_tcp.h"
#include "./p_qrpc_request_job_wss.h"

namespace QRpc {

class QRPCRequestJob : public QThread
{
    Q_OBJECT
public:
    QRPCRequestJobResponse _response;
    QRPCRequestJobWSS _requestJobWSS;
    QRPCRequestJobTcp _requestJobTcp;
    QRPCRequestJobHttp _requestJobHttp;
    QRPCRequestJobLocalSocket _requestJobLocalSocket;
    QRPCRequestJobDataBase _requestJobDataBase;
    QRPCRequest::Action action=QRPCRequest::acRequest;
    QString action_fileName;
    QSslConfiguration sslConfiguration;
    QHash<int,QRPCRequestJobProtocol*> _requestJobProtocolMap;

    explicit QRPCRequestJob():
        QThread(nullptr),
        _response(this),
        _requestJobWSS(this),
        _requestJobTcp(this),
        _requestJobHttp(this),
        _requestJobLocalSocket(this),
        _requestJobDataBase(this)
    {
        this->moveToThread(this);
        static qlonglong taskCount=0;
        this->setObjectName(qsl("ReqJob%1").arg(++taskCount));

        _requestJobProtocolMap[QRpc::WebSocket]=&this->_requestJobWSS;
        _requestJobProtocolMap[QRpc::TcpSocket]=&this->_requestJobTcp;
        _requestJobProtocolMap[QRpc::Http]=&this->_requestJobHttp;
        _requestJobProtocolMap[QRpc::Https]=&this->_requestJobHttp;
        _requestJobProtocolMap[QRpc::LocalSocket]=&this->_requestJobLocalSocket;
        _requestJobProtocolMap[QRpc::DataBase]=&this->_requestJobDataBase;

        QHashIterator<int, QRPCRequestJobProtocol*> i(_requestJobProtocolMap);
        while (i.hasNext()){
            i.next();
            QObject::connect(i.value(), &QRPCRequestJobProtocol::callback, this, &QRPCRequestJob::onRunCallback);
        }
    }

    ~QRPCRequestJob(){
        QHashIterator<int, QRPCRequestJobProtocol*> i(_requestJobProtocolMap);
        while (i.hasNext()){
            i.next();
            QObject::disconnect(i.value(), &QRPCRequestJobProtocol::callback, this, &QRPCRequestJob::onRunCallback);
        }
    }

    void run()override{
        this->exec();
    }

    auto start(){
        QThread::start();
        while(this->eventDispatcher()==nullptr){
            QThread::msleep(1);
        }
        return this;
    }

    QRPCRequestJobResponse&response()
    {
        return this->_response;
    }

    void setResponse(QRPCRequestJobResponse &value)
    {
        this->_response = value;
    }

public slots:

    void onRunJob(const QSslConfiguration*sslConfiguration, const QVariantHash&headers, const QVariant&vUrl, const QString&fileName, QRpc::QRPCRequest*request){
        this->sslConfiguration=QSslConfiguration(*sslConfiguration);
        auto url=vUrl.toUrl();
        this->action_fileName=fileName;
        QRPCRequestJobResponse response(headers, url, *request, this);
        this->setResponse(response);
        this->onRun();
    }

    void onRunCallback(const QVariant&v){
        Q_UNUSED(v)
        this->quit();
    }

    void onRun(){

        const auto&e=this->response().request_exchange.call();
        const auto iprotocol=e.protocol();
        auto protocol=this->_requestJobProtocolMap[iprotocol];

        if(protocol==nullptr){
            this->response().response_qt_status_code = QNetworkReply::ProtocolUnknownError;
            this->response().response_status_code = QNetworkReply::ProtocolUnknownError;
            this->quit();
        }
        else{
            protocol->sslConfiguration=this->sslConfiguration;
            protocol->action=this->action;
            protocol->action_fileName=this->action_fileName;
            protocol->call(&this->response());
        }
    }
};

}
