#pragma once

#include "./p_qrpc_request_job_protocol.h"
#include <QHttpMultiPart>
#include <QTemporaryFile>
#include <QCoreApplication>
#include <QMutex>
#include <QSslConfiguration>
#include <QStandardPaths>

namespace QRpc {

//!
//! \brief The QRPCRequestJobHttp class
//!
class QRPCRequestJobHttp : public QRPCRequestJobProtocol
{
    Q_OBJECT
public:
    QString tempLocation;
    QTimer*__timeout=nullptr;
    QMutex mutexRequestFinished;
    QFile fileDownload;
    QFile fileUpload;
    QFile fileTemp;
    QNetworkRequest request;
    QRPCRequestJobResponse*response=nullptr;
    QNetworkReply *reply = nullptr;
    QNetworkAccessManager*nam = nullptr;

    Q_INVOKABLE explicit QRPCRequestJobHttp(QObject*parent);

    ~QRPCRequestJobHttp();

    void fileMake();

    void fileFree();

    virtual bool call(QRPCRequestJobResponse*response)override;

    void timeout_start();

    void timeout_stop();

private slots:

    void onReplyError(QNetworkReply::NetworkError e);;

    void onReplyFinish();;
    void onReplyTimeout();;

    void onReplyDelete();;

    void onReplyProgressUpload(qint64 bytesSent, qint64 bytesTotal);

    void onReplyProgressDownload(qint64 bytesReceived, qint64 bytesTotal);

    void onFinish();

};

}
