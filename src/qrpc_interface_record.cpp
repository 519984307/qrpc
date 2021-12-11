#include "./qrpc_interface_record.h"
#include <QDate>

namespace QRpc {

QRPCInterfaceRecord::QRPCInterfaceRecord(QObject *parent) : QRpc::QRPCController(parent)
{

}

QRPCInterfaceRecord::~QRPCInterfaceRecord()
{
}

QVariant QRPCInterfaceRecord::get()
{
    QRPC_METHOD_CHECK_GET();
    return this->rq().co().setNotImplemented().emptyData();
}

QVariant QRPCInterfaceRecord::set()
{
    QRPC_METHOD_CHECK_POST();
    return this->rq().co().setNotImplemented().emptyData();
}

QVariant QRPCInterfaceRecord::del()
{
    QRPC_METHOD_CHECK_DELETE();
    return this->rq().co().setNotImplemented().emptyData();
}

QVariant QRPCInterfaceRecord::list()
{
    QRPC_METHOD_CHECK_GET();
    return this->rq().co().setNotImplemented().emptyData();
}


} // namespace QRpc
