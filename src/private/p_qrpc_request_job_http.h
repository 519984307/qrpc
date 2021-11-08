#pragma once

#include "./p_qrpc_request_job_protocol.h"
#include <QHttpMultiPart>
#include <QTemporaryFile>
#include <QMutex>
#include <QSslConfiguration>

namespace QRpc {

class QRPCRequestJobHttp : public QRPCRequestJobProtocol
{
    Q_OBJECT
public:
    QTimer*__timeout=nullptr;
    QMutex mutexRequestFinished;
    QFile fileDownload;
    QFile fileUpload;
    QTemporaryFile fileTemp;
    QNetworkRequest request;
    QRPCRequestJobResponse*response=nullptr;
    QNetworkReply *reply = nullptr;
    QNetworkAccessManager*nam = nullptr;    

    Q_INVOKABLE explicit QRPCRequestJobHttp(QObject*parent):QRPCRequestJobProtocol(parent){
        fileTemp.setAutoRemove(false);
    }

    Q_INVOKABLE ~QRPCRequestJobHttp(){
        if(this->fileTemp.isOpen())
            this->fileTemp.close();
        if(this->nam!=nullptr)
            this->nam->deleteLater();
    }

    virtual bool call(QRPCRequestJobResponse*response)override{
        static const QVector<QString> removeHeaders={{qsl("host")},{qsl("content-length")}};
        static const QVector<QString> ignoreHeaders={{ContentDispositionName}, {ContentTypeName}, {ContentDispositionName.toLower()}, {ContentTypeName.toLower()}};

        if(this->nam==nullptr)
            this->nam = new QNetworkAccessManager(nullptr);

#if Q_RPC_LOG_SUPER_VERBOSE
        sWarning()<<tr("request.url: %1").arg(response->request_url);
#endif

        const auto&request_url=response->request_url.toUrl();
        this->response=response;

        const auto&requestBody = response->request_body;
        this->request=QNetworkRequest(QUrl(request_url));
        this->request.setSslConfiguration(sslConfiguration);
        QHash<QByteArray,QByteArray> request_header_ignored;
        QHash<QByteArray,QByteArray> request_header;

        auto configureHeaders=[this, &request_header, &request_header_ignored, &response](){

            if(!response->request_header.isEmpty()){
                QHashIterator<QString, QVariant> i(response->request_header);
                while (i.hasNext()) {
                    i.next();
                    if(removeHeaders.contains(i.key().toLower()))
                        continue;
                    else{
                        auto v=i.value();
                        QStringList headerValues;
                        if(v.typeId()==QMetaType::QVariantList || v.typeId()==QMetaType::QStringList){
                            auto vList=v.toList();
                            for(auto&r:vList){
                                headerValues<<r.toString().replace(qsl("\n"), qsl(";"));
                            }
                        }
                        else if(v.typeId()==QMetaType::QVariantMap || v.typeId()==QMetaType::QVariantHash){
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
                        if(!ignoreHeaders.contains(k.toLower()))
                            request_header.insert(k, v.toByteArray());
                        else
                            request_header_ignored.insert(k, v.toByteArray());
#if Q_RPC_LOG_SUPER_VERBOSE
                        sInfo()<<":request.setRawHeader("<<i.key()<<", "<<i.value()<<")";
#endif
                    }
                }
            }

            if(!request_header.isEmpty()){
                QHashIterator<QByteArray,QByteArray> i(request_header);
                while (i.hasNext()) {
                    i.next();
                    const auto&k=i.key();
                    const auto&v=i.value();
                    this->request.setRawHeader(k, v);
                }
            }
        };

        configureHeaders();

        auto configureHeadersIgnored=[this, &request_header_ignored](){

            QHashIterator<QByteArray,QByteArray> i(request_header_ignored);
            while (i.hasNext()) {
                i.next();
                const auto&k=i.key();
                const auto&v=i.value();
                this->request.setRawHeader(k, v);
            }
        };

        if(action==QRPCRequest::acUpload){
            if(this->fileUpload.isOpen())
                this->fileUpload.close();
            this->fileUpload.setFileName(this->action_fileName);
            if(!this->fileUpload.exists()){
                auto msg=qsl("invalid upload fileName:%1")+this->fileDownload.fileName();
                sWarning()<<msg;
                response->response_qt_status_code=QNetworkReply::NoError;
                response->response_status_code=-1;
                response->response_status_reason_phrase=msg.toUtf8();
            }
            else if(!this->fileUpload.open(this->fileUpload.ReadOnly)){
                auto msg=qsl("invalid upload fileName:%1, %2").arg(this->fileDownload.fileName(), this->fileUpload.errorString());
                sWarning()<<msg;
                response->response_qt_status_code=QNetworkReply::NoError;
                response->response_status_code=-1;
                response->response_status_reason_phrase=msg.toUtf8();
            }
            else{
                auto multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
                QHttpPart httpPart;
                QHashIterator<QByteArray,QByteArray> i(request_header_ignored);
                while (i.hasNext()) {
                    i.next();
                    const auto&k=i.key();
                    const auto&v=i.value();
                    httpPart.setRawHeader( k, v );
                }
                httpPart.setBodyDevice(&this->fileUpload);
                multiPart->append(httpPart);
                this->reply=nam->post(this->request, multiPart);
                multiPart->setParent(reply);
                QObject::connect(this->reply, &QNetworkReply::uploadProgress , this, &QRPCRequestJobHttp::onReplyProgressUpload);
            }
        }
        else{
            configureHeadersIgnored();
            if(action==QRPCRequest::acDownload){
                this->fileDownload.setFileName(this->action_fileName);
                if(!this->fileDownload.open(QFile::Truncate | QFile::WriteOnly | QFile::Unbuffered)){
                    auto msg=qsl("invalid download fileName: %1").arg(this->fileDownload.fileName());
                    sWarning()<<msg;
                    response->response_qt_status_code=QNetworkReply::NoError;
                    response->response_status_code=-1;
                    response->response_status_reason_phrase = msg.toUtf8();
                }
                else if(!this->fileTemp.open()){
                    auto msg=qsl("invalid download temporary fileName: %1").arg(this->fileTemp.fileName());
                    sWarning()<<msg;
                    response->response_qt_status_code=QNetworkReply::NoError;
                    response->response_status_code=-1;
                    response->response_status_reason_phrase = msg.toUtf8();
                }
                else{
                    this->reply=nam->get(this->request);
                    QObject::connect(this->reply, &QNetworkReply::downloadProgress , this, &QRPCRequestJobHttp::onReplyProgressDownload);
                }
            }
            else /*else if(action==QRPCRequest::acRequest){*/ {
                auto method=response->request_exchange.call().method();
                auto methodName=response->request_exchange.call().methodName().toUtf8().toUpper();
                if(method==QRpc::Post || method==QRpc::Put){
                    auto v=this->request.header(QNetworkRequest::ContentTypeHeader);
                    if(!v.isValid()){
                        this->request.setHeader(QNetworkRequest::ContentTypeHeader, qsl("application/x-www-form-urlencoded"));
                    }
                }

                if(method==QRpc::Head)
                    this->reply=nam->head(this->request);
                else if(method==QRpc::Options)
                    this->reply=nam->sendCustomRequest(this->request, methodName);
                else if(method==QRpc::Get)
                    this->reply=nam->get(this->request);
                else if(method==QRpc::Post)
                    this->reply=nam->post(this->request, requestBody);
                else if(method==QRpc::Put)
                    this->reply=nam->put(this->request, requestBody);
                else if(method==QRpc::Delete)
                    this->reply=nam->deleteResource(this->request);
                else
                    this->reply=nam->sendCustomRequest(this->request, methodName, requestBody);
            }
        }


        response->request_start=QDateTime::currentDateTime();
        if(this->reply==nullptr){
            return false;
        }
        else{
            response->response_qt_status_code=QNetworkReply::NoError;
            response->response_status_code=0;

            QObject::connect(this->reply, &QNetworkReply::destroyed, this, &QRPCRequestJobHttp::onReplyDelete);
            QObject::connect(this->reply, &QNetworkReply::finished, this, &QRPCRequestJobHttp::onReplyFinish);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
            QObject::connect(this->reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred), this, &QRPCRequestJobHttp::onReplyError);
#else
            QObject::connect(this->reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), this, &QRPCRequestJobHttp::onReplyError);
#endif
            this->reply->ignoreSslErrors();

            return true;
        }
    }

