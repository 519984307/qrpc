#pragma once

#include <QObject>
#include "./qrpc_global.h"

namespace QRpc {

class Q_RPC_EXPORT LastError : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit LastError(QObject *parent = nullptr);

    Q_INVOKABLE ~LastError();

    Q_INVOKABLE virtual QString nativeErrorCode() const;
    Q_INVOKABLE virtual QString text() const;
    Q_INVOKABLE virtual bool isValid() const;
    Q_INVOKABLE virtual void clear();

signals:
};

} // namespace QRpc
