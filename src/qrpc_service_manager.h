#pragma once

#include "./qrpc_global.h"
#include "./qstm_setting_manager.h"
#include "./qrpc_service_setting.h"
#include <QSettings>

namespace QRpc {

class Q_RPC_EXPORT ServiceManager : public QStm::SettingManager
{
    Q_OBJECT
public:
    Q_DECLARE_INSTANCE(ServiceManager)
    Q_SETTING_MANAGER_REPLACE_METHODS(ServiceManager, ServiceSetting)
};

}
