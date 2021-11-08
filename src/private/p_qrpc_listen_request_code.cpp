#include "./p_qrpc_listen_request_code.h"
#include "../qrpc_listen_request.h"

/**

https://restfulapi.net/http-status-codes/

REST Specific Status Codes
200 (OK)

It indicates that the REST API successfully carried out whatever action the client requested and that no more specific code in the 2xx series is appropriate.

Unlike the 204 status code, a 200 response should include a response body. The information returned with the response is dependent on the method used in the request, for example:

    GET an entity corresponding to the requested resource is sent in the response;
    HEAD the entity-header fields corresponding to the requested resource are sent in the response without any message-body;
    POST an entity describing or containing the result of the action;
    TRACE an entity containing the request message as received by the end server.

201 (Created)

A REST API responds with the 201 status code whenever a resource is created inside a collection. There may also be times when a new resource is created as a result of some controller action, in which case 201 would also be an appropriate response.

The newly created resource can be referenced by the URI(s) returned in the entity of the response, with the most specific URI for the resource given by a Location header field.

The origin server MUST create the resource before returning the 201 status code. If the action cannot be carried out immediately, the server SHOULD respond with a 202 (Accepted) response instead.
202 (Accepted)

A 202 response is typically used for actions that take a long while to process. It indicates that the request has been accepted for processing, but the processing has not been completed. The request might or might not be eventually acted upon, or even maybe disallowed when processing occurs.

Its purpose is to allow a server to accept a request for some other process (perhaps a batch-oriented process that is only run once per day) without requiring that the user agent’s connection to the server persist until the process is completed.

The entity returned with this response SHOULD include an indication of the request’s current status and either a pointer to a status monitor (job queue location) or some estimate of when the user can expect the request to be fulfilled.
203 Non-Authoritative Information
204 (No Content)

The 204 status code is usually sent out in response to a PUT, POST, or DELETE request when the REST API declines to send back any status message or representation in the response message’s body.

An API may also send 204 in conjunction with a GET request to indicate that the requested resource exists, but has no state representation to include in the body.

If the client is a user agent, it SHOULD NOT change its document view from that which caused the request to be sent. This response is primarily intended to allow input for actions to take place without causing a change to the user agent’s active document view. However, any new or updated metainformation SHOULD be applied to the document currently in the user agent’s dynamic view.

The 204 response MUST NOT include a message-body and thus is always terminated by the first empty line after the header fields.
300 Multiple Choices
301 (Moved Permanently)

The 301 status code indicates that the REST API’s resource model has been significantly redesigned, and a new permanent URI has been assigned to the client’s requested resource. The REST API should specify the new URI in the response’s Location header, and all future requests should be directed to the given URI.

You will hardly use this response code in your API as you can always use the API versioning for new API while retaining the old one.
302 (Found)

The HTTP response status code 302 Found is a common way of performing URL redirection. An HTTP response with this status code will additionally provide a URL in the Location header field. The user agent (e.g., a web browser) is invited by a response with this code to make a second. Otherwise identical, request to the new URL specified in the location field.

Many web browsers implemented this code in a manner that violated this standard, changing the request type of the new request to GET, regardless of the type employed in the original request (e.g., POST). RFC 1945 and RFC 2068 specify that the client is not allowed to change the method on the redirected request. The status codes 303 and 307 have been added for servers that wish to make unambiguously clear which kind of reaction is expected of the client.
303 (See Other)

A 303 response indicates that a controller resource has finished its work, but instead of sending a potentially unwanted response body, it sends the client the URI of a response resource. The response can be the URI of the temporary status message, or the URI to some already existing, more permanent, resource.

Generally speaking, the 303 status code allows a REST API to send a reference to a resource without forcing the client to download its state. Instead, the client may send a GET request to the value of the Location header.

The 303 response MUST NOT be cached, but the response to the second (redirected) request might be cacheable.
304 (Not Modified)

This status code is similar to 204 (“No Content”) in that the response body must be empty. The critical distinction is that 204 is used when there is nothing to send in the body, whereas 304 is used when the resource has not been modified since the version specified by the request headers If-Modified-Since or If-None-Match.

In such a case, there is no need to retransmit the resource since the client still has a previously-downloaded copy.

Using this saves bandwidth and reprocessing on both the server and client, as only the header data must be sent and received in comparison to the entirety of the page being re-processed by the server, then sent again using more bandwidth of the server and client.
307 (Temporary Redirect)

A 307 response indicates that the REST API is not going to process the client’s request. Instead, the client should resubmit the request to the URI specified by the response message’s Location header. However, future requests should still use the original URI.

A REST API can use this status code to assign a temporary URI to the client’s requested resource. For example, a 307 response can be used to shift a client request over to another host.

The temporary URI SHOULD be given by the Location field in the response. Unless the request method was HEAD, the entity of the response SHOULD contain a short hypertext note with a hyperlink to the new URI(s). If the 307 status code is received in response to a request other than GET or HEAD, the user agent MUST NOT automatically redirect the request unless it can be confirmed by the user, since this might change the conditions under which the request was issued.
400 (Bad Request)

400 is the generic client-side error status, used when no other 4xx error code is appropriate. Errors can be like malformed request syntax, invalid request message parameters, or deceptive request routing etc.

The client SHOULD NOT repeat the request without modifications.

401 (Unauthorized)

A 401 error response indicates that the client tried to operate on a protected resource without providing the proper authorization. It may have provided the wrong credentials or none at all. The response must include a WWW-Authenticate header field containing a challenge applicable to the requested resource.

The client MAY repeat the request with a suitable Authorization header field. If the request already included Authorization credentials, then the 401 response indicates that authorization has been refused for those credentials. If the 401 response contains the same challenge as the prior response, and the user agent has already attempted authentication at least once, then the user SHOULD be presented the entity that was given in the response, since that entity might include relevant diagnostic information.
403 (Forbidden)

A 403 error response indicates that the client’s request is formed correctly, but the REST API refuses to honor it, i.e., the user does not have the necessary permissions for the resource. A 403 response is not a case of insufficient client credentials; that would be 401 (“Unauthorized”).

Authentication will not help, and the request SHOULD NOT be repeated. Unlike a 401 Unauthorized response, authenticating will make no difference.
404 (Not Found)

The 404 error status code indicates that the REST API can’t map the client’s URI to a resource but may be available in the future. Subsequent requests by the client are permissible.

No indication is given of whether the condition is temporary or permanent. The 410 (Gone) status code SHOULD be used if the server knows, through some internally configurable mechanism, that an old resource is permanently unavailable and has no forwarding address. This status code is commonly used when the server does not wish to reveal exactly why the request has been refused, or when no other response is applicable.
405 (Method Not Allowed)

The API responds with a 405 error to indicate that the client tried to use an HTTP method that the resource does not allow. For instance, a read-only resource could support only GET and HEAD, while a controller resource might allow GET and POST, but not PUT or DELETE.

A 405 response must include the Allow header, which lists the HTTP methods that the resource supports. For example:

Allow: GET, POST

406 (Not Acceptable)

The 406 error response indicates that the API is not able to generate any of the client’s preferred media types, as indicated by the Accept request header. For example, a client request for data formatted as application/xml will receive a 406 response if the API is only willing to format data as application/json.

If the response could be unacceptable, a user agent SHOULD temporarily stop receipt of more data and query the user for a decision on further actions.
412 (Precondition Failed)

The 412 error response indicates that the client specified one or more preconditions in its request headers, effectively telling the REST API to carry out its request only if certain conditions were met. A 412 response indicates that those conditions were not met, so instead of carrying out the request, the API sends this status code.
415 (Unsupported Media Type)

The 415 error response indicates that the API is not able to process the client’s supplied media type, as indicated by the Content-Type request header. For example, a client request including data formatted as application/xml will receive a 415 response if the API is only willing to process data formatted as application/json.

For example, the client uploads an image as image/svg+xml, but the server requires that images use a different format.
500 (Internal Server Error)

500 is the generic REST API error response. Most web frameworks automatically respond with this response status code whenever they execute some request handler code that raises an exception.

A 500 error is never the client’s fault, and therefore, it is reasonable for the client to retry the same request that triggered this response and hope to get a different response.

The API response is the generic error message, given when an unexpected condition was encountered and no more specific message is suitable.
501 (Not Implemented)

*/

