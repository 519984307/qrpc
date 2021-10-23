#pragma once

#include "../qrpc_controller_setting.h"
#include "../qrpc_controller_options.h"
#include "./qstm_util_variant.h"
#include <QDebug>
#include <QFile>
#include <QSettings>
#include <QStringList>
#include <QMetaMethod>
#include <QMetaProperty>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

namespace QRpc {

#define dPvt()\
    auto&p = *reinterpret_cast<ControllerOptionsPrv*>(this->p)

class ControllerOptionsPrv{
public:
    QVariant settingsFileName;
    ControllerSetting settingsDefault;
    QVariantHash settingBody;
    QVariantHash arguments;
    QHash<QString, ControllerSetting*> settings;
    ControllerOptions*parent=nullptr;
    QObject*parentParent=nullptr;



    explicit ControllerOptionsPrv(ControllerOptions*parent):settingsDefault(parent){
        this->parent=parent;
        this->init();
    }
    virtual ~ControllerOptionsPrv(){
        this->clear();
    }

    void init(){
    }

    bool isLoaded(){
        QHashIterator<QString, ControllerSetting*> i(this->settings);
        while (i.hasNext()) {
            i.next();
            if(i.key().trimmed().isEmpty())
                continue;
            else{
                auto&v=i.value();
                if(v->isValid())
                    return true;
            }
        }
        return false;
    }

    bool isEmpty(){
        QHashIterator<QString, ControllerSetting*> i(this->settings);
        while (i.hasNext()) {
            i.next();
            auto&v=i.value();
            if(v->isValid())
                return false;
        }
        return true;
    }

    void clear(){
        auto _detail=this->settings.values();
        this->settings.clear();
        qDeleteAll(_detail);
    }

    QVariantHash toHash(){
        QVariantHash vSettings;
        auto vList=QList<ControllerSetting*>()<<&this->settingsDefault;
        vList=vList+this->settings.values();
        for(auto&v:vList)
            vSettings.insert(v->name(), v->toHash());
        return vSettings;
    }

    ControllerSetting&settingGetCheck(const QString&settingName){

        const auto p1=settingName.toLower().trimmed();
        for(auto&setting:this->settings){
            if(setting->macth(p1)){
                return*setting;
            }
        }
        static ControllerSetting defaultControllerSetting;
        defaultControllerSetting.setEnabled(false);
        return defaultControllerSetting;
        return defaultControllerSetting;
    }

    ControllerOptions &insert(const QVariantHash &value)
    {
        auto&p=*this;
        auto vValue=value;
        if(!vValue.isEmpty()){
            auto name=vValue.value(qsl("name")).toByteArray().trimmed();
            if(!name.isEmpty()){
                auto setting=p.settings.value(name);
                if(setting!=nullptr)
                    setting->deleteLater();

                auto l=QStringList()<<QT_STRINGIFY2(activityLimit)<<QT_STRINGIFY2(activityInterval);
                for(auto&property:l){
                    auto v=vValue.value(property);
                    if(v.isValid() && v.toLongLong()<=0){
                        v=ControllerSetting::parseInterval(v);
                        vValue[property]=v;
                    }
                }
                setting=new ControllerSetting();
                setting->fromHash(vValue);
                setting->setName(name);
                p.settings.insert(setting->name(), setting);
            }
        }
        return*this->parent;
    }

    bool v_load(const QVariant &v){
        if(v.type()==v.List || v.type()==v.StringList)
            return this->load(v.toStringList());
        else if(v.type()==v.Map || v.type()==v.Hash)
            return this->load(v.toHash());
        else
            return this->load(v.toString());
    }

    bool load(QObject *settingsObject)
    {
        auto&p=*this;
        if(settingsObject!=nullptr){
            auto metaObject=settingsObject->metaObject();
            for(int methodIndex = 0; methodIndex < metaObject->methodCount(); ++methodIndex) {
                auto metaMethod = metaObject->method(methodIndex);
                if(metaMethod.parameterCount()==0){
                    auto methodName=QString(metaMethod.name()).toLower().trimmed();
                    auto staticNames=QStringList()<<"settingsfilename"<<"settings_server"<<"settingsserver";
                    if(staticNames.contains(methodName)){
                        QVariant invokeReturn;
                        auto argReturn=Q_RETURN_ARG(QVariant, invokeReturn);
                        if(metaMethod.invoke(settingsObject, argReturn)){
                            return p.v_load(invokeReturn);
                        }
                    }
                }
            }
        }
        return false;
    }

