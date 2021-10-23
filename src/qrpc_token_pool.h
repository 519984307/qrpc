#pragma once

#include "./qrpc_global.h"
#include <QObject>
#include <QThread>
#include <QVariant>
#include <QVariantHash>

namespace QRpc {

    typedef std::function<void(const bool isValid,const QVariantHash&vToken)> TokenPoolCallBack;
    class Q_RPC_EXPORT TokenPool : public QThread
    {
        Q_OBJECT
    public:
        Q_INVOKABLE explicit TokenPool(QObject *parent = nullptr);
        Q_INVOKABLE ~TokenPool();

        Q_DECLARE_INSTANCE(TokenPool)

        virtual QVariantHash token(const QByteArray&md5)const;

        void run() override;

        bool tokenCheck(const QByteArray&md5);

    public slots:
        void tokenCheck(const QByteArray&md5, QRpc::TokenPoolCallBack callback);
        void tokenInsert(const QByteArray&md5, QVariantHash&tokenPayload);
        void tokenRemove(const QByteArray&md5);
        void tokenClear();
    signals:
        void tokenCharger();
    private:
        void*p=nullptr;
    };

}