namespace QRpc {


static const QHash<int,QString>&makeStateCodes(){
    static QHash<int,QString> state_codes;

    //https://restfulapi.net/http-status-codes/
    //REST Specific Status Codes
    state_codes.insert(200, "(OK)");
    state_codes.insert(201, "(Created)");
    state_codes.insert(202, "(Accepted)");
    state_codes.insert(203, "(Non-Authoritative Information)");
    state_codes.insert(204, "(No Content)");
    state_codes.insert(300, "(Multiple Choices)");
    state_codes.insert(301, "(Moved Permanently)");
    state_codes.insert(302, "(Found)");
    state_codes.insert(303, "(See Other)");
    state_codes.insert(304, "(Not Modified)");
    state_codes.insert(307, "(Temporary Redirect)");
    state_codes.insert(400, "(Bad Request)");
    state_codes.insert(401, "(Unauthorized)");
    state_codes.insert(403, "(Forbidden)");
    state_codes.insert(404, "(Not Found)");
    state_codes.insert(405, "(Method Not Allowed)");
    state_codes.insert(406, "(Not Acceptable)");
    state_codes.insert(412, "(Precondition Failed)");
    state_codes.insert(415, "(Unsupported Media Type)");
    state_codes.insert(500, "(Internal Server Error)");
    state_codes.insert(501, "(Not Implemented)");
    state_codes.insert(502, "(Bad Gateway Error)");

    return state_codes;
};

static const auto&stateCodes=makeStateCodes();


static QMap<QNetworkReply::NetworkError, QString>&makeStaticError(){
    static QMap<QNetworkReply::NetworkError, QString> staticError;

    if(staticError.isEmpty()){
        staticError.insert(QNetworkReply::ConnectionRefusedError           , qsl("ConnectionRefusedError           ").trimmed());
        staticError.insert(QNetworkReply::RemoteHostClosedError            , qsl("RemoteHostClosedError            ").trimmed());
        staticError.insert(QNetworkReply::HostNotFoundError                , qsl("HostNotFoundError                ").trimmed());
        staticError.insert(QNetworkReply::TimeoutError                     , qsl("TimeoutError                     ").trimmed());
        staticError.insert(QNetworkReply::OperationCanceledError           , qsl("OperationCanceledError           ").trimmed());
        staticError.insert(QNetworkReply::SslHandshakeFailedError          , qsl("SslHandshakeFailedError          ").trimmed());
        staticError.insert(QNetworkReply::TemporaryNetworkFailureError     , qsl("TemporaryNetworkFailureError     ").trimmed());
        staticError.insert(QNetworkReply::NetworkSessionFailedError        , qsl("NetworkSessionFailedError        ").trimmed());
        staticError.insert(QNetworkReply::BackgroundRequestNotAllowedError , qsl("BackgroundRequestNotAllowedError ").trimmed());
        staticError.insert(QNetworkReply::TooManyRedirectsError            , qsl("TooManyRedirectsError            ").trimmed());
        staticError.insert(QNetworkReply::InsecureRedirectError            , qsl("InsecureRedirectError            ").trimmed());
        staticError.insert(QNetworkReply::UnknownNetworkError              , qsl("UnknownNetworkError              ").trimmed());
        staticError.insert(QNetworkReply::ProxyConnectionRefusedError      , qsl("ProxyConnectionRefusedError      ").trimmed());
        staticError.insert(QNetworkReply::ProxyConnectionClosedError       , qsl("ProxyConnectionClosedError       ").trimmed());
        staticError.insert(QNetworkReply::ProxyNotFoundError               , qsl("ProxyNotFoundError               ").trimmed());
        staticError.insert(QNetworkReply::ProxyTimeoutError                , qsl("ProxyTimeoutError                ").trimmed());
        staticError.insert(QNetworkReply::ProxyAuthenticationRequiredError , qsl("ProxyAuthenticationRequiredError ").trimmed());
        staticError.insert(QNetworkReply::UnknownProxyError                , qsl("UnknownProxyError                ").trimmed());
        staticError.insert(QNetworkReply::ContentAccessDenied              , qsl("ContentAccessDenied              ").trimmed());
        staticError.insert(QNetworkReply::ContentOperationNotPermittedError, qsl("ContentOperationNotPermittedError").trimmed());
        staticError.insert(QNetworkReply::ContentNotFoundError             , qsl("ContentNotSFoundError             ").trimmed());
        staticError.insert(QNetworkReply::AuthenticationRequiredError      , qsl("AuthenticationRequiredError      ").trimmed());
        staticError.insert(QNetworkReply::ContentReSendError               , qsl("ContentReSendError               ").trimmed());
        staticError.insert(QNetworkReply::ContentConflictError             , qsl("ContentConflictError             ").trimmed());
        staticError.insert(QNetworkReply::ContentGoneError                 , qsl("ContentGoneError                 ").trimmed());
        staticError.insert(QNetworkReply::UnknownContentError              , qsl("UnknownContentError              ").trimmed());
        staticError.insert(QNetworkReply::ProtocolUnknownError             , qsl("ProtocolUnknownError             ").trimmed());
        staticError.insert(QNetworkReply::ProtocolInvalidOperationError    , qsl("ProtocolInvalidOperationError    ").trimmed());
        staticError.insert(QNetworkReply::ProtocolFailure                  , qsl("ProtocolFailure                  ").trimmed());
        staticError.insert(QNetworkReply::InternalServerError              , qsl("InternalServerError              ").trimmed());
        staticError.insert(QNetworkReply::OperationNotImplementedError     , qsl("OperationNotImplementedError     ").trimmed());
        staticError.insert(QNetworkReply::ServiceUnavailableError          , qsl("ServiceUnavailableError          ").trimmed());
        staticError.insert(QNetworkReply::UnknownServerError               , qsl("UnknownServerError               ").trimmed());
    }

    return staticError;
};
const static auto staticErrors=makeStaticError();

QRPCListenRequestCode::QRPCListenRequestCode(QObject *parent) : QObject(parent)
{
    if(parent==nullptr){
        qFatal("invalid object");
    }
}

QRPCListenRequestCode::~QRPCListenRequestCode()
{

}

QRPCListenRequestCode &QRPCListenRequestCode::operator=(const QVariant &v)
{
    QVariant code;
    QVariant phrase;
    if(v.typeId()==QMetaType::QVariantMap || v.typeId()==QMetaType::QVariantHash){
        auto vv=v.toHash();
        code=vv.value(qsl("code")).toInt();
        phrase=vv.value(qsl("phrase")).toString();
    }
    else {
        code=v.toInt();
    }
    if(code.toInt()>0)
        this->setCode(code.toInt(), phrase.toByteArray());
    else
        this->setOK();
    return*this;
}

QVariant QRPCListenRequestCode::emptyData()
{
    return {};
}

bool QRPCListenRequestCode::isOK()
{
    auto code=this->code(ssOK);
    return code==ssOK;
}

int QRPCListenRequestCode::code()
{
    auto parent=this->parent();
    auto request=(parent==nullptr)?nullptr:dynamic_cast<QRPCListenRequest*>(parent);
    if(request==nullptr){
        qFatal("invalid object");
    }
    return request->responseCode();
}

int QRPCListenRequestCode::code(QVariant code)
{
    auto parent=this->parent();
    auto request=(parent==nullptr)?nullptr:dynamic_cast<QRPCListenRequest*>(parent);
    if(request==nullptr){
        qFatal("invalid object");
    }
    return request->responseCode(code.toInt());
}

QString QRPCListenRequestCode::reasonPhrase()
{
    return stateCodes.value(this->code());
}

const QString QRPCListenRequestCode::reasonPhrase(const int stateCode)
{
    return stateCodes.value(stateCode);
}

const QString QRPCListenRequestCode::qt_network_error_phrase(const QNetworkReply::NetworkError networkError)
{
    return staticErrors.value(networkError);
}

QRPCListenRequestCode &QRPCListenRequestCode::setCode(QVariant code, const QString&phrase)
{
    auto parent=this->parent();
    auto request=(parent==nullptr)?nullptr:dynamic_cast<QRPCListenRequest*>(parent);
    if(request==nullptr){
        qFatal("invalid object");
    }
    else{
        request->setResponseCode(code.toInt());
        request->setResponsePhrase(phrase.toUtf8());
    }
    return*this;
}

QRPCListenRequestCode &QRPCListenRequestCode::setContinue(const QString&phrase)
{
    return this->setCode(ssContinue, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setSwitchingProtocols(const QString&phrase)
{
    return this->setCode(ssSwitchingProtocols, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setOK(const QString&phrase)
{
    return this->setCode(ssOK, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setCreated(const QString&phrase)
{
    return this->setCode(ssCreated, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setAccepted(const QString&phrase)
{
    return this->setCode(ssAccepted, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setNonAuthoritativeInformation(const QString&phrase)
{
    return this->setCode(ssNonAuthoritativeInformation, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setNoContent(const QString&phrase)
{
    return this->setCode(ssNoContent, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setResetContent(const QString&phrase)
{
    return this->setCode(ssResetContent, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setPartialContent(const QString&phrase)
{
    return this->setCode(ssPartialContent, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setMultipleChoices(const QString&phrase)
{
    return this->setCode(ssMultipleChoices, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setMovedPermanently(const QString&phrase)
{
    return this->setCode(ssMovedPermanently, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setFound(const QString&phrase)
{
    return this->setCode(ssFound, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setSeeOther(const QString&phrase)
{
    return this->setCode(ssSeeOther, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setNotModified(const QString&phrase)
{
    return this->setCode(ssNotModified, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setUseProxy(const QString&phrase)
{
    return this->setCode(ssUseProxy, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setTemporaryRedirect(const QString&phrase)
{
    return this->setCode(ssTemporaryRedirect, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setBadRequest(const QString&phrase)
{
    return this->setCode(ssBadRequest, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setUnauthorized(const QString&phrase)
{
    return this->setCode(ssUnauthorized, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setPaymentRequired(const QString&phrase)
{
    return this->setCode(ssPaymentRequired, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setForbidden(const QString&phrase)
{
    return this->setCode(ssPaymentRequired, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setNotFound(const QString&phrase)
{
    return this->setCode(ssNotFound, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setMethodNotAllowed(const QString&phrase)
{
    return this->setCode(ssMethodNotAllowed, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setNotAcceptable(const QString&phrase)
{
    return this->setCode(ssNotAcceptable, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setProxyAuthenticationRequired(const QString&phrase)
{
    return this->setCode(ssProxyAuthenticationRequired, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setRequestTimeout(const QString&phrase)
{
    return this->setCode(ssRequestTimeout, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setConflict(const QString&phrase)
{
    return this->setCode(ssConflict, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setGone(const QString&phrase)
{
    return this->setCode(ssGone, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setLengthRequired(const QString&phrase)
{
    return this->setCode(ssLengthRequired, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setPreconditionFailed(const QString&phrase)
{
    return this->setCode(ssPreconditionFailed, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setPayloadTooLarge(const QString&phrase)
{
    return this->setCode(ssPayloadTooLarge, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setURITooLong(const QString&phrase)
{
    return this->setCode(ssURITooLong, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setUnsupportedMediaType(const QString&phrase)
{
    return this->setCode(ssURITooLong, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setRangeNotSatisfiable(const QString&phrase)
{
    return this->setCode(ssRangeNotSatisfiable, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setExpectationFailed(const QString&phrase)
{
    return this->setCode(ssRangeNotSatisfiable, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setUpgradeRequired(const QString&phrase)
{
    return this->setCode(ssUpgradeRequired, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setInternalServerError(const QString&phrase)
{
    return this->setCode(ssInternalServerError, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setNotImplemented(const QString&phrase)
{
    return this->setCode(ssNotImplemented, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setBadGateway(const QString&phrase)
{
    return this->setCode(ssBadGateway, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setServiceUnavailable(const QString&phrase)
{
    return this->setCode(ssServiceUnavailable, phrase);
}

QRPCListenRequestCode &QRPCListenRequestCode::setGatewayTimeout(const QString&phrase)
{
    return this->setCode(ssGatewayTimeout, phrase);
}

QRPCListenRequestCode&QRPCListenRequestCode::setHTTPVersionNotSupported(const QString&phrase)
{
    return this->setCode(ssHTTPVersionNotSupported, phrase);
}

}
