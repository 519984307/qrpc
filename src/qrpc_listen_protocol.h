#pragma once

#include "./qrpc_global.h"
#include "./qrpc_types.h"
#include <QObject>
#include <QVariant>

class QSettings;

namespace QRpc {
    class QRPCListen;
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

        Q_INVOKABLE explicit QRPCListenProtocol(QObject *parent = nullptr);
    private:
        Q_INVOKABLE explicit QRPCListenProtocol(int protocol, const QMetaObject&metaObject, QObject *parent);
    public:

        /**
         * @brief isValid
         * @return
         */
        virtual bool isValid() const;

        /**
         * @brief makeListen
         * @param parent
         * @return
         */
        virtual QRPCListen *makeListen();

        /**
         * @brief protocolName
         * @return
         */
        virtual int protocol();
    private:
        virtual void setProtocol(const int &value);
    public:

        /**
         * @brief protocolName
         * @return
         */
        virtual QByteArray protocolName();

        /**
         * @brief optionName
         * @return
         */
        virtual QByteArray optionName();

    private:
        virtual void setOptionName(const QByteArray &value);
    public:

        /**
         * @brief minThreads
         * @return
         */
        virtual int minThreads() const;
        virtual void setMinThreads(int value);

        /**
         * @brief maxThreads
         * @return
         */
        virtual int maxThreads() const;
        virtual void setMaxThreads(int value);

        /**
         * @brief cleanupInterval
         * @return
         *
         * default value 10000ms
         */
        virtual int cleanupInterval() const;
        virtual void setCleanupInterval(int value);

        /**
         * @brief maxRequestSize
         * @return
         */
        virtual int maxRequestSize() const;
        virtual void setMaxRequestSize(int value);

        /**
         * @brief maxMultiPartSize
         * @return
         */
        virtual int maxMultiPartSize() const;
        virtual void setMaxMultiPartSize(int value);

        /**
         * @brief driver
         * @return
         */
        virtual QByteArray driver() const;
        virtual void setDriver(const QByteArray &value);

        /**
         * @brief hostName
         * @return
         */
        virtual QByteArray hostName() const;
        virtual void setHostName(const QByteArray &value);

        /**
         * @brief hostName
         * @return
         */
        virtual QByteArray userName() const;
        virtual void setUserName(const QByteArray &value);

        /**
         * @brief password
         * @return
         */
        virtual QByteArray password() const;
        virtual void setPassword(const QByteArray &value);

        /**
         * @brief database
         * @return
         */
        virtual QByteArray database() const;
        virtual void setDatabase(const QByteArray &value);

        /**
         * @brief options
         * @return
         */
        virtual QByteArray options() const;
        virtual void setOptions(const QByteArray &value);


        /**
         * @brief queue
         * @return
         */
        virtual QVariantList &queue();
        virtual void setQueue(const QByteArray &value);
        virtual void setQueue(const QVariantList &value);

        /**
         * @brief port
         * @return
         */
        virtual QVariantList port() const;
        virtual void setPort(const QVariant &value);

        /**
         * @brief toMap
         * @return
         */
        virtual QVariantMap toMap() const;

        /**
         * @brief toHash
         * @return
         */
        virtual QVariantHash&toHash() const;

        /**
         * @brief settings
         * @return
         */
        virtual QSettings&settings() const;

        /**
         * @brief setSettings
         * @param settings
         */
        virtual void setSettings(const QVariantHash&settings, const QVariantHash&defaultSettings);

        /**
         * @brief makeSettings
         * @return
         */
        virtual QSettings*makeSettings(QObject *parent=nullptr);

        /**
         * @brief makeSettingsMap
         * @return
         */
        virtual QVariantHash makeSettingsHash() const;

        /**
         * @brief enabled
         * @return
         */
        virtual bool enabled() const;
        virtual void setEnabled(bool value);

        /**
         * @brief sslKeyFile
         * @return
         */
        virtual QByteArray sslKeyFile() const;
        virtual void setSslKeyFile(const QByteArray &value);

        /**
         * @brief sslCertFile
         * @return
         */
        virtual QByteArray sslCertFile() const;
        virtual void setSslCertFile(const QByteArray &value);

        /**
         * @brief realMessageOnException
         * @return
         *
         * true on debug
         */
        virtual bool realMessageOnException() const;
        virtual void setRealMessageOnException(bool value);

    private:
        QObject*p=nullptr;
    signals:
        void changeProperty();
    };

}
