#pragma once

#include "./qrpc_global.h"
#include "./qrpc_controller_setting.h"
#include <QSettings>

namespace QRpc {

class Q_RPC_EXPORT ControllerOptions : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit ControllerOptions(QObject *parent = nullptr);
    Q_INVOKABLE explicit ControllerOptions(const QStringList&settingFileName, QObject *parent = nullptr);
    Q_INVOKABLE explicit ControllerOptions(const QString&settingFileName, QObject *parent = nullptr);
    Q_INVOKABLE ~ControllerOptions();

    /**
     * @brief clear
     */
    virtual void clear();

    /**
     * @brief insert
     * @param settingName
     */
    virtual ControllerOptions&insert(ControllerSetting &value);
    virtual ControllerOptions&insert(const QVariantHash &value);

    /**
     * @brief setting
     * @return
     */
    virtual ControllerSetting &setting();
    virtual ControllerSetting &setting(const QString &value);

    /**
     * @brief load
     * @param settings
     * @return
     */
    virtual bool load(const QVariant&settings);
    virtual bool load(const QVariantHash&settings) const;
    virtual bool load(const ControllerOptions &manager) const;
    virtual bool load(QObject *settingsObject) const;

    /**
     * @brief settingsFileName
     * @return
     */
    virtual QVariant settingsFileName() const;

    /**
     * @brief operator <<
     * @param value
     * @return
     */
    ControllerOptions&operator<<(ControllerSetting&value);

    /**
     * @brief settingBody
     * @return
     *
     * full settings source
     */
    virtual QVariantHash settingBody() const;

    /**
     * @brief arguments
     * @return
     *
     * arguments with settings
     */
    virtual QVariantHash arguments() const;
    virtual void setArguments(const QVariantHash &value);

    /**
     * @brief toMap
     * @return
     */
    virtual QVariantMap toMap() const;

    /**
     * @brief toHash
     * @return
     */
    virtual QVariantHash toHash() const;

private:
    void*p=nullptr;
};

}
