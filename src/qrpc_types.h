#pragma once

#include <QObject>
#include <QHash>
#include <QMap>
#include <QVariant>
#include <QString>
#include <QTemporaryFile>
#include <QSslConfiguration>
#include <QSslCertificate>

namespace QRpc {

    static const auto ContentTypeName=QString("Content-Type");
    static const auto ContentDispositionName=QString("Content-Disposition");

    enum ContentType {
          AppNone=1
        , AppXML=AppNone*2
        , AppJson=AppXML*2
        , AppCBOR=AppJson*2
        , AppOctetStream=AppCBOR*2
        , AppXwwwForm=AppOctetStream*2
        , AppPNG=AppXwwwForm*2
        , AppJPGE=AppPNG*2
        , AppGIF=AppJPGE*2
        , AppPDF=AppGIF*2
        , AppTXT=AppPDF*2
        , AppHTML=AppTXT*2
        , AppCSS=AppHTML*2
        , AppJS=AppCSS*2
        , AppSVG=AppJS*2
        , AppWOFF=AppSVG*2
        , AppWOFF2=AppWOFF*2
        , AppTTF=AppWOFF2*2
        , AppEOT=AppTTF*2
        , AppOTF=AppEOT*2
    };

    enum AuthorizationType{//https://developer.mozilla.org/en-US/docs/Web/HTTP/Authentication#Authentication_schemes
        Basic// (see RFC 7617, base64-encoded credentials. See below for more information.),
        ,Bearer// (see RFC 6750, bearer tokens to access OAuth 2.0-protected resources),
        ,Digest// (see RFC 7616, only md5 hashing is supported in Firefox, see bug 472823 for SHA encryption support),
        ,HOBA// (see RFC 7486, Section 3, HTTP Origin-Bound Authentication, digital-signature-based),
        ,Mutual// (see RFC 8120),
        ,AWS4_HMAC_SHA256// (see AWS docs).
        ,Service
    };

    static const QHash<ContentType, QString> ___QRPCContentTypeHeaderTypeToHeader(){
        QHash<ContentType,QString> __return;
        __return.insert(AppNone, "application/x-www-form-urlencoded");
        __return.insert(AppXML, "application/xml");
        __return.insert(AppJson, "application/json");
        __return.insert(AppCBOR, "application/cbor");
        __return.insert(AppOctetStream, "application/octet-stream");
        __return.insert(AppXwwwForm, "application/x-www-form-urlencoded");
        __return.insert(AppPNG, "image/png");
        __return.insert(AppJPGE, "image/jpg");
        __return.insert(AppGIF, "image/gif");
        __return.insert(AppPDF, "application/pdf");
        __return.insert(AppTXT, "text/plain; charset=utf-8");
        __return.insert(AppHTML, "text/html; charset=utf-8");
        __return.insert(AppCSS, "text/css");
        __return.insert(AppJS, "text/javascript");
        __return.insert(AppSVG, "image/svg+xml");
        __return.insert(AppWOFF, "font/woff");
        __return.insert(AppWOFF2, "font/woff2");
        __return.insert(AppTTF, "application/x-font-ttf");
        __return.insert(AppEOT, "application/vnd.ms-fontobject");
        __return.insert(AppOTF, "application/font-otf");
        return __return;
    }

    const auto QRPCContentTypeHeaderTypeToHeader=___QRPCContentTypeHeaderTypeToHeader();

    static const QHash<QString, ContentType> ___ContentTypeHeaderToHeaderType(){
        QHash<QString, ContentType> __return;
        __return.insert("application/x-www-form-urlencoded" , AppNone);
        __return.insert("application/xml" , AppXML);
        __return.insert("application/json", AppJson);
        __return.insert("application/cbor", AppCBOR);
        __return.insert("application/octet-stream", AppOctetStream);
        __return.insert("application/x-www-form-urlencoded" , AppXwwwForm);
        __return.insert("image/png" , AppPNG);
        __return.insert("image/jpg" , AppJPGE);
        __return.insert("image/gif" , AppGIF);
        __return.insert("application/pdf" , AppPDF);
        __return.insert("text/plain; charset=utf-8" , AppTXT);
        __return.insert("text/html; charset=utf-8", AppHTML);
        __return.insert("text/css" , AppCSS);
        __return.insert("text/javascript", AppJS);
        __return.insert("image/svg+xml" , AppSVG);
        __return.insert("font/woff", AppWOFF);
        __return.insert("font/woff2" , AppWOFF2);
        __return.insert("application/x-font-ttf" , AppTTF);
        __return.insert("application/vnd.ms-fontobject" , AppEOT);
        __return.insert("application/font-otf" , AppOTF);
        return __return;
    }

