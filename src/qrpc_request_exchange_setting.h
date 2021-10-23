#pragma once

#include "./qstm_object.h"
#include "./qrpc_global.h"
#include "./qrpc_types.h"

namespace QRpc {

class Q_RPC_EXPORT QRPCRequestExchangeSetting: public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(QString      protocol         READ protocolName       WRITE setProtocol            )
    Q_PROPERTY(int          method           READ ___method          WRITE setMethod              )
    Q_PROPERTY(QString      vHost            READ vHost              WRITE setVHost               )
    Q_PROPERTY(QString      driver           READ driver             WRITE setDriver              )
    Q_PROPERTY(QString      hostName         READ hostName           WRITE setHostName            )
    Q_PROPERTY(QString      userName         READ userName           WRITE setUserName            )
    Q_PROPERTY(QString      passWord         READ passWord           WRITE setPassWord            )
    Q_PROPERTY(QVariantHash parameter        READ parameter          WRITE setParameter           )
    Q_PROPERTY(int          port             READ port               WRITE setPort                )
    Q_PROPERTY(QString      route            READ route              WRITE setRoute               )
    Q_PROPERTY(QString      topic            READ topic              WRITE setTopic               )
    Q_PROPERTY(QVariant     activityLimit    READ activityLimit      WRITE setActivityLimit       )

    Q_INVOKABLE explicit QRPCRequestExchangeSetting(QObject *parent=nullptr);
    explicit QRPCRequestExchangeSetting(QRPCRequestExchangeSetting&e, QObject *parent=nullptr);
    Q_INVOKABLE virtual ~QRPCRequestExchangeSetting();

    /**
     * @brief operator =
     * @param v
     * @return
     */
    QRPCRequestExchangeSetting&operator=(const QRPCRequestExchangeSetting&e);
    QRPCRequestExchangeSetting&operator=(const QVariantHash&e);

    /**
     * @brief toMap
     * @return
     */
    virtual QVariantMap toMap() const;

    /**
     * @brief toMap
     * @return
     */
    virtual QVariantHash toHash() const;

    /**
     * @brief url
     * @return
     */
    virtual QString url() const;

    /**
     * @brief isValid
     * @return
     */
    virtual bool isValid() const;

    /**
     * @brief print
     * @return
     */
    virtual QRPCRequestExchangeSetting&print(const QString&output=QString());

    /**
     * @brief printOut
     * @param output
     * @return
     */
    virtual QStringList printOut(const QString&output=QString());

    /**
     * @brief method
     * @return
     */
    virtual QRPCRequestMethod method() const;
    virtual void setMethod(const int&value);
    virtual void setMethod(const QString&value);
    virtual QString methodName() const;

    /**
     * @brief protocol
     * @return
     */
    virtual QRPCProtocol protocol() const;
    virtual QString protocolName() const;
    virtual QString protocolUrlName() const;
    virtual void setProtocol(const QRPCProtocol &value);
    virtual void setProtocol(const QVariant&value);

    /**
     * @brief driver
     * @return
     */
    virtual QString driver() const;
    virtual void setDriver(const QString &value);

    /**
     * @brief hostName
     * @return
     */
    virtual QString hostName() const;
    virtual void setHostName(const QString &value);

    /**
     * @brief vHost
     * @return
     */
    virtual QString vHost() const;
    virtual void setVHost(const QString &value);

    /**
     * @brief userName
     * @return
     */
    virtual QString userName() const;
    virtual void setUserName(const QString &value);

    /**
     * @brief passWord
     * @return
     */
    virtual QString passWord() const;
    virtual void setPassWord(const QString &value);

    /**
     * @brief route
     * @return
     */
    virtual QString &route() const;
    virtual void setRoute(const QVariant &value);


    /**
     * @brief topic
     * @return
     */
    virtual QString topic() const;
    virtual void setTopic(const QString &value);

    /**
     * @brief port
     * @return
     */
    virtual int port() const;
    virtual void setPort(int port);

    /**
     * @brief activityLimit
     * @return
     */
    virtual qlonglong activityLimit() const;
    virtual void setActivityLimit(const QVariant &value);

    /**
     * @brief parameter
     * @return
     */
    virtual QVariantHash parameter() const;
    virtual void setParameter(const QVariantHash &parameter);

private:
    void*p = nullptr;
    int ___method() const{ return this->method();}
};

}
