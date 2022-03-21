#include "./p_qrpc_listen_broker_mqtt.h"

namespace QRpc {

#define dPvt() auto &p = *reinterpret_cast<ListenBrokerMQTTPvt *>(this->p)

class ListenBrokerMQTTPvt
{
public:
    explicit ListenBrokerMQTTPvt(ListenBrokerMQTT *object) { Q_UNUSED(object) }
    virtual ~ListenBrokerMQTTPvt() {}
};

ListenBrokerMQTT::ListenBrokerMQTT(QObject *parent) : Listen{parent}
{
    this->p = new ListenBrokerMQTTPvt{this};
}

ListenBrokerMQTT::~ListenBrokerMQTT()
{
    dPvt();
    delete &p;
}

} // namespace QRpc
