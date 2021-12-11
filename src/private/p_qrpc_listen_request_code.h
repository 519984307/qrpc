#pragma once

#include "./qrpc_global.h"
#include <QNetworkReply>

namespace QRpc {

class Q_RPC_EXPORT QRPCListenRequestCode : public QObject
{
    Q_OBJECT
public:

    enum Code{//https://developer.mozilla.org/pt-BR/docs/Web/HTTP/Status
        //Respostas de informação (100-199),
         ssContinue                     = 100
        ,ssSwitchingProtocols           = 101

        //Respostas de sucesso (200-299),
        ,ssOK                           = 200
        ,ssCreated                      = 201
        ,ssAccepted                     = 202
        ,ssNonAuthoritativeInformation  = 203
        ,ssNoContent                    = 204
        ,ssResetContent                 = 205
        ,ssPartialContent               = 206

        //Redirecionamentos (300-399)
        ,ssMultipleChoices              = 300
        ,ssMovedPermanently             = 301
        ,ssFound                        = 302
        ,ssSeeOther                     = 303
        ,ssNotModified                  = 304
        ,ssUseProxy                     = 305
        ,ssTemporaryRedirect            = 307

        //Erros do cliente (400-499)
        ,ssBadRequest                   = 400
        ,ssUnauthorized                 = 401
        ,ssPaymentRequired              = 402
        ,ssForbidden                    = 403
        ,ssNotFound                     = 404
        ,ssMethodNotAllowed             = 405
        ,ssNotAcceptable                = 406
        ,ssProxyAuthenticationRequired  = 407
        ,ssRequestTimeout               = 408
        ,ssConflict                     = 409
        ,ssGone                         = 410
        ,ssLengthRequired               = 411
        ,ssPreconditionFailed           = 412
        ,ssPayloadTooLarge              = 413
        ,ssURITooLong                   = 414
        ,ssUnsupportedMediaType         = 415
        ,ssRangeNotSatisfiable          = 416
        ,ssExpectationFailed            = 417
        ,ssUpgradeRequired              = 426

        //Erros do servidor (500-599).
        ,ssInternalServerError          = 500
        ,ssNotImplemented               = 501
        ,ssBadGateway                   = 502
        ,ssServiceUnavailable           = 503
        ,ssGatewayTimeout               = 504
        ,ssHTTPVersionNotSupported      = 505
    };


    /**
     * @brief QRPCListenRequestCode
     * @param parent
     */
    Q_INVOKABLE explicit QRPCListenRequestCode(QObject *parent = nullptr);

    /**
     * @brief ~QRPCListenRequestCode
     */
    Q_INVOKABLE ~QRPCListenRequestCode();

    /**
     * @brief operator =
     * @param v
     * @return
     */
    QRPCListenRequestCode&operator=(const QVariant&v);

    /**
     * @brief emptyData
     * @return
     */
    virtual QVariant emptyData();

    /**
     * @brief isOK
     * @return
     */
    bool isOK();

    /**
     * @brief code
     * @return
     */
    int code();
    int code(const QVariant &code);

    /**
     * @brief setCode
     * @return
     */
    QRPCListenRequestCode&setCode();

    /**
     * @brief reasonPhrase
     * @return
     */
    QString reasonPhrase();

    /**
     * @brief reasonPhrase
     * @param stateCode
     * @return
     */
    static const QString reasonPhrase(const int stateCode);

    /**
     * @brief qt_network_message
     * @param stateCode
     * @return
     */
    static const QString qt_network_error_phrase(const QNetworkReply::NetworkError networkError);

    QRPCListenRequestCode&setCode(QVariant code, const QString&phrase=QString());
    QRPCListenRequestCode&setContinue(const QString&phrase=QString());
    QRPCListenRequestCode&setSwitchingProtocols(const QString&phrase=QString());
    QRPCListenRequestCode&setOK(const QString&phrase=QString());
    QRPCListenRequestCode&setCreated(const QString&phrase=QString());
    QRPCListenRequestCode&setAccepted(const QString&phrase=QString());
    QRPCListenRequestCode&setNonAuthoritativeInformation(const QString&phrase=QString());
    QRPCListenRequestCode&setNoContent(const QString&phrase=QString());
    QRPCListenRequestCode&setResetContent(const QString&phrase=QString());
    QRPCListenRequestCode&setPartialContent(const QString&phrase=QString());
    QRPCListenRequestCode&setMultipleChoices(const QString&phrase=QString());
    QRPCListenRequestCode&setMovedPermanently(const QString&phrase=QString());
    QRPCListenRequestCode&setFound(const QString&phrase=QString());
    QRPCListenRequestCode&setSeeOther(const QString&phrase=QString());
    QRPCListenRequestCode&setNotModified(const QString&phrase=QString());
    QRPCListenRequestCode&setUseProxy(const QString&phrase=QString());
    QRPCListenRequestCode&setTemporaryRedirect(const QString&phrase=QString());
    QRPCListenRequestCode&setBadRequest(const QString&phrase=QString());
    QRPCListenRequestCode&setUnauthorized(const QString&phrase=QString());
    QRPCListenRequestCode&setPaymentRequired(const QString&phrase=QString());
    QRPCListenRequestCode&setForbidden(const QString&phrase=QString());
    QRPCListenRequestCode&setNotFound(const QString&phrase=QString());
    QRPCListenRequestCode&setMethodNotAllowed(const QString&phrase=QString());
    QRPCListenRequestCode&setNotAcceptable(const QString&phrase=QString());
    QRPCListenRequestCode&setProxyAuthenticationRequired(const QString&phrase=QString());
    QRPCListenRequestCode&setRequestTimeout(const QString&phrase=QString());
    QRPCListenRequestCode&setConflict(const QString&phrase=QString());
    QRPCListenRequestCode&setGone(const QString&phrase=QString());
    QRPCListenRequestCode&setLengthRequired(const QString&phrase=QString());
    QRPCListenRequestCode&setPreconditionFailed(const QString&phrase=QString());
    QRPCListenRequestCode&setPayloadTooLarge(const QString&phrase=QString());
    QRPCListenRequestCode&setURITooLong(const QString&phrase=QString());
    QRPCListenRequestCode&setUnsupportedMediaType(const QString&phrase=QString());
    QRPCListenRequestCode&setRangeNotSatisfiable(const QString&phrase=QString());
    QRPCListenRequestCode&setExpectationFailed(const QString&phrase=QString());
    QRPCListenRequestCode&setUpgradeRequired(const QString&phrase=QString());
    QRPCListenRequestCode&setInternalServerError(const QString&phrase=QString());
    QRPCListenRequestCode&setNotImplemented(const QString&phrase=QString());
    QRPCListenRequestCode&setBadGateway(const QString&phrase=QString());
    QRPCListenRequestCode&setServiceUnavailable(const QString&phrase=QString());
    QRPCListenRequestCode&setGatewayTimeout(const QString&phrase=QString());
    QRPCListenRequestCode &setHTTPVersionNotSupported(const QString&phrase=QString());

};

}
