#include "./qrpc_controller_options.h"
#include "./private/p_qrpc_controller_options.h"

namespace QRpc {

ControllerOptions::ControllerOptions(QObject *parent) : QObject(nullptr)
{
    this->p = new ControllerOptionsPrv(this);
    if(parent && parent->thread()==this->thread())
        this->setParent(parent);
    dPvt();
    p.parentParent = parent;
    p.load(p.parentParent);
}

ControllerOptions::ControllerOptions(const QStringList &settingFileName, QObject *parent)
{
    this->p = new ControllerOptionsPrv(this);
    if(parent!=nullptr){
        if(parent->thread()==this->thread())
            this->setParent(parent);
    }
    dPvt();
    p.load(settingFileName);
}

ControllerOptions::ControllerOptions(const QString &settingFileName, QObject *parent):QObject(parent)
{
    this->p = new ControllerOptionsPrv(this);
    if(parent!=nullptr){
        if(parent->thread()==this->thread())
            this->setParent(parent);
    }
    dPvt();
    p.load(settingFileName);
}

ControllerOptions::~ControllerOptions()
{
    dPvt();delete&p;
}

void ControllerOptions::clear()
{
    dPvt();
    p.clear();
}

ControllerOptions &ControllerOptions::insert(ControllerSetting &value)
{
    dPvt();
    return p.insert(value.toHash());
}

ControllerOptions &ControllerOptions::insert(const QVariantHash &value)
{
    dPvt();
    return p.insert(value);
}

ControllerSetting&ControllerOptions::setting()
{
    return this->setting(qsl_null);
}

ControllerSetting&ControllerOptions::setting(const QString&value)
{
    dPvt();
    return p.settingGetCheck(value);
}

bool ControllerOptions::load(const QVariant &settings)
{
    dPvt();
    return p.v_load(settings);
}

bool ControllerOptions::load(const QVariantHash &settings)const
{
    dPvt();
    return p.v_load(settings);
}

bool ControllerOptions::load(const ControllerOptions&manager)const
{
    dPvt();
    return p.load(manager.toHash());
}

bool ControllerOptions::load(QObject *settingsObject)const
{
    dPvt();
    return p.load(settingsObject);
}

QVariant ControllerOptions::settingsFileName()const
{
    dPvt();
    return p.settingsFileName;
}

ControllerOptions &ControllerOptions::operator<<(ControllerSetting &value)
{
    return this->insert(value);
}

QVariantHash ControllerOptions::settingBody() const
{
    dPvt();
    return p.settingBody;
}

QVariantHash ControllerOptions::arguments() const
{
    dPvt();
    return p.arguments;
}

void ControllerOptions::setArguments(const QVariantHash &value)
{
    dPvt();
    p.arguments = value;
}

QVariantMap ControllerOptions::toMap() const
{
    dPvt();
    return QVariant(p.toHash()).toMap();
}

QVariantHash ControllerOptions::toHash() const
{
    dPvt();
    return p.toHash();
}

}
