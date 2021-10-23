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
    QRPCRequestJobWSS _QRPCRequestJobWSS;
    QRPCRequestJobTcp _QRPCRequestJobTcp;
    QRPCRequestJobHttp _QRPCRequestJobHttp;
    QRPCRequestJobLocalSocket _QRPCRequestJobLocalSocket;
    QRPCRequestJobDataBase _QRPCRequestJobDataBase;
    QRPCRequest::Action action=QRPCRequest::acRequest;
    QString action_fileName;
    QSslConfiguration sslConfiguration;
    QSslCertificate sslCertificate;
    QHash<int,QRPCRequestJobProtocol*> __requestJobProtocolMap;
    //QList<QSslError> sslErrorsExpected;
    explicit QRPCRequestJob():QThread(nullptr),
        _response(this),
        _QRPCRequestJobWSS(this),
        _QRPCRequestJobTcp(this),
        _QRPCRequestJobHttp(this),
        _QRPCRequestJobLocalSocket(this),
        _QRPCRequestJobDataBase(this)
    {
        this->moveToThread(this);
        static qlonglong taskCount=0;
        ++taskCount;
        this->setObjectName(qsl("QRPCRequestJob%1").arg(taskCount));

        __requestJobProtocolMap[QRpc::WebSocket]=&this->_QRPCRequestJobWSS;
        __requestJobProtocolMap[QRpc::TcpSocket]=&this->_QRPCRequestJobTcp;
        __requestJobProtocolMap[QRpc::Http]=&this->_QRPCRequestJobHttp;
        __requestJobProtocolMap[QRpc::Https]=&this->_QRPCRequestJobHttp;
        __requestJobProtocolMap[QRpc::LocalSocket]=&this->_QRPCRequestJobLocalSocket;
        __requestJobProtocolMap[QRpc::DataBase]=&this->_QRPCRequestJobDataBase;

        QHashIterator<int, QRPCRequestJobProtocol*> i(__requestJobProtocolMap);
        while (i.hasNext()){
            i.next();
            QObject::connect(i.value(), &QRPCRequestJobProtocol::callback, this, &QRPCRequestJob::onRunCallback);
        }
    }

    ~QRPCRequestJob(){
        QHashIterator<int, QRPCRequestJobProtocol*> i(__requestJobProtocolMap);
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

    void onRunJob(const QVariantHash&headers, const QVariant&vUrl, const QString&fileName, QRpc::QRPCRequest*request){
        auto url=vUrl.toUrl();
        const auto&sslCertificate=request->sslCertificate();
        if(!sslCertificate.isEmpty()){
            auto cert = QSslCertificate::fromPath(sslCertificate);
            if(!cert.isEmpty()){
                auto certificate=cert.first();
                sslConfiguration=request->sslConfiguration();
                sslConfiguration.setLocalCertificate(certificate);
            }
        }

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
        auto protocol=this->__requestJobProtocolMap[iprotocol];

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