    void timeout_start(){
        if(response->activityLimit>0){
            this->__timeout=new QTimer(nullptr);
            this->__timeout->setInterval(response->activityLimit);
            QObject::connect(this->__timeout, &QTimer::timeout, this, &QRPCRequestJobHttp::onReplyTimeout);
            this->__timeout->start();
        }
    }

    void timeout_stop(){
        auto timer=this->__timeout;
        this->__timeout=nullptr;
        if(timer!=nullptr){
            timer->stop();
            QObject::disconnect(timer, &QTimer::timeout, this, &QRPCRequestJobHttp::onReplyTimeout);
            timer->deleteLater();
        }
    }

private slots:

    void onReplyError(QNetworkReply::NetworkError e){
        if(response->response_qt_status_code==QNetworkReply::NoError){
            response->response_qt_status_code=e;
        }
        this->onFinish();
    };

    void onReplyFinish(){
        QMutexLocker<QMutex> locker(&mutexRequestFinished);
        if(this->reply!=nullptr){
            if(response->response_qt_status_code==QNetworkReply::NoError){
                if(response->response_qt_status_code!=QNetworkReply::TimeoutError)
                    response->response_qt_status_code = this->reply->error();
                this->onFinish();
            }
        }

    };
    void onReplyTimeout(){
        {
            QMutexLocker<QMutex> locker(&mutexRequestFinished);
            if(response->response_qt_status_code==QNetworkReply::NoError){
                response->response_qt_status_code=QNetworkReply::TimeoutError;
#if Q_RPC_LOG
                sWarning()<<":local timeout forced, url:"<<this->response->request_url;
#endif
            }
        }
        //gera crash
        //if(this->reply!=nullptr)
        //    this->reply->abort();
        this->reply=nullptr;
        emit this->callback(QVariant());
    };

