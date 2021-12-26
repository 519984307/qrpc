#pragma once

#include "../qrpc_request.h"
#include "./p_qrpc_util.h"
#include "./qstm_util_variant.h"

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

    Q_INVOKABLE explicit QRPCRequestJobResponse(QObject*parent):QObject(parent){
    }
    explicit QRPCRequestJobResponse(const QVariantHash&request_header, const QVariant&vUrl, QRPCRequest&request, QObject*parent):QObject(parent){
        this->request_url = vUrl;
        this->request_header = request_header;
        this->request_body = request.body().body().toByteArray();
        this->request_parameters = request.body().toHash();
        this->activityLimit = request.activityLimit();
        this->request_exchange = request.exchange();
    }

    Q_INVOKABLE  virtual ~QRPCRequestJobResponse(){
    }

    /**
     * @brief operator =
     * @param v
     * @return
     */
    QRPCRequestJobResponse &operator =(QRPCRequestJobResponse &e){
        this->request_exchange=e.request_exchange;
        this->qrpcRequest=e.qrpcRequest;
        this->request_url=e.request_url;
        this->request_header=e.request_header;
        this->request_body=e.request_body;
        this->activityLimit=(e.activityLimit>0)?e.activityLimit:this->activityLimit;
        this->request_start=e.request_start;
        this->response_status_code=e.response_status_code;
        this->response_status_reason_phrase=e.response_status_reason_phrase;
        this->response_body=e.response_body;
        this->response_qt_status_code=e.response_qt_status_code;
        this->responseHeader=e.responseHeader;
        return*this;
    }

    QVariantHash toMapResquest(){
        auto method=this->request_exchange.call().method();
        return Util::toMapResquest(method,request_url,request_body,request_parameters,response_body,request_header,request_start,request_finish);
    }

    QVariantHash toMapResponse(){
        QVariantHash map;
        VariantUtil vu;
        map.insert(qsl("finish"), QDateTime::currentDateTime());
        map.insert(qsl("header"), this->responseHeader);
        map.insert(qsl("status_code"), this->response_status_code);
        map.insert(qsl("qt_status_code"), this->response_qt_status_code);
        map.insert(qsl("status_reason_phrase"), this->response_status_reason_phrase);
        map.insert(qsl("body"), vu.toVariant(this->response_body));
        return map;
    }

    QVariantHash toVariant(){
        auto rpclog = QVariantList{qvh{{qsl("resquest"), this->toMapResquest()}},qvh{ {qsl("response"), this->toMapResponse()}}};
        ///*{"openapi", this->toMapOpenAPI()}*,/
        return qvh({{qsl("rpclog"), rpclog}});
    }

};

}
