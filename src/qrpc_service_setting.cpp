#include "./qrpc_service_setting.h"
#include "../../qstm/src/private/p_qstm_base_setting.h"


namespace QRpc {

    QVariant ServiceSetting::url()const
    {
        QVariantList vList;
        for(auto&v:this->routeList()){
            auto route=v.toString().trimmed();
            auto url=qsl("%1:%2/%3").arg(this->hostName()).arg(this->port()).arg(route);
            while(url.contains(qsl("//")))
                url=url.replace(qsl("//"), qsl("/"));


            switch (qTypeId(protocol())) {
            case QMetaType_QVariantList:
            case QMetaType_QStringList:
            {
                auto record=this->protocol().toList();
                for(const auto&v:record){
                    QString protocol;
                    switch (qTypeId(v)) {
                    case QMetaType_Int:
                        protocol=QRPCProtocolName.value(v.toInt());
                        break;
                    case QMetaType_QString:
                    case QMetaType_QByteArray:
                        protocol=v.toString().toLower();
                        break;
                    default:
                        continue;
                    }
                    vList<<qsl("%1://%2").arg(protocol,url);
                }
                break;
            }
            default:
                const auto protocol=this->protocol().toString();
                vList<<qsl("%1://%2").arg(protocol,url);;
            }
        }
        QVariant __return=(vList.size()==1)?vList.first():vList;
        return __return;
    }

}
