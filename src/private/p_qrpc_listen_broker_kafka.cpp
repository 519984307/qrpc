#include "./p_qrpc_listen_broker_kafka.h"
#include <QCryptographicHash>

namespace QRpc {

#define dPvt()\
    auto&p =*reinterpret_cast<ListenBrokerKAFKAPvt*>(this->p)

class ListenBrokerKAFKAPvt{
public:
    explicit ListenBrokerKAFKAPvt(ListenBrokerKAFKA*object)
    {
        Q_UNUSED(object)
    }
    virtual ~ListenBrokerKAFKAPvt()
    {
    }
};

ListenBrokerKAFKA::ListenBrokerKAFKA(QObject *parent):Listen(parent)
{
    this->p = new ListenBrokerKAFKAPvt(this);
}

ListenBrokerKAFKA::~ListenBrokerKAFKA()
{
    dPvt();delete&p;
}

}
