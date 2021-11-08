#pragma once

#include "./qrpc_global.h"
#include "./qrpc_listen_request.h"

namespace QRpc {

class QRPCListen;
class QRPCListenRequest;
//!
//! \brief The QRPCListenRequestCache class
//!
class Q_RPC_EXPORT QRPCListenRequestCache:public QObject{
    Q_OBJECT
public:

    //!
    //! \brief QRPCListenRequestCache
    //! \param parent
    //!
    explicit QRPCListenRequestCache(QRPCListen*parent=nullptr);

    //!
    //! \brief ~QRPCListenRequestCache
    //!
    Q_INVOKABLE virtual ~QRPCListenRequestCache();

    //!
    //! \brief clear
    //!
    virtual void clear();

    //!
    //! \brief toRequest
    //! \param uuid
    //! \return
    //!
    QRPCListenRequest&toRequest(const QUuid &uuid);

    //!
    //! \brief createRequest
    //! \return
    //!
    QRPCListenRequest&createRequest();

    //!
    //! \brief createRequest
    //! \param vRequest
    //! \return
    //!
    QRPCListenRequest&createRequest(const QVariant&vRequest);

private:
    void*p=nullptr;
};

}
