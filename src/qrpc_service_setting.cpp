#include "./qrpc_service_setting.h"
#include "./private/p_qstm_base_setting.h"


namespace QRpc {

    QVariant ServiceSetting::url()const
    {
        QVariantList vList;
        for(auto&v:this->routeList()){
            auto route=v.toString().trimmed();
            auto url=qsl("%1:%2/%3").arg(this->hostName()).arg(this->port()).arg(route);
            while(url.contains(qsl("//")))
                url=url.replace(qsl("//"), qsl("/"));
            if(qTypeId(protocol())==QMetaType_QVariantList || qTypeId(protocol())==QMetaType_QStringList){
                auto record=this->protocol().toList();
                for(const auto&v:record){
                    QString protocol;
                    if(qTypeId(v)==QMetaType_Int)
                        protocol=QRPCProtocolName.value(v.toInt());
                    else if(qTypeId(v)==QMetaType_QString || qTypeId(v)==QMetaType_QByteArray)
                        protocol=v.toString().toLower();
                    else
                        continue;

                    vList<<qsl("%1://%2").arg(protocol,url);
                }
            }
            else{
                const auto protocol=this->protocol().toString();
                vList<<qsl("%1://%2").arg(protocol,url);;
            }
        }
        QVariant __return=(vList.size()==1)?vList.first():vList;
        return __return;
    }

}
