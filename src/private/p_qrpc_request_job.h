#pragma once

#include <QSslCertificate>
#include <QSslConfiguration>
#include "../qrpc_request_exchange_setting.h"
#include "./p_qrpc_request_job_response.h"


namespace QRpc {

//!
//! \brief The RequestJob class
//!
class RequestJob : public QThread
{
    Q_OBJECT
public:


    explicit RequestJob();

    ~RequestJob();

    void run()override;

    static RequestJob*newJob(Request::Action action, const QString&action_fileName={});
    static RequestJob*runJob(RequestJob *job);

    RequestJob &start();

    RequestJob &release();

    RequestJobResponse&response();

    void setResponse(RequestJobResponse &value);



public slots:

    void onRunJob(const QSslConfiguration*sslConfiguration, const QVariantHash&headers, const QVariant&vUrl, const QString&fileName, QRpc::Request*request);

    void onRunCallback(const QVariant&v);

    void onRun();
private:
    void*p=nullptr;
};

}
