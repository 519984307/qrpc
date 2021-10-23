#pragma once

#include "./qrpc_global.h"
#include "./qstm_setting_base.h"

namespace QRpc {

/**
 * @brief The ServiceSetting struct
 */
class Q_RPC_EXPORT ServiceSetting:public QStm::SettingBase{
    Q_OBJECT
    SETTING_OBJECT(ServiceSetting)
public:
    Q_PROPERTY(QString      service                 READ service              WRITE setService                 )
    Q_PROPERTY(QVariantHash headers                 READ headers              WRITE setHeaders                 )
    Q_PROPERTY(QVariantHash parameters              READ parameters           WRITE setParameters              )
    Q_PROPERTY(QString      method                  READ method               WRITE setMethod                  )
    Q_PROPERTY(QVariant     protocol                READ protocol             WRITE setProtocol                )
    Q_PROPERTY(QString      driverName              READ driverName           WRITE setDriverName              )
    Q_PROPERTY(QString      hostName                READ hostName             WRITE setHostName                )
    Q_PROPERTY(QString      userName                READ userName             WRITE setUserName                )
    Q_PROPERTY(QString      password                READ password             WRITE setPassword                )
    Q_PROPERTY(int          port                    READ port                 WRITE setPort                    )
    Q_PROPERTY(QVariant     route                   READ route                WRITE setRoute                   )
    Q_PROPERTY(QString      path                    READ path                 WRITE setPath                    )
    Q_PROPERTY(QVariant     body                    READ body                 WRITE setBody                    )
    Q_PROPERTY(QVariant     cacheInterval           READ cacheInterval        WRITE setCacheInterval           )
    Q_PROPERTY(bool         cacheCleanup            READ cacheCleanup         WRITE setCacheCleanup            )
    Q_PROPERTY(QVariant     cacheCleanupInterval    READ cacheCleanupInterval WRITE setCacheCleanupInterval    )
public:    
    Q_INVOKABLE explicit ServiceSetting(QObject*parent=nullptr):QStm::SettingBase(parent){
    }
    Q_INVOKABLE ~ServiceSetting(){
    }
    QVariant url()const;
};


}