    void onReplyDelete(){
        this->reply=nullptr;
    };

    void onReplyProgressUpload(qint64 bytesSent, qint64 bytesTotal){
        Q_UNUSED(bytesSent)
        Q_UNUSED(bytesTotal)
    }

    void onReplyProgressDownload(qint64 bytesReceived, qint64 bytesTotal)    {
        Q_UNUSED(bytesReceived)
        Q_UNUSED(bytesTotal)
        if(this->reply!=nullptr){
            const auto bytes=this->reply->read(bytesReceived);
            this->fileTemp.write(bytes);
        }
    }


    void onFinish(){
        this->timeout_stop();

        response->request_finish=QDateTime::currentDateTime();
        if(this->fileTemp.isOpen()){
            //auto fileName=this->fileTemp.fileName();
            this->fileTemp.close();
            this->fileDownload.close();
            QFile::remove(this->fileDownload.fileName());
            QFile::rename(this->fileTemp.fileName(), this->fileDownload.fileName());
            QFile::remove(this->fileTemp.fileName());
        }


        auto __nan=this->nam;
        this->nam=nullptr;

        auto __reply=this->reply;
        this->reply=nullptr;

        if(__reply!=nullptr){
            for(auto&v:__reply->rawHeaderPairs())
                response->responseHeader.insert(v.first, v.second);

            if(response->response_status_code!=QNetworkReply::TimeoutError){
                response->response_qt_status_code = __reply->error();
                response->response_body = __reply->readAll();
                response->response_status_code = __reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();;
                response->response_status_reason_phrase = __reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toByteArray();
                if(response->response_qt_status_code==QNetworkReply::ConnectionRefusedError)
                    response->response_status_code=502;
                if(response->response_status_code==0)
                    response->response_status_code=response->response_qt_status_code;
            }
        }
        if(__reply!=nullptr){
            __reply->abort();
            delete __reply;
        }
        if(__nan!=nullptr){
            delete __nan;
        }
        emit this->callback(QVariant());
    }

};

}
