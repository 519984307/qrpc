#pragma once

#include <QSslCertificate>
#include <QSslConfiguration>
#include "../qrpc_request_exchange_setting.h"
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

//!
//! \brief The QRPCRequestJob class
//!
class QRPCRequestJob : public QThread
{
    Q_OBJECT
public:
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
    QRPCRequestJobResponse _response;
    QRPCRequest::Action action=QRPCRequest::acRequest;
    QString action_fileName;
    QSslConfiguration sslConfiguration;
    QHash<int,QRPCRequestJobProtocol*> _requestJobProtocolMap;

    explicit QRPCRequestJob():
        QThread(nullptr),
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
        _response(this)
    {
        this->moveToThread(this);
        static qlonglong taskCount=0;
        this->setObjectName(qsl("ReqJob%1").arg(++taskCount));
#ifdef Q_RPC_HTTP
        _requestJobProtocolMap[QRpc::Http]=&this->_requestJobHttp;
        _requestJobProtocolMap[QRpc::Https]=&this->_requestJobHttp;
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
        QHashIterator<int, QRPCRequestJobProtocol*> i(_requestJobProtocolMap);
        while (i.hasNext()){
            i.next();
            QObject::connect(i.value(), &QRPCRequestJobProtocol::callback, this, &QRPCRequestJob::onRunCallback);
        }
    }

    ~QRPCRequestJob()
    {
        QHashIterator<int, QRPCRequestJobProtocol*> i(_requestJobProtocolMap);
        while (i.hasNext()){
            i.next();
            QObject::disconnect(i.value(), &QRPCRequestJobProtocol::callback, this, &QRPCRequestJob::onRunCallback);
        }
    }

    void run()override
    {
        this->exec();
    }

    auto start()
    {
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

    void onRunJob(const QSslConfiguration*sslConfiguration, const QVariantHash&headers, const QVariant&vUrl, const QString&fileName, QRpc::QRPCRequest*request)
    {
        this->sslConfiguration=QSslConfiguration(*sslConfiguration);
        auto url=vUrl.toUrl();
        this->action_fileName=fileName;
        QRPCRequestJobResponse response(headers, url, *request, this);
        this->setResponse(response);
        this->onRun();
    }

    void onRunCallback(const QVariant&v)
    {
        Q_UNUSED(v)
        this->quit();
    }

    void onRun()
    {

        const auto&e=this->response().request_exchange.call();
        const auto iprotocol=e.protocol();
        auto protocol=this->_requestJobProtocolMap[iprotocol];

        if(protocol==nullptr){
            this->response().response_qt_status_code = QNetworkReply::ProtocolUnknownError;
            this->response().response_status_code = QNetworkReply::ProtocolUnknownError;
            this->quit();
            return;
        }

        protocol->sslConfiguration=this->sslConfiguration;
        protocol->action=this->action;
        protocol->action_fileName=this->action_fileName;
        protocol->call(&this->response());
    }
};

}
