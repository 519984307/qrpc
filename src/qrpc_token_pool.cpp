#include "./qrpc_token_pool.h"
#include <QMutex>
#include <QTimer>

namespace QRpc {

#define dPvt()\
    auto&p =*reinterpret_cast<TokenPoolPvt*>(this->p)

class TokenPoolPvt:public QObject{
public:
    QMutex mutex;
    QHash<QByteArray, QVariantHash> tokenMap;
    TokenPool*tokenPool=nullptr;
    explicit TokenPoolPvt(TokenPool*parent):QObject(parent){
        this->tokenPool=parent;
    }

    virtual ~TokenPoolPvt(){
    }
public:
    bool tokenCheck(const QByteArray &md5)
    {
        auto&p=*this;
        auto&vToken=p.tokenMap.value(md5);
        return !vToken.isEmpty();
    }
};

TokenPool::TokenPool(QObject *parent) : QThread(nullptr)
{
    this->p = new TokenPoolPvt(this);
    Q_UNUSED(parent)
    this->moveToThread(this);
}

QRpc::TokenPool::~TokenPool()
{
    dPvt();
    delete&p;
}

QVariantHash TokenPool::token(const QByteArray &md5) const
{
    dPvt();
    QMutexLocker locker(&p.mutex);
    return p.tokenMap.value(md5);
}

void TokenPool::run()
{
    emit this->tokenCharger();
    this->exec();
}

bool TokenPool::tokenCheck(const QByteArray &md5)
{
    dPvt();
    return p.tokenCheck(md5);
}

void TokenPool::tokenCheck(const QByteArray &md5, TokenPoolCallBack callback)
{
    dPvt();
    p.mutex.lock();
    auto isValid=p.tokenCheck(md5);
    auto&vToken=p.tokenMap.value(md5);
    p.mutex.unlock();
    QTimer::singleShot(1, this, [&isValid, &vToken, &callback](){ callback(isValid, vToken); });
}

void TokenPool::tokenInsert(const QByteArray &md5, QVariantHash &tokenPayload)
{
    dPvt();
    QMutexLocker locker(&p.mutex);
    p.tokenMap.insert(md5,tokenPayload);
}

void TokenPool::tokenRemove(const QByteArray &md5)
{
    dPvt();
    QMutexLocker locker(&p.mutex);
    p.tokenMap.remove(md5);
}

void TokenPool::tokenClear()
{
    dPvt();
    QMutexLocker locker(&p.mutex);
    p.tokenMap.clear();
}

}
