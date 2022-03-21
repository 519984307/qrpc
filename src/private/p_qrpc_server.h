#pragma once

#include "../qrpc_controller.h"
#include "../qrpc_controller_options.h"
#include "../qrpc_listen_colletion.h"
#include "../qrpc_server.h"
#include "./p_qrpc_listen_qrpc.h"
#include <QCryptographicHash>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QMutex>

namespace QRpc {

//!
//! \brief The ServerPvt class
//!
class Q_RPC_EXPORT ServerPvt : public QObject
{
public:
    ControllerOptions controllerOptions;
    QString serverName;
    bool enabledInterfaces = true;
    QVariant settingsFileName;
    ListenColletions *listenColletions = nullptr;
    Server *server = nullptr;
    //!
    //! \brief ServerPvt
    //! \param server
    //!
    explicit ServerPvt(Server *server = nullptr) : QObject(server)
    {
        this->server = server;
        this->listenColletions = new ListenColletions(server);
    }

    //!
    //! \brief ~ServerPvt
    //!
    virtual ~ServerPvt()
    {
        this->serverStop();
        delete this->listenColletions;
    }

    //!
    //! \brief toMd5
    //! \param v
    //! \return
    //!
    static QByteArray toMd5(const QVariant &v)
    {
        if (!v.isValid())
            return QByteArray();

        auto token_data = v.toByteArray().trimmed();
        if (token_data.length() == 32) {
            return token_data;
        }
        return QCryptographicHash::hash(token_data, QCryptographicHash::Md5).toHex();
    }

    //!
    //! \brief start
    //! \return
    //!
    bool serverStart()
    {
        if (this->server->objectName().isEmpty()) {
            static int countServerName = 0;
            auto name = qsl("Server_%1").arg(++countServerName);
            this->server->setObjectName(name);
        }

        this->listenColletions->setObjectName(this->server->objectName() + qsl("Pvt"));
        if (!this->listenColletions->isRunning())
            return this->listenColletions->start();
        return this->listenColletions->isRunning();
    }

    //!
    //! \brief stop
    //! \return
    //!
    bool serverStop()
    {
        this->listenColletions->quit();
        return false;
    }

    bool v_load(const QVariant &v)
    {
        auto typeId = qTypeId(v);
        if (QMetaTypeUtilVariantList.contains(typeId))
            return this->load(v.toStringList());
        if (QMetaTypeUtilVariantDictionary.contains(typeId))
            return this->load(v.toHash());
        return this->load(v.toString());
    }

    bool load(const QStringList &settingsFileName)
    {
        QVariantList vList;
        auto &p = *this;
        p.settingsFileName.clear();
        for (auto &fileName : settingsFileName) {
            QFile file(fileName);
            if (file.fileName().isEmpty())
                continue;

            if (!file.exists()) {
#if Q_RPC_LOG
                sWarning() << tr("file not exists %1").arg(file.fileName());
#endif
                continue;
            }

            if (!file.open(QFile::ReadOnly)) {
#if Q_RPC_LOG
                sWarning() << tr("%1, %2").arg(file.fileName(), file.errorString());
#endif
                continue;
            }

            auto bytes = file.readAll();
            file.close();
            QJsonParseError *error = nullptr;
            auto doc = QJsonDocument::fromJson(bytes, error);
            if (error != nullptr) {
#if Q_RPC_LOG
                sWarning() << tr("%1, %2").arg(file.fileName(), error->errorString());
#endif
                continue;
            }

            if (doc.object().isEmpty()) {
#if Q_RPC_LOG
                sWarning() << tr("object is empty, %2").arg(file.fileName());
#endif
                continue;
            }

            auto vHash = doc.object().toVariantHash();
            if (!vHash.isEmpty())
                vList << vHash;
        }
        Q_DECLARE_VU;
        auto vHash = vu.vMerge(vList).toHash();
        if (p.load(vHash))
            p.settingsFileName = settingsFileName;
        else
            p.settingsFileName.clear();
        return !p.settingsFileName.isNull();
    }

    bool load(const QString &settingsFileName)
    {
        QFile file(settingsFileName);
        auto &p = *this;
        if (file.fileName().isEmpty())
            return false;

        if (!file.exists()) {
#if Q_RPC_LOG
            sWarning() << tr("file not exists %1").arg(file.fileName());
#endif
            return false;
        }

        if (!file.open(QFile::ReadOnly)) {
#if Q_RPC_LOG
            sWarning() << tr("%1, %2").arg(file.fileName(), file.errorString());
#endif
            return false;
        }

        p.settingsFileName = settingsFileName;
        auto bytes = file.readAll();
        file.close();
        QJsonParseError *error = nullptr;
        auto doc = QJsonDocument::fromJson(bytes, error);
        if (error != nullptr) {
#if Q_RPC_LOG
            sWarning() << tr("%1, %2").arg(file.fileName(), error->errorString());
#endif
            return false;
        }

        if (doc.object().isEmpty()) {
#if Q_RPC_LOG
            sWarning() << tr("object is empty, %2").arg(file.fileName());
#endif
            return false;
        }
        const auto vMap = doc.object().toVariantHash();
        return this->load(vMap);
    }

    virtual bool load(const QVariantHash &settings) const
    {
        if (!settings.contains(qsl("protocol"))) {
#if Q_RPC_LOG
            sWarning() << tr("Json property [protocol] not detected");
#endif
            return false;
        }

        auto protocol = settings.value(qsl("protocol")).toHash();
        auto controller = settings.value(qsl("controller")).toHash();

        if (protocol.isEmpty()) {
#if Q_RPC_LOG
            sWarning() << tr("Json property [protocol] is empty");
#endif
            return false;
        }

        this->listenColletions->setSettings(protocol);
        this->controllerOptions.load(controller);
        return true;
    }

public:
    QByteArray settingsGroup = qbl("RPCServer");
    QVariantHash settings;
    QList<const QMetaObject *> controllers;
    QList<const QMetaObject *> controllerParsers;
};

} // namespace QRpc
