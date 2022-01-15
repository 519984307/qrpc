#include "controllerMethods.h"


bool makeObjectList(int limit, QVariantList&outList)
{
    for (int index = 1; index <= limit; ++index){
        outList<<QVariantHash{{"uuid",QUuid::createUuid()}, {"index", index}};

    }
    return !outList.isEmpty();
}



ControllerMethods::ControllerMethods(QObject *parent) : QRpc::QRPCController(parent)
{

}

bool ControllerMethods::authorization()
{
    //code to check
    // ex: check acl
    // check class QRPCRequest to request
    return true;
}

QVariant ControllerMethods::listObjects()
{
    QRPC_METHOD_CHECK_GET();
    QRPC_V_SET_INT(limit);
    QRPC_V_CHECK_BODY_PARSER();//check obrigatory parameters
    QVariantList outList;

    if(!makeObjectList(limit, outList)){
        this->rq().co().setNoContent();
        return {};
    }

    return outList;//response to convert in bytes, json, xml, cbor
}
