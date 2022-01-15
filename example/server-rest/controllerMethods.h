#ifndef CONTROLLERMETHODS_H
#define CONTROLLERMETHODS_H

#include <QRpc/Server>

//!
//! \brief The ControllerMethods class
//!
//! inhetihed of QRpc::QRPCController
class ControllerMethods : public QRpc::QRPCController
{
    Q_OBJECT
public:
    //!
    //! \brief ControllerMethods
    //! \param parent
    //!
    //! necessary Q_INVOKABLE
    Q_INVOKABLE explicit ControllerMethods(QObject *parent = nullptr);


    //!
    //! \brief authorization
    //! \return
    //!
    //! authorization validations
    //! check more authorization methods:
    //!     canAuthorization
    //!     beforeAuthorization
    //!     afterAuthorization
    //!
    virtual bool authorization();

    //!
    //! \brief listObjects
    //! \return
    //!
    //! method to return object list, necessary Q_INVOKABLE
    //! check more Invoke methods:
    //!     requestBeforeInvoke
    //!     requestAfterInvoke
    Q_INVOKABLE QVariant listObjects();

};


//auto register interface
QRPC_CONTROLLER_AUTO_REGISTER(ControllerMethods)

#endif
