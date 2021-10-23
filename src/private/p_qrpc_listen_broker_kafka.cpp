#include "./p_qrpc_listen_broker_kafka.h"
#include <QCryptographicHash>

namespace QRpc {

#define dPvt()\
    auto&p =*reinterpret_cast<QRPCListenBrokerKAFKAPvt*>(this->p)

class QRPCListenBrokerKAFKAPvt{
public:
    explicit QRPCListenBrokerKAFKAPvt(QRPCListenBrokerKAFKA*object){
        Q_UNUSED(object)
    }
    virtual ~QRPCListenBrokerKAFKAPvt(){
    }
};

QRPCListenBrokerKAFKA::QRPCListenBrokerKAFKA(QObject *parent):QRPCListen(parent){
    this->p = new QRPCListenBrokerKAFKAPvt(this);
}

QRPCListenBrokerKAFKA::~QRPCListenBrokerKAFKA()
{
    dPvt();delete&p;
}

}
