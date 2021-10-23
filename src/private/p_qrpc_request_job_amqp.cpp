#include "./p_qrpc_request_job_amqp.h"

namespace QRpc {

AMQPPool &QRPCRequestJobAMQP::___pool(){
    static AMQPPool*pool=nullptr;
    if(pool==nullptr){
        pool=new AMQPPool();
        pool->setObjectName("poolC");
        pool->start();
    }
    return*pool;
}

}


