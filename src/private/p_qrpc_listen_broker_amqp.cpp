#include "./p_qrpc_listen_broker_amqp.h"
#include <QCryptographicHash>

namespace QRpc {

#define dPvt()\
    auto&p =*reinterpret_cast<QRPCListenBrokerAMQPPvt*>(this->p)

class QRPCListenBrokerAMQPPvt{
public:
    explicit QRPCListenBrokerAMQPPvt(QRPCListenBrokerAMQP*object){
        Q_UNUSED(object)
    }
    virtual ~QRPCListenBrokerAMQPPvt(){
    }
};

QRPCListenBrokerAMQP::QRPCListenBrokerAMQP(QObject *parent):QRPCListen(parent){
    this->p = new QRPCListenBrokerAMQPPvt(this);
}

QRPCListenBrokerAMQP::~QRPCListenBrokerAMQP()
{
    dPvt();delete&p;
}

}
