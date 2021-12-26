#ifndef QRPC_RequestDownLoadUploadFunctional_H
#define QRPC_RequestDownLoadUploadFunctional_H

#include "./qrpc_test_functional.h"

namespace QRpc {

class Q_RPC_RequestDownLoadUploadFunctional : public SDKGoogleTestFunctional {
public:
};

TEST_F(Q_RPC_RequestDownLoadUploadFunctional, serviceStart)
{
    EXPECT_EQ(this->serviceStart(),true)<<"fail: service start";
}

TEST_F(Q_RPC_RequestDownLoadUploadFunctional, serviceStop)
{
    EXPECT_EQ(this->serviceStop(),true)<<"fail: service stop";
}

}

#endif // RequestDownLoadUploadFunctional_H
