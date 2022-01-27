#pragma once

#include <QSslCertificate>
#include <QSslConfiguration>
#include "../qrpc_request_exchange_setting.h"
#include "./p_qrpc_request_job_response.h"


namespace QRpc {

//!
//! \brief The QRPCRequestJob class
//!
class QRPCRequestJob : public QThread
{
    Q_OBJECT
public:


    explicit QRPCRequestJob();

    ~QRPCRequestJob();

    void run()override;

    static QRPCRequestJob*newJob(QRPCRequest::Action action, const QString&action_fileName={});

    QRPCRequestJob &start();

    QRPCRequestJob &release();

    QRPCRequestJobResponse&response();

    void setResponse(QRPCRequestJobResponse &value);



public slots:

    void onRunJob(const QSslConfiguration*sslConfiguration, const QVariantHash&headers, const QVariant&vUrl, const QString&fileName, QRpc::QRPCRequest*request);

    void onRunCallback(const QVariant&v);

    void onRun();
private:
    void*p=nullptr;
};

}