    const auto ContentTypeHeaderToHeaderType=___ContentTypeHeaderToHeaderType();

    static const QHash<ContentType, QString> ___QRPCContentTypeHeaderToExtension(){
        QHash<ContentType,QString> __return;
        __return.insert(AppXML, "xml");
        __return.insert(AppJson, "json");
        __return.insert(AppCBOR, "cbor");
        __return.insert(AppPNG, "png");
        __return.insert(AppJPGE, "jpg");
        __return.insert(AppGIF, "gif");
        __return.insert(AppPDF, "pdf");
        __return.insert(AppTXT, "txt");
        __return.insert(AppHTML, "html");
        __return.insert(AppCSS, "css");
        __return.insert(AppJS, "js");
        __return.insert(AppSVG, "svg");
        __return.insert(AppWOFF, "woff");
        __return.insert(AppWOFF2, "woff2");
        __return.insert(AppTTF, "ttf");
        __return.insert(AppEOT, "eot");
        __return.insert(AppOTF, "otf");

        return __return;
    }

    const auto QRPCContentTypeHeaderToExtension=___QRPCContentTypeHeaderToExtension();

    static const QHash<QString, ContentType> ___QRPCContentTypeExtensionToHeader(){
        QHash<QString, ContentType> __return;
        __return.insert("", AppOctetStream);
        __return.insert("xml", AppXML);
        __return.insert("json", AppJson);
        __return.insert("cbor", AppCBOR);
        __return.insert("png", AppPNG);
        __return.insert("jpg", AppJPGE);
        __return.insert("gif", AppGIF);
        __return.insert("pdf", AppPDF);
        __return.insert("txt", AppTXT);
        __return.insert("html", AppHTML);
        __return.insert("css", AppCSS);
        __return.insert("js", AppJS);
        __return.insert("svg", AppSVG);
        __return.insert("woff", AppWOFF);
        __return.insert("woff2", AppWOFF2);
        __return.insert("ttf", AppTTF);
        __return.insert("eot", AppEOT);
        __return.insert("otf", AppOTF);
        return __return;
    }

    const auto QRPCContentTypeExtensionToHeader=___QRPCContentTypeExtensionToHeader();


    enum QRPCRequestMethod {Head=1, Get=2, Post=4, Put=8, Delete=16, Options=32, MaxMethod=Options};

    static const QHash<int,QString> ___QRPCRequestMethodName(){
        QHash<int, QString> r;
        r.insert(Head, QT_STRINGIFY2(Head));
        r.insert(Get, QT_STRINGIFY2(Get));
        r.insert(Post, QT_STRINGIFY2(Post));
        r.insert(Put, QT_STRINGIFY2(Put));
        r.insert(Delete, QT_STRINGIFY2(Delete));
        r.insert(Options, QT_STRINGIFY2(Options));
        return r;
    }
    static const auto QRPCRequestMethodName = ___QRPCRequestMethodName();
    static const auto QRPCRequestMethodNameList = ___QRPCRequestMethodName().values();

