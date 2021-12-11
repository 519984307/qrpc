#include "./qrpc_request_exchange.h"
#include "./private/p_qrpc_request_job.h"
namespace QRpc {

#define dPvt()\
    auto&p =*reinterpret_cast<QRPCRequestExchangePvt*>(this->p)

class QRPCRequestExchangePvt{
public:
    QRPCRequestExchange*parent=nullptr;
    QRPCRequestExchangeSetting call;
    QRPCRequestExchangeSetting back;

    explicit QRPCRequestExchangePvt(QRPCRequestExchange*parent):call(parent), back(parent){
        this->parent=parent;
    }

    virtual ~QRPCRequestExchangePvt(){
    }
};

QRPCRequestExchange::QRPCRequestExchange(QObject *parent):QObject(parent)
{
    this->p = new QRPCRequestExchangePvt(this);
}

QRPCRequestExchange::QRPCRequestExchange(QRPCRequestExchange &exchange, QObject *parent):QObject(parent)
{
    this->p = new QRPCRequestExchangePvt(this);
    dPvt();
    p.call=exchange.call();
    p.back=exchange.back();
}

QRPCRequestExchange::~QRPCRequestExchange()
{
    dPvt();delete&p;
}

QRPCRequestExchange&QRPCRequestExchange::operator =(QRPCRequestExchange &e)
{
    dPvt();
    p.call=e.call();
    p.back=e.back();
    return*this;
}

QRPCRequestExchangeSetting &QRPCRequestExchange::call()
{
    dPvt();
    return p.call;
}

QRPCRequestExchangeSetting &QRPCRequestExchange::back()
{
    dPvt();
    return p.back;
}

QVariantMap QRPCRequestExchange::toMap() const
{
    dPvt();
    return QVariantMap{{qsl("call"),p.call.toHash()}, {qsl("back"),p.back.toHash()}};
}

QVariantHash QRPCRequestExchange::toHash() const
{
    dPvt();
    return QVariantHash{{qsl("call"),p.call.toHash()}, {qsl("back"),p.back.toHash()}};
}

QRPCRequestExchange &QRPCRequestExchange::print(const QString &output)
{
    for(auto&v:this->printOut(output))
        sInfo()<<v;
    return*this;
}

QStringList QRPCRequestExchange::printOut(const QString &output)
{
    dPvt();
    QStringList out;
    auto space=output.trimmed().isEmpty()?qsl_null:qsl("    ");
    if(p.call.isValid()){
        out<<qsl("%1%2 exchange.call").arg(space, output).trimmed();
        auto outtext=space+qsl(".    ");
        for(auto&v:p.call.printOut(outtext))
            out<<v;
    }
    if(p.back.isValid()){
        out<<qsl("%1%2 exchange.back").arg(space, output).trimmed();
        auto outtext=space+qsl(".    ");
        for(auto&v:p.back.printOut(outtext))
            out<<v;
    }
    return out;
}

void QRPCRequestExchange::setProtocol(const QVariant &value)
{
    this->call().setProtocol(value);
}

void QRPCRequestExchange::setProtocol(const QRPCProtocol &value)
{
    this->call().setProtocol(value);
}

void QRPCRequestExchange::setMethod(const QString &value)
{
    return this->call().setMethod(value);
}

void QRPCRequestExchange::setMethod(const QRPCRequestMethod &value)
{
    this->call().setMethod(value);
}

void QRPCRequestExchange::setHostName(const QString &value)
{
    this->call().setHostName(value);
}

void QRPCRequestExchange::setRoute(const QVariant &value)
{
    this->call().setRoute(value);
}

void QRPCRequestExchange::setPort(const int &value)
{
    this->call().setPort(value);
}

}
