#include "./p_qrpc_listen_broker_mqtt.h"
#include <QCryptographicHash>

namespace QRpc {

#define dPvt()\
    auto&p =*reinterpret_cast<QRPCListenBrokerMQTTPvt*>(this->p)

class QRPCListenBrokerMQTTPvt{
public:
    explicit QRPCListenBrokerMQTTPvt(QRPCListenBrokerMQTT*object){
        Q_UNUSED(object)
    }
    virtual ~QRPCListenBrokerMQTTPvt(){
    }
};

QRPCListenBrokerMQTT::QRPCListenBrokerMQTT(QObject *parent):QRPCListen(parent){
    this->p = new QRPCListenBrokerMQTTPvt(this);
}

QRPCListenBrokerMQTT::~QRPCListenBrokerMQTT()
{
    dPvt();delete&p;
}

}
