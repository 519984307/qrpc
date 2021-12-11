#pragma once

#include "./qrpc_global.h"
#include "./qrpc_types.h"
#include <QObject>
#include <QVariant>

class QSettings;

namespace QRpc {
class QRPCListen;

//!
//! \brief The QRPCListenProtocol class
//!
class Q_RPC_EXPORT QRPCListenProtocol : public QObject
{
    Q_OBJECT
    friend class QRPCListenColletions;
    friend class QRPCListenColletionsPvt;
public:

    Q_PROPERTY(int          protocol                READ protocol               WRITE setProtocol               NOTIFY changeProperty   )
    Q_PROPERTY(QByteArray   protocolName            READ protocolName                                           NOTIFY changeProperty   )
    Q_PROPERTY(QByteArray   optionName              READ optionName             WRITE setOptionName             NOTIFY changeProperty   )
    Q_PROPERTY(int          cleanupInterval         READ cleanupInterval        WRITE setCleanupInterval        NOTIFY changeProperty   )
    Q_PROPERTY(int          minThreads              READ minThreads             WRITE setMinThreads             NOTIFY changeProperty   )
    Q_PROPERTY(int          maxThreads              READ maxThreads             WRITE setMaxThreads             NOTIFY changeProperty   )
    Q_PROPERTY(int          maxRequestSize          READ maxRequestSize         WRITE setMaxRequestSize         NOTIFY changeProperty   )
    Q_PROPERTY(int          maxMultiPartSize        READ maxMultiPartSize       WRITE setMaxMultiPartSize       NOTIFY changeProperty   )
    Q_PROPERTY(QByteArray   driver                  READ driver                 WRITE setDriver                 NOTIFY changeProperty   )
    Q_PROPERTY(QByteArray   hostName                READ hostName               WRITE setHostName               NOTIFY changeProperty   )
    Q_PROPERTY(QByteArray   userName                READ userName               WRITE setUserName               NOTIFY changeProperty   )
    Q_PROPERTY(QByteArray   password                READ password               WRITE setPassword               NOTIFY changeProperty   )
    Q_PROPERTY(QByteArray   database                READ database               WRITE setDatabase               NOTIFY changeProperty   )
    Q_PROPERTY(QByteArray   options                 READ options                WRITE setOptions                NOTIFY changeProperty   )
    Q_PROPERTY(QVariantList port                    READ port                   WRITE setPort                   NOTIFY changeProperty   )
    Q_PROPERTY(QVariantList queue                   READ queue                  WRITE setQueue                  NOTIFY changeProperty   )
    Q_PROPERTY(QByteArray   sslKeyFile              READ sslKeyFile             WRITE setSslKeyFile             NOTIFY changeProperty   )
    Q_PROPERTY(QByteArray   sslCertFile             READ sslCertFile            WRITE setSslCertFile            NOTIFY changeProperty   )
    Q_PROPERTY(bool         enabled                 READ enabled                WRITE setEnabled                NOTIFY changeProperty   )
    Q_PROPERTY(bool         realMessageOnException  READ realMessageOnException WRITE setRealMessageOnException NOTIFY changeProperty   )

    //!
    //! \brief QRPCListenProtocol
    //! \param parent
    //!
    Q_INVOKABLE explicit QRPCListenProtocol(QObject *parent = nullptr);
private:
    //!
    //! \brief QRPCListenProtocol
    //! \param protocol
    //! \param metaObject
    //! \param parent
    //!
    Q_INVOKABLE explicit QRPCListenProtocol(int protocol, const QMetaObject&metaObject, QObject *parent);
public:

    //!
    //! \brief isValid
    //! \return
    //!
    virtual bool isValid() const;

    //!
    //! \brief makeListen
    //! \return
    //!
    virtual QRPCListen *makeListen();

    //!
    //! \brief protocol
    //! \return
    //!
    virtual int protocol();
private:

    //!
    //! \brief setProtocol
    //! \param value
    //!
    virtual void setProtocol(const int &value);
public:

    //!
    //! \brief protocolName
    //! \return
    //!
    virtual QByteArray protocolName();

    //!
    //! \brief optionName
    //! \return
    //!
    virtual QByteArray optionName();

private:

    //!
    //! \brief setOptionName
    //! \param value
    //!
    virtual void setOptionName(const QByteArray &value);
public:

    //!
    //! \brief minThreads
    //! \return
    //!
    virtual int minThreads() const;

    //!
    //! \brief setMinThreads
    //! \param value
    //!
    virtual void setMinThreads(int value);

