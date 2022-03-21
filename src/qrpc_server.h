#pragma once

#include "./qrpc_controller_options.h"
#include "./qrpc_global.h"
#include "./qrpc_listen_colletion.h"
#include "./qrpc_listen_request.h"

namespace QRpc {

//!
//! \brief The Server class
//!
class Q_RPC_EXPORT Server : public QObject
{
    Q_OBJECT
public:
    //!
    //! \brief Server
    //! \param parent
    //!
    Q_INVOKABLE explicit Server(QObject *parent = nullptr);

    //!
    //! \brief Server
    //! \param settings
    //! \param parent
    //!
    Q_INVOKABLE explicit Server(const QVariant &settings, QObject *parent = nullptr);

    //!
    //! \brief ~Server
    //!
    ~Server();

    //!
    //! \brief controllerOptions
    //! \return
    //!
    virtual ControllerOptions &controllerOptions();

    //!
    //! \brief isFinished
    //! \return
    //!
    Q_INVOKABLE virtual bool isFinished() const;

    //!
    //! \brief isRunning
    //! \return
    //!
    Q_INVOKABLE virtual bool isRunning() const;

    //!
    //! \brief start
    //! \return
    //!
    Q_INVOKABLE virtual bool start();

    //!
    //! \brief stop
    //! \return
    //!
    Q_INVOKABLE virtual bool stop();

    //!
    //! \brief interfacesStart
    //! \return
    //!
    virtual Server &interfacesStart();

    //!
    //! \brief interfacesStop
    //! \return
    //!
    virtual Server &interfacesStop();

    //!
    //! \brief interfacesRunning
    //! \return
    //!
    Q_INVOKABLE virtual bool interfacesRunning();

    //!
    //! \brief controllers
    //! \return
    //!
    virtual QList<const QMetaObject *> &controllers();

    //!
    //! \brief controllerParsers
    //! \return
    //!
    virtual QList<const QMetaObject *> &controllerParsers();

    //!
    //! \brief requestEnable
    //!
    virtual void requestEnable();

    /**
     * @brief requestDisable
     * @return
     */
    virtual void requestDisable();

    //!
    //! \brief colletions
    //! \return
    //!
    virtual ListenColletions &colletions();

    //!
    //! \brief settingsFileName
    //! \return
    //!
    Q_INVOKABLE virtual QVariant settingsFileName();

    //!
    //! \brief setSettingsFileName
    //! \param fileName
    //! \return
    //!
    virtual bool setSettingsFileName(const QString &fileName);

    //!
    //! \brief setSettings
    //! \param settings
    //! \return
    //!
    Q_INVOKABLE virtual bool setSettings(const QVariant &settings) const;

    //!
    //! \brief authorizationRequest
    //! \param request
    //! \return
    //!
    virtual bool authorizationRequest(const ListenRequest &request) const;

    //!
    //! \brief serverName
    //! \return
    //!
    Q_INVOKABLE virtual QString serverName() const;

    //!
    //! \brief setServerName
    //! \param value
    //!
    Q_INVOKABLE virtual void setServerName(const QString &value);

private:
    QObject *p = nullptr;
};

//typedef QRpc::Server QRPCServer;

} // namespace QRpc