    static const QHash<QString,QRPCRequestMethod> ___QRPCRequestMethodType(){
        QHash<QString,QRPCRequestMethod> r;
        r.insert(QT_STRINGIFY2(head), Head);
        r.insert(QT_STRINGIFY2(get), Get);
        r.insert(QT_STRINGIFY2(post), Post);
        r.insert(QT_STRINGIFY2(put), Put);
        r.insert(QT_STRINGIFY2(delete), Delete);
        r.insert(QT_STRINGIFY2(options), Options);

        r.insert(QT_STRINGIFY2(Head), Head);
        r.insert(QT_STRINGIFY2(Get), Get);
        r.insert(QT_STRINGIFY2(Post), Post);
        r.insert(QT_STRINGIFY2(Put), Put);
        r.insert(QT_STRINGIFY2(Delete), Delete);
        r.insert(QT_STRINGIFY2(Options), Options);
        return r;
    }
    static const auto QRPCRequestMethodType=___QRPCRequestMethodType();
    enum QRPCProtocol {TcpSocket=1, UdpSocket=2, WebSocket=4, Mqtt=8, Amqp=16, Http=32, Https=64, DataBase=128, Kafka=256, LocalSocket=612};
    Q_DECLARE_FLAGS(QRPCProtocols, QRPCProtocol)
    static const QHash<QString,QRPCProtocol> ___QRPCProtocolType(){
        QHash<QString,QRPCProtocol> r;
        r.insert(QT_STRINGIFY2(RPC), QRPCProtocol(0));
        r.insert(QT_STRINGIFY2(TcpSocket), TcpSocket);
        r.insert(QT_STRINGIFY2(UdpSocket), UdpSocket);
        r.insert(QT_STRINGIFY2(WebSocket), WebSocket);
        r.insert(QT_STRINGIFY2(Mqtt), Mqtt);
        r.insert(QT_STRINGIFY2(Amqp), Amqp);
        r.insert(QT_STRINGIFY2(Http), Http);
        r.insert(QT_STRINGIFY2(Https), Https);
        r.insert(QT_STRINGIFY2(DataBase), DataBase);
        r.insert(QT_STRINGIFY2(Kafka), Kafka);
        r.insert(QT_STRINGIFY2(LocalSocket), LocalSocket);

        r.insert(QT_STRINGIFY2(tcp), TcpSocket);
        r.insert(QT_STRINGIFY2(udp), UdpSocket);
        r.insert(QT_STRINGIFY2(wss), WebSocket);
        r.insert(QT_STRINGIFY2(mqtt), Mqtt);
        r.insert(QT_STRINGIFY2(amqp), Amqp);
        r.insert(QT_STRINGIFY2(http), Http);
        r.insert(QT_STRINGIFY2(https), Https);
        r.insert(QT_STRINGIFY2(database), DataBase);
        r.insert(QT_STRINGIFY2(dataBase), DataBase);
        r.insert(QT_STRINGIFY2(kafka), Kafka);
        r.insert(QT_STRINGIFY2(LocalSocket), LocalSocket);
        return r;
    }

    static const QHash<QRPCProtocol, QString> ___QRPCProtocolName(){
        QHash<QRPCProtocol, QString> r;
        r.insert(QRPCProtocol(0), QT_STRINGIFY2(rpc));
        r.insert(TcpSocket, QT_STRINGIFY2(tcpsocket));
        r.insert(UdpSocket, QT_STRINGIFY2(udpsocket));
        r.insert(WebSocket, QT_STRINGIFY2(websocket));
        r.insert(Mqtt, QT_STRINGIFY2(mqtt));
        r.insert(Amqp, QT_STRINGIFY2(amqp));
        r.insert(Http, QT_STRINGIFY2(http));
        r.insert(Https, QT_STRINGIFY2(https));
        r.insert(DataBase, QT_STRINGIFY2(database));
        r.insert(LocalSocket, QT_STRINGIFY2(localsocket));
        r.insert(Kafka, QT_STRINGIFY2(kafka));
        return r;
    }

    static const QHash<QRPCProtocol, QString> ___QRPCProtocolUrlName(){
        QHash<QRPCProtocol, QString> r;
        r.insert(QRPCProtocol(0), QT_STRINGIFY2(rpc));
        r.insert(TcpSocket, QT_STRINGIFY2(tcp));
        r.insert(UdpSocket, QT_STRINGIFY2(udp));
        r.insert(WebSocket, QT_STRINGIFY2(wss));
        r.insert(Mqtt, QT_STRINGIFY2(mqtt));
        r.insert(Amqp, QT_STRINGIFY2(amqp));
        r.insert(Http, QT_STRINGIFY2(http));
        r.insert(Https, QT_STRINGIFY2(https));
        r.insert(DataBase, QT_STRINGIFY2(database));
        r.insert(LocalSocket, QT_STRINGIFY2(localsocket));
        r.insert(Kafka, QT_STRINGIFY2(dpkafka));
        return r;
    }

    static const auto QRPCProtocolName=___QRPCProtocolName();
    static const auto QRPCProtocolType=___QRPCProtocolType();
    static const auto QRPCProtocolUrlName=___QRPCProtocolUrlName();
    static const auto rpcProtocolMin=QRPCProtocol(1);
    static const auto rpcProtocolMax=LocalSocket;
    class QRPCListen;
    class QRPCListenProtocol;


