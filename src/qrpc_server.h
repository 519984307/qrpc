#pragma once

#include "./qrpc_global.h"
#include "./qrpc_listen_colletion.h"
#include "./qrpc_listen_request.h"
#include "./qrpc_controller_options.h"

namespace QRpc {

//!
//! \brief The QRPCServer class
//!
class Q_RPC_EXPORT QRPCServer: public QObject
{
    Q_OBJECT
public:

    //!
    //! \brief QRPCServer
    //! \param parent
    //!
    Q_INVOKABLE explicit QRPCServer(QObject *parent=nullptr);

    //!
    //! \brief QRPCServer
    //! \param settings
    //! \param parent
    //!
    Q_INVOKABLE explicit QRPCServer(const QVariant &settings,  QObject *parent=nullptr);

    //!
    //! \brief ~QRPCServer
    //!
    Q_INVOKABLE virtual ~QRPCServer();

    //!
    //! \brief controllerOptions
    //! \return
    //!
    virtual ControllerOptions&controllerOptions();

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
    virtual QRPCServer &interfacesStart();

    //!
    //! \brief interfacesStop
    //! \return
    //!
    virtual QRPCServer &interfacesStop();

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
    //! \brief parsers
    //! \return
    //!
    virtual QList<const QMetaObject *> &parsers();

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
    virtual QRPCListenColletions&colletions();

    //!
    //! \brief settingsFileName
    //! \return
    //!
    Q_INVOKABLE virtual QVariant settingsFileName();

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
    virtual bool authorizationRequest(const QRPCListenRequest&request) const;

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
    QObject*p = nullptr;

};

}
