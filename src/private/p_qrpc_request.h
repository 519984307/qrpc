#pragma once

#include "../qrpc_request.h"
#include "./p_qrpc_util.h"
#include "../qrpc_listen_request.h"
#include "./p_qrpc_request_job.h"
#include "./p_qrpc_http_response.h"
#include <QList>
#include <QMutex>
#include <QNetworkReply>
#include <QMetaProperty>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QtCborCommon>
#include <QCborStreamReader>
#include <QCborStreamWriter>
#include <QCborArray>
#include <QCborMap>
#include <QCborValue>
#include <QCborParserError>
#include <QThread>
#include <QStringList>
#include <QDir>
#include <QtCore/QCoreApplication>

namespace QRpc {


#define dPvt()\
    auto&p =*reinterpret_cast<QRPCRequestPvt*>(this->p)

class QRPCRequestPvt:public QObject{
    Q_OBJECT
public:
    QRPCRequest*parent=nullptr;
    QHash<int,int> requestRecovery;

    QRPCRequestExchange exchange;
    QRpc::QRPCHttpHeaders qrpcHeader;
    QRPCRequest::Body qrpcBody;
    QRpc::QRPCHttpResponse qrpcResponse;
    QRpc::LastError qrpcLastError;

    QUrl request_url;
    QVariant request_body;

    QNetworkReply::NetworkError response_qt_status_code=QNetworkReply::NoError;
    QString response_reason_phrase;
    QRPCListenRequest*request=nullptr;

    QString fileLog;

    QSslConfiguration sslConfiguration;

    explicit QRPCRequestPvt(QRPCRequest*parent);

    virtual ~QRPCRequestPvt();

    void setSettings(const ServiceSetting &setting);


    QString parseFileName(const QString&fileName);

    void writeLog(QRPCRequestJobResponse&response, const QVariant&request);

    QRPCHttpResponse &upload(const QString &route, const QString&fileName);

    QRPCHttpResponse &download(const QString &route, const QString&fileName);

    QRPCHttpResponse&call(const QRPCRequestMethod&method, const QVariant &vRoute, const QVariant &body);
signals:
    void runJob(const QSslConfiguration*sslConfig, const QVariantHash&headers, const QVariant&url, const QString&fileName, QRpc::QRPCRequest*request);
};

}
