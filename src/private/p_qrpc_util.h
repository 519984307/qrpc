#pragma once

#include <QObject>
#include <QVariant>
#include <QString>
#include <QByteArray>
#include <QUuid>
#include <QUrl>
#include "./qrpc_macro.h"

namespace QRpc {
namespace Util {
    const QString routeParser(const QVariant&vRouteBase);

    const QByteArray routeExtractMethod(const QString&routeBase);

    const QByteArray routeExtract(const QString&routeBase);

    const QString headerFormatName(const QString&name);

    const QString parseQueryItem(const QVariant&v);

    const QVariantHash toMapResquest(int method, const QVariant&request_url, const QString&request_body, const QVariantHash &request_parameters, const QString&response_body, const QVariantHash&request_header, const QDateTime&request_start, const QDateTime&request_finish);

};

}
