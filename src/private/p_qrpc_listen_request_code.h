#pragma once

#include "../qrpc_global.h"
#include <QNetworkReply>

namespace QRpc {

//!
//! \brief The ListenRequestCode class
//!
class Q_RPC_EXPORT ListenRequestCode : public QObject
{
    Q_OBJECT
public:
    //!
    //! \brief The Code enum
    //!
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

    //!
    //! \brief ListenRequestCode
    //! \param parent
    //!
    Q_INVOKABLE explicit ListenRequestCode(QObject *parent = nullptr);

    ~ListenRequestCode();

    //!
    //! \brief operator =
    //! \param v
    //! \return
    //!
    ListenRequestCode&operator=(const QVariant&v);

    //!
    //! \brief emptyData
    //! \return
    //!
    virtual QVariant emptyData();

    //!
    //! \brief isOK
    //! \return
    //!
    bool isOK();

    //!
    //! \brief code
    //! \return
    //!
    int code();
    int code(const QVariant &code);
    ListenRequestCode&setCode();

    //!
    //! \brief reasonPhrase
    //! \return
    //!
    QString reasonPhrase();

    //!
    //! \brief reasonPhrase
    //! \param stateCode
    //! \return
    //!
    static const QString reasonPhrase(const int stateCode);

    //!
    //! \brief qt_network_error_phrase
    //! \param networkError
    //! \return
    //!
    static const QString qt_network_error_phrase(const QNetworkReply::NetworkError networkError);

    //!
    //! \brief setCode
    //! \param code
    //! \param phrase
    //! \return
    //!
    ListenRequestCode&setCode(QVariant code, const QString&phrase=QString());


    ListenRequestCode&setContinue(const QString&phrase=QString());
    ListenRequestCode&setSwitchingProtocols(const QString&phrase=QString());
    ListenRequestCode&setOK(const QString&phrase=QString());
    ListenRequestCode&setCreated(const QString&phrase=QString());
    ListenRequestCode&setAccepted(const QString&phrase=QString());
    ListenRequestCode&setNonAuthoritativeInformation(const QString&phrase=QString());
    ListenRequestCode&setNoContent(const QString&phrase=QString());
    ListenRequestCode&setResetContent(const QString&phrase=QString());
    ListenRequestCode&setPartialContent(const QString&phrase=QString());
    ListenRequestCode&setMultipleChoices(const QString&phrase=QString());
    ListenRequestCode&setMovedPermanently(const QString&phrase=QString());
    ListenRequestCode&setFound(const QString&phrase=QString());
    ListenRequestCode&setSeeOther(const QString&phrase=QString());
    ListenRequestCode&setNotModified(const QString&phrase=QString());
    ListenRequestCode&setUseProxy(const QString&phrase=QString());
    ListenRequestCode&setTemporaryRedirect(const QString&phrase=QString());
    ListenRequestCode&setBadRequest(const QString&phrase=QString());
    ListenRequestCode&setUnauthorized(const QString&phrase=QString());
    ListenRequestCode&setPaymentRequired(const QString&phrase=QString());
    ListenRequestCode&setForbidden(const QString&phrase=QString());
    ListenRequestCode&setNotFound(const QString&phrase=QString());
    ListenRequestCode&setMethodNotAllowed(const QString&phrase=QString());
    ListenRequestCode&setNotAcceptable(const QString&phrase=QString());
    ListenRequestCode&setProxyAuthenticationRequired(const QString&phrase=QString());
    ListenRequestCode&setRequestTimeout(const QString&phrase=QString());
    ListenRequestCode&setConflict(const QString&phrase=QString());
    ListenRequestCode&setGone(const QString&phrase=QString());
    ListenRequestCode&setLengthRequired(const QString&phrase=QString());
    ListenRequestCode&setPreconditionFailed(const QString&phrase=QString());
    ListenRequestCode&setPayloadTooLarge(const QString&phrase=QString());
    ListenRequestCode&setURITooLong(const QString&phrase=QString());
    ListenRequestCode&setUnsupportedMediaType(const QString&phrase=QString());
    ListenRequestCode&setRangeNotSatisfiable(const QString&phrase=QString());
    ListenRequestCode&setExpectationFailed(const QString&phrase=QString());
    ListenRequestCode&setUpgradeRequired(const QString&phrase=QString());
    ListenRequestCode&setInternalServerError(const QString&phrase=QString());
    ListenRequestCode&setNotImplemented(const QString&phrase=QString());
    ListenRequestCode&setBadGateway(const QString&phrase=QString());
    ListenRequestCode&setServiceUnavailable(const QString&phrase=QString());
    ListenRequestCode&setGatewayTimeout(const QString&phrase=QString());
    ListenRequestCode &setHTTPVersionNotSupported(const QString&phrase=QString());

};

}
