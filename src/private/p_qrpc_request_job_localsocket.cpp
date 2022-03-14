#include "./p_qrpc_request_job_localsocket.h"

namespace QRpc {

RequestJobLocalSocket::RequestJobLocalSocket(QObject *parent):RequestJobProtocol(parent)
{
}

RequestJobLocalSocket::~RequestJobLocalSocket()
{
}

bool RequestJobLocalSocket::call(RequestJobResponse *response)
{
    this->response=response;
    if(m_socket!=nullptr)
        delete m_socket;
    m_socket = new QTcpSocket();
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    QObject::connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred), this, &RequestJobLocalSocket::onReplyError);
#else
    QObject::connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error), this, &RequestJobLocalSocket::onReplyError);
#endif
    QObject::connect(m_socket, &QTcpSocket::connected, this, &RequestJobLocalSocket::onConnected);
    QObject::connect(m_socket, &QTcpSocket::readChannelFinished, this, &RequestJobLocalSocket::onFinish);
    QUrl url(this->response->request_url.toUrl());
    const QString hostName=url.host(QUrl::FullyDecoded);
    const int port=url.port();
    m_socket->connectToHost(hostName, port);
    return true;
}

void RequestJobLocalSocket::onConnected()
{
    this->buffer.clear();
    if(!m_socket->waitForConnected(response->activityLimit)){
        response->response_qt_status_code=QNetworkReply::TimeoutError;
        response->response_status_code=501;
    }
    else{
        ListenRequest request(response->request_body);
        auto body=request.toJson();
        m_socket->write(body);
        m_socket->flush();
        if(!m_socket->waitForDisconnected(response->activityLimit)){
            response->response_qt_status_code=QNetworkReply::TimeoutError;
            response->response_status_code=501;
        }
        else{
            this->buffer.append(m_socket->readAll());
            if(request.fromJson(this->buffer)){
                response->response_qt_status_code=QNetworkReply::NoError;
                response->response_status_code=200;
                response->response_body=this->buffer;
            }
            m_socket->close();
        }
    }
    this->onFinish();
}

void RequestJobLocalSocket::onReplyError(QAbstractSocket::SocketError e)
{
    Q_UNUSED(e)
    if(e!=QAbstractSocket::SocketError::RemoteHostClosedError){
        response->response_qt_status_code=QNetworkReply::UnknownServerError;
        this->onFinish();

    }
}

void RequestJobLocalSocket::onFinish()
{
    ListenRequest request(this->buffer);
    if(!request.isValid()){
        response->response_qt_status_code=QNetworkReply::InternalServerError;
        response->response_status_code=404;
        response->response_body.clear();
    }
    else if(response->response_status_code!=QNetworkReply::TimeoutError){
        auto code=request.responseCode();
        response->response_qt_status_code = QNetworkReply::NetworkError(code==200?0:code);
        response->response_body = request.responseBody().toByteArray();
        response->response_status_code = request.responseCode();
        response->response_status_reason_phrase = request.responsePhrase();
        if(response->response_status_code==0)
            response->response_status_code=response->response_qt_status_code;
    }

    emit this->callback(QVariant());
}

}