    static const QHash<int, QString> ___QSslSslProtocolToName(){
        static QHash<int, QString> r;
        r.insert(QSsl::SslProtocol::SslV3, QT_STRINGIFY2(SslV3));
        r.insert(QSsl::SslProtocol::SslV2, QT_STRINGIFY2(SslV2));
        r.insert(QSsl::SslProtocol::TlsV1_0, QT_STRINGIFY2(TlsV1_0));
        r.insert(QSsl::SslProtocol::TlsV1_1, QT_STRINGIFY2(TlsV1_1));
        r.insert(QSsl::SslProtocol::TlsV1_2, QT_STRINGIFY2(TlsV1_2));
        r.insert(QSsl::SslProtocol::AnyProtocol, QT_STRINGIFY2(AnyProtocol));
        r.insert(QSsl::SslProtocol::TlsV1SslV3, QT_STRINGIFY2(TlsV1SslV3));
        r.insert(QSsl::SslProtocol::SecureProtocols, QT_STRINGIFY2(SecureProtocols));
        r.insert(QSsl::SslProtocol::TlsV1_0OrLater, QT_STRINGIFY2(TlsV1_0OrLater));
        r.insert(QSsl::SslProtocol::TlsV1_1OrLater, QT_STRINGIFY2(TlsV1_1OrLater));
        r.insert(QSsl::SslProtocol::TlsV1_2OrLater, QT_STRINGIFY2(TlsV1_2OrLater));
        r.insert(QSsl::SslProtocol::DtlsV1_0, QT_STRINGIFY2(DtlsV1_0));
        r.insert(QSsl::SslProtocol::DtlsV1_0OrLater, QT_STRINGIFY2(DtlsV1_0OrLater));
        r.insert(QSsl::SslProtocol::DtlsV1_2, QT_STRINGIFY2(DtlsV1_2));
        r.insert(QSsl::SslProtocol::DtlsV1_2OrLater, QT_STRINGIFY2(DtlsV1_2OrLater));
        r.insert(QSsl::SslProtocol::TlsV1_3, QT_STRINGIFY2(TlsV1_3));
        r.insert(QSsl::SslProtocol::TlsV1_3OrLater, QT_STRINGIFY2(TlsV1_3OrLater));

        return r;
    }

    static const auto QSslSslProtocolToName=___QSslSslProtocolToName();


    static const QHash<QString, QSsl::SslProtocol> ___QSslProtocolNameToProtocol(){
        static QHash<QString, QSsl::SslProtocol> r;
        r.insert( QString(QT_STRINGIFY2(SslV3)).toLower(), QSsl::SslProtocol::SslV3);
        r.insert( QString(QT_STRINGIFY2(SslV2)).toLower(), QSsl::SslProtocol::SslV2);
        r.insert( QString(QT_STRINGIFY2(TlsV1_0)).toLower(), QSsl::SslProtocol::TlsV1_0);
        r.insert( QString(QT_STRINGIFY2(TlsV1_1)).toLower(), QSsl::SslProtocol::TlsV1_1);
        r.insert( QString(QT_STRINGIFY2(TlsV1_2)).toLower(), QSsl::SslProtocol::TlsV1_2);
        r.insert( QString(QT_STRINGIFY2(AnyProtocol)).toLower(), QSsl::SslProtocol::AnyProtocol);
        r.insert( QString(QT_STRINGIFY2(TlsV1SslV3)).toLower(), QSsl::SslProtocol::TlsV1SslV3);
        r.insert( QString(QT_STRINGIFY2(SecureProtocols)).toLower(), QSsl::SslProtocol::SecureProtocols);
        r.insert( QString(QT_STRINGIFY2(TlsV1_0OrLater)).toLower(), QSsl::SslProtocol::TlsV1_0OrLater);
        r.insert( QString(QT_STRINGIFY2(TlsV1_1OrLater)).toLower(), QSsl::SslProtocol::TlsV1_1OrLater);
        r.insert( QString(QT_STRINGIFY2(TlsV1_2OrLater)).toLower(), QSsl::SslProtocol::TlsV1_2OrLater);
        r.insert( QString(QT_STRINGIFY2(DtlsV1_0)).toLower(), QSsl::SslProtocol::DtlsV1_0);
        r.insert( QString(QT_STRINGIFY2(DtlsV1_0OrLater)).toLower(), QSsl::SslProtocol::DtlsV1_0OrLater);
        r.insert( QString(QT_STRINGIFY2(DtlsV1_2)).toLower(), QSsl::SslProtocol::DtlsV1_2);
        r.insert( QString(QT_STRINGIFY2(DtlsV1_2OrLater)).toLower(), QSsl::SslProtocol::DtlsV1_2OrLater);
        r.insert( QString(QT_STRINGIFY2(TlsV1_3)).toLower(), QSsl::SslProtocol::TlsV1_3);
        r.insert( QString(QT_STRINGIFY2(TlsV1_3OrLater)).toLower(), QSsl::SslProtocol::TlsV1_3OrLater);

        return r;
    }

    static const auto QSslProtocolNameToProtocol=___QSslProtocolNameToProtocol();
}

Q_DECLARE_OPERATORS_FOR_FLAGS(QRpc::QRPCProtocols)
