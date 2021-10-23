#pragma once

#include "./p_qrpc_request_job_protocol.h"
#include "../qrpc_listen_request.h"
#include <QDebug>
#include <QTcpSocket>
#include <QUrl>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QSqlError>

namespace QRpc {

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

    Q_INVOKABLE explicit QRPCRequestJobDataBase(QObject*parent):QRPCRequestJobProtocol(parent), listen_request(this), listen_response(this){
    }

    Q_INVOKABLE ~QRPCRequestJobDataBase(){
    }


    QRPCListenRequest&requestMake(QRPCRequestJobResponse*response){

        const auto&request_url=response->request_url;
        this->response=response;

        static const auto removeHeaders=QStringList()<<qsl("host")<< qsl("content-length");
        static const auto ignoreHeaders=QStringList()<<ContentDispositionName<<ContentTypeName<<ContentDispositionName.toLower()<<ContentTypeName.toLower();
        this->listen_request.clear();
        this->listen_request.makeUuid();

        auto requestHeader=this->listen_request.requestHeader();
        //auto requestParamMap=this->listen_request.requestParamMap();

        auto configureHeaders=[&requestHeader, &response](){

            if(!response->request_header.isEmpty()){
                QHashIterator<QString, QVariant> i(response->request_header);
                while (i.hasNext()) {
                    i.next();
                    const auto k=i.key().toLower();
                    if(removeHeaders.contains(k))
                        continue;
                    else if(ignoreHeaders.contains(k))
                        continue;
                    else{
                        auto v=i.value();
                        QStringList headerValues;
                        if(v.type()==v.List || v.type()==v.StringList){
                            auto vList=v.toList();
                            for(auto&r:vList){
                                headerValues<<r.toString().replace(qsl("\n"), qsl(";"));
                            }
                        }
                        else if(v.type()==v.Map || v.type()==v.Hash){
                            auto vMap=v.toHash();
                            QHashIterator<QString, QVariant> i(vMap);
                            while (i.hasNext()) {
                                i.next();
                                auto r=qsl("%1=%2").arg(i.value().toString(), v.toString()).replace(qsl("\n"), qsl(";"));
                                headerValues<<r;
                            }
                        }
                        else{
                            headerValues<<v.toString();
                        }
                        v=headerValues.join(qsl("; "));
                        auto k=i.key().toUtf8().trimmed();
                        requestHeader[k]=v;
#if Q_RPC_LOG_SUPER_VERBOSE
                        sInfo()<<":request.setRawHeader("<<i.key()<<", "<<i.value()<<")";
#endif
                    }
                }
            }
        };

        configureHeaders();

        requestHeader[qsl("broker-topic-request")]=request_url;
        requestHeader[qsl("broker-topic-response")]=this->listen_request.requestUuid().toString();

        auto v=requestHeader[qsl("Content-Type")];
        if(!v.isValid()){
            requestHeader[qsl("Content-Type")]=qsl("application/x-www-form-urlencoded");
        }
        response->request_start=QDateTime::currentDateTime();
        response->response_qt_status_code=QNetworkReply::NoError;
        response->response_status_code=0;

        auto requestMethod=response->request_exchange.call().method();
        auto methodRoute=response->request_exchange.call().route().toUtf8().toUpper();
        auto topicName=response->request_exchange.call().topic().toUtf8().toUpper();
        auto requestPath=methodRoute+qsl("/")+topicName;

        this->listen_request.setRequestMethod(requestMethod);
        this->listen_request.setRequestPath(requestPath);
        this->listen_request.setRequestHeader(requestHeader);
        this->listen_request.setRequestBody(response->request_body);
        return this->listen_request;
    }

    void connectionClose(){
        auto cnn=QSqlDatabase::database(this->sqlConnectionName);
        if(cnn.isValid()){
            cnn.close();
            cnn=QSqlDatabase();
            QSqlDatabase::removeDatabase(this->sqlConnectionName);
        }
        this->sqlConnectionName.clear();
        this->sqlDriver=nullptr;
        this->dbDriverType=QSqlDriver::UnknownDbms;
    }


