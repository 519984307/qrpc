#pragma once

#include "../qrpc_request.h"
#include "./p_qrpc_util.h"
#include <QStm>

#include <QUuid>
#include <QDebug>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>

#include <QTimer>
#include <QThread>
#include <QUrl>
#include <QUrlQuery>

namespace QRpc {

//!
//! \brief The QRPCRequestJobResponse class
//!
class QRPCRequestJobResponse:public QObject{
    Q_OBJECT
public:

    QRPCRequestExchange request_exchange;
    QRPCRequest*qrpcRequest=nullptr;

    QUuid request_uuid=QUuid::createUuidV3(QUuid::createUuid(), QUuid::createUuid().toString().toUtf8());
    QVariant request_url;
    QVariantHash request_header;
    QVariantHash request_parameters;
    QByteArray request_body;
    qlonglong activityLimit=120000;
    QDateTime request_start;
    QDateTime request_finish;
    int response_status_code = QNetworkReply::NoError;
    QByteArray response_status_reason_phrase;
    QByteArray response_body;
    QNetworkReply::NetworkError response_qt_status_code = QNetworkReply::NoError;
    QHash<QString, QVariant> responseHeader;

    explicit QRPCRequestJobResponse(QObject*parent);

    explicit QRPCRequestJobResponse(const QVariantHash&request_header, const QVariant&vUrl, QRPCRequest&request, QObject*parent);

    ~QRPCRequestJobResponse();

    QRPCRequestJobResponse &operator =(QRPCRequestJobResponse &e);

    void clear();

    QVariantHash toMapResquest();

    QVariantHash toMapResponse();

    QVariantHash toVariant();

};

}