    bool load(const QStringList &settingsFileName)
    {
        QVariantList vList;
        auto&p=*this;
        for(auto&fileName:settingsFileName){
            QFile file(fileName);
            if(fileName.isEmpty())
                continue;
            else if(!file.exists())
#if Q_RPC_LOG
                sWarning()<<qsl("file not exists %1").arg(file.fileName());
#endif
            else if(!file.open(QFile::ReadOnly))
#if Q_RPC_LOG
                sWarning()<<qsl("%1, %2").arg(file.fileName(), file.errorString());
#endif
            else{
                auto bytes=file.readAll();
                file.close();
                QJsonParseError*error=nullptr;
                auto doc=QJsonDocument::fromJson(bytes, error);
                if(error!=nullptr)
#if Q_RPC_LOG
                    sWarning()<<qsl("%1, %2").arg(file.fileName(), error->errorString());
#endif
                else if(doc.object().isEmpty())
#if Q_RPC_LOG
                    sWarning()<<qsl("object is empty, %1").arg(file.fileName());
#endif
                else{
                    auto map=doc.object().toVariantHash();
                    if(!map.isEmpty())
                        vList<<map;
                }
            }
        }
        VariantUtil vu;
        auto vMap=vu.vMerge(vList).toHash();
        if(p.load(vMap))
            this->settingsFileName=settingsFileName;
        else
            this->settingsFileName.clear();
        return p.isLoaded();
    }

    bool load(const QString &fileName)
    {
        auto&p=*this;
        QFile file(fileName);
        if(fileName.trimmed().isEmpty())
#if Q_RPC_LOG
            sWarning()<<qsl("not file settings");
#endif
        else if(!file.exists())
#if Q_RPC_LOG
            sWarning()<<qsl("file not exists %1").arg(file.fileName());
#endif
        else if(!file.open(QFile::ReadOnly))
#if Q_RPC_LOG
            sWarning()<<qsl("%1, %2").arg(file.fileName(), fileName);
#endif
        else{
            auto bytes=file.readAll();
            file.close();
            QJsonParseError*error=nullptr;
            auto doc=QJsonDocument::fromJson(bytes, error);
            if(error!=nullptr)
#if Q_RPC_LOG
                sWarning()<<qsl("%1, %2").arg(file.fileName(), error->errorString());
#endif
            else if(doc.object().isEmpty())
#if Q_RPC_LOG
                sWarning()<<qsl("object is empty, %1").arg(file.fileName());
#endif
            else{
                auto map=doc.object().toVariantHash();
                if(!map.contains(qsl("services")))
#if Q_RPC_LOG
                    sWarning()<<qsl("tag services not exists, %1").arg(file.fileName());
#endif
                else
                    return p.load(map);
            }
        }
        this->settingsFileName.clear();
        return false;
    }


    bool load(const QVariantHash &settings)
    {
        auto&p=*this;
        p.settingBody=settings;

        auto arguments=settings.value(qsl("arguments"));

        if(arguments.canConvert(arguments.Map)){
            QHashIterator<QString, QVariant> i(arguments.toHash());
            while (i.hasNext()) {
                i.next();
                p.arguments.insert(i.key().toLower(),i.value());
            }
        }
        else if(arguments.canConvert(arguments.List)){
            for(auto&v:arguments.toList()){
                auto l=v.toString().split(qsl("="));
                if(l.isEmpty()){
                    continue;
                }
                else if(l.size()==1){
                    auto key=l.first();
                    auto value=l.last();
                    p.arguments.insert(key,value);
                }
                else{
                    auto key=l.first().toLower();
                    auto value=l.last();
                    p.arguments.insert(key,value);
                }
            }
        }

        QVariantHash defaultVariables({{qsl("hostName") , qsl("SERVICE_HOST")}});
        QVariantHash defaultValues;
        if(!defaultVariables.isEmpty()){
            QHashIterator<QString, QVariant> i(defaultVariables);
            while (i.hasNext()) {
                i.next();
                auto env = i.value().toByteArray();
                auto v = QByteArray(getenv(env)).trimmed();
                if(v.isEmpty())
                    v = QByteArray(getenv(env.toLower())).trimmed();
                if(!v.isEmpty())
                    defaultValues.insert(i.key(),v);
            }
        }

        auto defaultSetting=settings.value(qsl("default")).toHash();

        p.settingsDefault=defaultSetting;

        if(settings.contains(qsl("hostName")) && settings.contains(qsl("port"))){
            this->insert(settings);
        }
        else{
            QHashIterator<QString, QVariant> i(settings);
            while (i.hasNext()) {
                i.next();
                auto value=i.value().toHash();
                value.insert(qsl("name"), i.key().trimmed());

                {
                    QHashIterator<QString, QVariant> i(defaultValues);
                    while (i.hasNext()) {
                        i.next();
                        if(!value.contains(i.key()))
                            value.insert(i.key(), i.value());
                    }
                }

                {
                    QHashIterator<QString, QVariant> i(defaultSetting);
                    while (i.hasNext()) {
                        i.next();
                        if(!value.contains(i.key()))
                            value.insert(i.key(), i.value());
                    }
                }

                this->insert(value);

            }
        }
        return this->isLoaded();
    }

};

}