    bool connectionMake(QSqlDatabase&outConnection){
        this->connectionClose();

        auto&exchange_call=this->response->request_exchange.call();


        auto driver=exchange_call.driver().trimmed();
        auto hostName=exchange_call.hostName().trimmed();
        auto userName=exchange_call.userName();
        auto password=exchange_call.passWord();
        auto port=exchange_call.port();
        auto connectOptions=settings[qsl("connectionoptions")].toString().trimmed();

        static int seq=0;
        auto connectionName=qsl("broker-%1-%2-%3").arg(driver).arg(port).arg(++seq).toLower();
        auto __connection=QSqlDatabase::addDatabase(driver,connectionName);


        if(__connection.isValid()){
            QString dataBaseName;
            if(__connection.driver()->dbmsType()==QSqlDriver::PostgreSQL)
                dataBaseName=qsl("postgres");
            else if(__connection.driver()->dbmsType()==QSqlDriver::MSSqlServer)
                dataBaseName=qsl("master");
            __connection.setHostName(hostName);
            __connection.setUserName(userName);
            __connection.setPassword(password);
            __connection.setPort(port);
            __connection.setDatabaseName(dataBaseName);
            __connection.setConnectOptions(connectOptions);
            if(!__connection.open()){
                auto msg=__connection.lastError().text();
                sWarning()<<msg;
                this->onBrokerError(msg);
            }
        }


        if(__connection.isOpen()){
            this->sqlConnectionName=__connection.connectionName();
            this->dbDriverType=__connection.driver()->dbmsType();
            this->sqlDriver=__connection.driver();
            this->requestPath=this->listen_request.requestPath();
            this->responsePath=this->listen_request.requestUuid().toString();
            if(!this->sqlDriver->subscribeToNotification(this->responsePath)){
                auto msg=__connection.lastError().text();
                sWarning()<<msg;
                this->onBrokerError(msg);
            }
            else{
                QObject::connect(sqlDriver, QOverload<const QString&, QSqlDriver::NotificationSource, const QVariant &>::of(&QSqlDriver::notification), this, &QRPCRequestJobDataBase::onReceiveBroker);
            }
        }
        outConnection=__connection;
        return outConnection.isOpen();
    }

    virtual bool call(QRPCRequestJobResponse*response){
        this->response=response;
        auto&request=this->requestMake(response);
        QSqlDatabase __connection;
        if(!this->connectionMake(__connection)){
            this->onBrokerError(qsl("invalid database connection"));
            return false;
        }
        else if(!__connection.isValid() || !__connection.isOpen()){
            this->onBrokerError(qsl("invalid database connection"));
            return false;
        }
        else{
            bool __return=false;
            auto request_url=response->request_url;
            auto requestBody=request.toJson();
            QString command;
            if(dbDriverType==QSqlDriver::PostgreSQL){
                command=qsl("select pg_notify('%1', '%2');").arg(request_url.toString(), requestBody);
            }
            if(!command.isEmpty()){
                QSqlQuery q(__connection);
                q.prepare(command);
                if(!q.exec()){
                    auto msg=q.lastError().text().trimmed();
                    sWarning()<<msg;
                    __return=msg.isEmpty();
                }
                else{
                    q.finish();
                    q.clear();
                    __return=true;
                }
            }
            __connection.close();
            return __return;
        }

    }

private slots:

    void onReceiveBroker(const QString &responseTopic, QSqlDriver::NotificationSource, const QVariant &responseBody){
        VariantUtil vu;
        if(!this->listen_response.fromHash(responseBody.toHash())){
            response->response_qt_status_code=QNetworkReply::InternalServerError;
            response->response_status_code=501;
            response->response_status_reason_phrase=qbl("Bad response");
        }
        else{
            auto topicUuidRequest=this->listen_request.requestUuid().toString();
            auto topicUuidResponse=this->listen_response.requestUuid().toString();
            auto topicUuidTopic=vu.toUuid(responseTopic).toString();
            if(!(topicUuidRequest==topicUuidResponse && topicUuidRequest==topicUuidTopic)){
                response->response_qt_status_code=QNetworkReply::InternalServerError;
                response->response_status_code=501;
                response->response_status_reason_phrase=qbl("Bad identification");
            }
            else{
                response->response_qt_status_code=QNetworkReply::NoError;
                response->response_status_code=200;
                response->response_status_reason_phrase.clear();
            }
        }
        this->onFinish();
    };

    void onBrokerError(const QString&error){
        response->response_qt_status_code=QNetworkReply::InternalServerError;
        response->response_status_reason_phrase=error.toUtf8();
        this->onFinish();
    };

    void onFinish(){
        QObject::disconnect(this->sqlDriver, QOverload<const QString&, QSqlDriver::NotificationSource, const QVariant &>::of(&QSqlDriver::notification), this, &QRPCRequestJobDataBase::onReceiveBroker);
        auto __connection=QSqlDatabase::database(this->sqlConnectionName);
        if(__connection.isValid()){
            __connection.close();
            __connection=QSqlDatabase();
            QSqlDatabase::removeDatabase(this->sqlConnectionName);
        }
        this->sqlDriver=nullptr;
        this->sqlConnectionName.clear();

        if(!listen_response.isValid()){
            response->response_qt_status_code=QNetworkReply::InternalServerError;
            response->response_status_code=404;
            response->response_status_reason_phrase=qbl("Internal server error");
            response->response_body.clear();
        }
        else if(response->response_status_code!=QNetworkReply::NoError){
            response->response_qt_status_code=QNetworkReply::InternalServerError;
            response->response_status_reason_phrase=qbl("Request timeout");
            response->response_body.clear();
        }
        else{
            auto code=listen_response.responseCode();
            response->response_qt_status_code = QNetworkReply::NetworkError(code==200?0:code);
            response->response_body = listen_response.responseBodyBytes();
            response->response_status_code = listen_response.responseCode();
            response->response_status_reason_phrase = listen_response.responsePhrase();
            if(response->response_status_code==0)
                response->response_status_code=response->response_qt_status_code;
        }
        emit this->callback(QVariant());
    }
};

}