    //!
    //! \brief maxThreads
    //! \return
    //!
    virtual int maxThreads() const;

    //!
    //! \brief setMaxThreads
    //! \param value
    //!
    virtual void setMaxThreads(int value);

    //!
    //! \brief cleanupInterval
    //! \return
    //!default value 10000ms
    virtual int cleanupInterval() const;

    //!
    //! \brief setCleanupInterval
    //! \param value
    //!
    virtual void setCleanupInterval(int value);

    //!
    //! \brief maxRequestSize
    //! \return
    //!
    virtual int maxRequestSize() const;

    //!
    //! \brief setMaxRequestSize
    //! \param value
    //!
    virtual void setMaxRequestSize(int value);

    //!
    //! \brief maxMultiPartSize
    //! \return
    //!
    virtual int maxMultiPartSize() const;

    //!
    //! \brief setMaxMultiPartSize
    //! \param value
    //!
    virtual void setMaxMultiPartSize(int value);

    //!
    //! \brief driver
    //! \return
    //!
    virtual QByteArray driver() const;

    //!
    //! \brief setDriver
    //! \param value
    //!
    virtual void setDriver(const QByteArray &value);

    //!
    //! \brief hostName
    //! \return
    //!
    virtual QByteArray hostName() const;

    //!
    //! \brief setHostName
    //! \param value
    //!
    virtual void setHostName(const QByteArray &value);

    //!
    //! \brief userName
    //! \return
    //!
    virtual QByteArray userName() const;

    //!
    //! \brief setUserName
    //! \param value
    //!
    virtual void setUserName(const QByteArray &value);

    //!
    //! \brief password
    //! \return
    //!
    virtual QByteArray password() const;

    //!
    //! \brief setPassword
    //! \param value
    //!
    virtual void setPassword(const QByteArray &value);

    //!
    //! \brief database
    //! \return
    //!
    virtual QByteArray database() const;

    //!
    //! \brief setDatabase
    //! \param value
    //!
    virtual void setDatabase(const QByteArray &value);

    //!
    //! \brief options
    //! \return
    //!
    virtual QByteArray options() const;

    //!
    //! \brief setOptions
    //! \param value
    //!
    virtual void setOptions(const QByteArray &value);

    //!
    //! \brief queue
    //! \return
    //!
    virtual QVariantList &queue();

    //!
    //! \brief setQueue
    //! \param value
    //!
    virtual void setQueue(const QByteArray &value);

    //!
    //! \brief setQueue
    //! \param value
    //!
    virtual void setQueue(const QVariantList &value);

    //!
    //! \brief port
    //! \return
    //!
    virtual QVariantList port() const;

    //!
    //! \brief setPort
    //! \param value
    //!
    virtual void setPort(const QVariant &value);

    //!
    //! \brief toMap
    //! \return
    //!
    virtual QVariantMap toMap() const;

    //!
    //! \brief toHash
    //! \return
    //!
    virtual QVariantHash&toHash() const;

    //!
    //! \brief settings
    //! \return
    //!
    virtual QSettings&settings() const;

    //!
    //! \brief setSettings
    //! \param settings
    //! \param defaultSettings
    //!
    virtual void setSettings(const QVariantHash&settings, const QVariantHash&defaultSettings);

    //!
    //! \brief makeSettings
    //! \param parent
    //! \return
    //!
    virtual QSettings*makeSettings(QObject *parent=nullptr);

    //!
    //! \brief makeSettingsHash
    //! \return
    //!
    virtual QVariantHash makeSettingsHash() const;

    //!
    //! \brief enabled
    //! \return
    //!
    virtual bool enabled() const;

    //!
    //! \brief setEnabled
    //! \param value
    //!
    virtual void setEnabled(bool value);

    //!
    //! \brief sslKeyFile
    //! \return
    //!
    virtual QByteArray sslKeyFile() const;
    virtual void setSslKeyFile(const QByteArray &value);

    //!
    //! \brief sslCertFile
    //! \return
    //!
    virtual QByteArray sslCertFile() const;

    //!
    //! \brief setSslCertFile
    //! \param value
    //!
    virtual void setSslCertFile(const QByteArray &value);

    //!
    //! \brief realMessageOnException
    //! \return
    //!true on debug
    virtual bool realMessageOnException() const;

    //!
    //! \brief setRealMessageOnException
    //! \param value
    //!
    virtual void setRealMessageOnException(bool value);

private:
    QObject*p=nullptr;
signals:
    //!
    //! \brief changeProperty
    //!
    void changeProperty();
};

}
