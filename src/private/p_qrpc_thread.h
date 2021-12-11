#pragma once

#include "../qrpc_global.h"
#include <QThread>

namespace QRpc {

//!
//! \brief The QRPCThread class
//!
class Q_RPC_EXPORT QRPCThread : public QThread{
    Q_OBJECT
public:

    //!
    //! \brief QRPCThread
    //! \param parent
    //!
    Q_INVOKABLE explicit QRPCThread(QObject *parent = nullptr);

    //!
    //! \brief ~QRPCThread
    //!
    Q_INVOKABLE ~QRPCThread();

    //!
    //! \brief run
    //!
    void run();

public slots:
    //!
    //! \brief eventRun
    //!
    virtual void eventRun();

    //!
    //! \brief start
    //! \return
    //!
    virtual bool start();

    //!
    //! \brief stop
    //! \return
    //!
    virtual bool stop();

    //!
    //! \brief quit
    //! \return
    //!
    virtual bool quit();
private:
    void*p=nullptr;
};
}
