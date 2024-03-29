#pragma once

#include "../../qstm/src/qstm_setting_manager.h"
#include "./qrpc_global.h"
#include "./qrpc_service_setting.h"
#include <QSettings>

namespace QRpc {

//!
//! \brief The ServiceManager class
//!
class Q_RPC_EXPORT ServiceManager : public QStm::SettingManager
{
    Q_OBJECT
public:
    Q_DECLARE_INSTANCE(ServiceManager)
    Q_SETTING_MANAGER_REPLACE_METHODS(ServiceManager, ServiceSetting)
};

}
