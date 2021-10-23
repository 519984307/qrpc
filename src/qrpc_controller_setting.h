#pragma once

#include "./qrpc_global.h"
#include "./qstm_setting_base.h"
#include <QVariant>
#include <QVariantHash>

namespace QRpc {

/**
 * @brief The ControllerSetting struct
 */
class Q_RPC_EXPORT ControllerSetting:public QStm::SettingBase{
    Q_OBJECT
    SETTING_OBJECT(ControllerSetting)
public:
    Q_PROPERTY(QVariantHash headers READ headers WRITE setHeaders )
    Q_PROPERTY(QString path READ path WRITE setPath )
public:
    Q_INVOKABLE explicit ControllerSetting(QObject*parent=nullptr):QStm::SettingBase(parent){
    }
    Q_INVOKABLE ~ControllerSetting(){
    }
};


}

