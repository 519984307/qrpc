#pragma once

#include "./p_qrpc_request_job_protocol.h"
#include "../qrpc_listen_request.h"
#include <QDebug>
#include <QTcpSocket>
#include <QUrl>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

namespace QRpc {

//!
//! \brief The QRPCRequestJobDataBase class
//!
class QRPCRequestJobDataBase : public QRPCRequestJobProtocol
{
    Q_OBJECT
public:
    QVariantHash settings;
    QRPCListenRequest listen_request;
    QRPCListenRequest listen_response;
    QRPCRequestJobResponse*response=nullptr;
    QSqlDriver::DbmsType dbDriverType=QSqlDriver::UnknownDbms;
    QString sqlConnectionName=nullptr;
    QString requestPath;
    QString responsePath;
    QSqlDriver*sqlDriver=nullptr;

    Q_INVOKABLE explicit QRPCRequestJobDataBase(QObject*parent);

    ~QRPCRequestJobDataBase();


    QRPCListenRequest&requestMake(QRPCRequestJobResponse*response);

    void connectionClose();


    bool connectionMake(QSqlDatabase&outConnection);

    virtual bool call(QRPCRequestJobResponse*response)override;

private slots:

    void onReceiveBroker(const QString &responseTopic, QSqlDriver::NotificationSource, const QVariant &responseBody);;

    void onBrokerError(const QString&error);;

    void onFinish();
};

}
