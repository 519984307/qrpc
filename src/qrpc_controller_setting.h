#pragma once

#include "./qrpc_global.h"
#include "../../qstm/src/qstm_setting_base.h"
#include <QVariant>
#include <QVariantHash>

namespace QRpc {

//!
//! \brief The ControllerSetting class
//!
class Q_RPC_EXPORT ControllerSetting:public QStm::SettingBase
{
    Q_OBJECT
    SETTING_OBJECT(ControllerSetting)
public:
    Q_PROPERTY(QVariantHash headers READ headers WRITE setHeaders )
    Q_PROPERTY(QString path READ path WRITE setPath )
public:
    //!
    //! \brief ControllerSetting
    //! \param parent
    //!
    Q_INVOKABLE explicit ControllerSetting(QObject*parent=nullptr):QStm::SettingBase(parent)
    {
    }
    ~ControllerSetting()
    {
    }
};


}

