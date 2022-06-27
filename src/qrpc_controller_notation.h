#pragma once

#include <QObject>
#include <QVariantHash>
#include <QVariantList>
#include <QString>
#include <QNotation>
#include "./qrpc_global.h"

namespace QRpcPrivate
{

//!
//! \brief The NotationsExtended class
//!
class Q_RPC_EXPORT NotationsExtended:public QNotation::Extended
{
public:

    enum Classification{
        ApiOperation
    };


    //!
    //! \brief NotationsExtended
    //! \param parent
    //!
    explicit NotationsExtended(QObject*parent=nullptr);
    virtual ~NotationsExtended();

    //!
    //! \brief apiDoc
    //!
    Q_NOTATION_DECLARE(apiDoc, Documentation)

    //!
    //! \brief apiName
    //!
    Q_NOTATION_DECLARE_FUNC(apiName, Information)

    //!
    //! \brief apiDescription
    //!
    Q_NOTATION_DECLARE_FUNC(apiDescription, Information)

    //!
    //! \brief apiGroup
    //!
    Q_NOTATION_DECLARE_FUNC(apiGroup, Information)

    //!
    //! \brief apiModule
    //!
    Q_NOTATION_DECLARE_FUNC(apiModule, Information)

    //!
    //! \brief apiRedirect
    //!
    Q_NOTATION_DECLARE(apiRedirect, Information)

    //!
    //! \brief apiBasePath
    //!
    Q_NOTATION_DECLARE_FUNC(apiBasePath, Information)

    //!
    //!
    Q_NOTATION_DECLARE_FUNC(opName, Information)

    //!
    //! \brief opDescription
    //!
    Q_NOTATION_DECLARE_FUNC(opDescription, Information)

    //!
    //! \brief opGroup
    //!
    Q_NOTATION_DECLARE_FUNC(opGroup, Information)

    //!
    //! \brief opPath
    //! \param path
    //! \return
    //!
    Q_NOTATION_DECLARE_FUNC(opPath, Information)

    //!
    //! \brief opCrud
    //!
    Q_NOTATION_DECLARE(opCrud, ApiOperation)

    //!
    //! \brief opTrace
    //!
    Q_NOTATION_DECLARE(opTrace, ApiOperation)

    //!
    //! \brief opPatch
    //!
    Q_NOTATION_DECLARE(opPatch, ApiOperation)

    //!
    //! \brief opHead
    //!
    Q_NOTATION_DECLARE(opHead, ApiOperation)

    //!
    //! \brief opOptions
    //!
    Q_NOTATION_DECLARE(opOptions, ApiOperation)

    //!
    //! \brief opGet
    //!
    Q_NOTATION_DECLARE(opGet, ApiOperation)

    //!
    //! \brief opPost
    //!
    Q_NOTATION_DECLARE(opPost, ApiOperation)

    //!
    //! \brief opPut
    //!
    Q_NOTATION_DECLARE(opPut, ApiOperation)

    //!
    //! \brief opDelete
    //!
    Q_NOTATION_DECLARE(opDelete, ApiOperation)

    //!
    //! \brief opRules
    //!
    Q_NOTATION_DECLARE_FUNC(opRules, Security)

    //!
    //! \brief rqExcludePath
    //!
    Q_NOTATION_DECLARE(rqExcludePath, Information)

    //!
    //! \brief rqSecurityIgnore
    //!
    Q_NOTATION_DECLARE(rqSecurityIgnore, Security)

    //!
    //! \brief apiRedirect
    //!
    Q_NOTATION_DECLARE(rqRedirect, Operation)
private:

};

}
