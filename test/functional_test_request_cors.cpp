#ifndef Q_RPC_RequestFunctionalCORS_H
#define Q_RPC_RequestFunctionalCORS_H

#include "./qrpc_test_functional.h"

namespace QRpc {

class Q_RPC_RequestFunctionalCORS : public SDKGoogleTestFunctional {
public:

    bool checkResponseHeader(QRPCHttpResponse &response){

        const auto&responseHeader=response.header().rawHeader();
        static auto headersName=QStringList()<<qsl("Date")<<qsl("Server")<<qsl("Access-Control-Allow-Origin")<<qsl("Access-Control-Allow-Methods")<<qsl("Access-Control-Allow-Headers")<<qsl("Access-Control-Max-Age")<<qsl("Vary");
        for(auto&v:headersName){
            if(!responseHeader.contains(v)){
                sWarning()<<v;
                return false;
            }
        }
        return true;
    }

};

QRPC_DECLARE_REQUEST_CLASS(Q_RPC_RequestFunctionalV1CORS,QRpc::AppJson,/)

#define Q_RPC_RequestFunctionalCORSV1(request)\
        Q_RPC_RequestFunctionalV1CORS request;\
        request.header().addRawHeader("Origin", "localhost");\
        request.header().addRawHeader("Accept", "*/*");\
        request.header().addRawHeader("Accept-Language: en-US,en;q=0.5", "*/*");\
        request.header().addRawHeader("Accept-Encoding: gzip, deflate", "*/*");\
        request.header().addRawHeader("Access-Control-Request-Method: POST", "*/*");\
        request.header().addRawHeader("Access-Control-Request-Headers: authorization,content-type", "*/*");\
        request.header().addRawHeader("Referer: http://localhost:9999", "*/*");\
        request.header().addRawHeader("Connection","keep-alive");\
        request.setPort(public_record.server_port_http);\

TEST_F(Q_RPC_RequestFunctionalCORS, serviceStart)
{
    EXPECT_EQ(this->serviceStart(),true)<<"fail: service start";
}

TEST_F(Q_RPC_RequestFunctionalCORS, check_header)
{
    Q_RPC_RequestFunctionalCORSV1(request);

    auto&response=request.call(QRpc::Options,"check_header",QVariant());
    EXPECT_EQ(response.isOk(),true)<<"fail";
    EXPECT_EQ(this->checkResponseHeader(response),true)<<"invalid response header";
    EXPECT_EQ(response.body().isEmpty(),true)<<"fail";
}

TEST_F(Q_RPC_RequestFunctionalCORS, check_get)
{
    Q_RPC_RequestFunctionalCORSV1(request);
    auto&response=request.call(QRpc::Options,"check_get",QVariant());
    EXPECT_EQ(response.isOk(),true)<<"fail";
    EXPECT_EQ(this->checkResponseHeader(response),true)<<"invalid response header";
    EXPECT_EQ(response.body().isEmpty(),true)<<"fail";
}

TEST_F(Q_RPC_RequestFunctionalCORS, check_post)
{
    Q_RPC_RequestFunctionalCORSV1(request);
    auto&response=request.call(QRpc::Options,"check_post",QVariant());
    EXPECT_EQ(response.isOk(),true)<<"fail";
    EXPECT_EQ(this->checkResponseHeader(response),true)<<"invalid response header";
    EXPECT_EQ(response.body().isEmpty(),true)<<"fail";
}

TEST_F(Q_RPC_RequestFunctionalCORS, check_put)
{
    Q_RPC_RequestFunctionalCORSV1(request);
    auto&response=request.call(QRpc::Options,"check_put",QVariant());
    EXPECT_EQ(response.isOk(),true)<<"fail";
    EXPECT_EQ(this->checkResponseHeader(response),true)<<"invalid response header";
    EXPECT_EQ(response.body().isEmpty(),true)<<"fail";
}

TEST_F(Q_RPC_RequestFunctionalCORS, check_delete)
{
    Q_RPC_RequestFunctionalCORSV1(request);
    auto&response=request.call(QRpc::Options,"check_delete",QVariant());
    EXPECT_EQ(response.isOk(),true)<<"fail";
    EXPECT_EQ(this->checkResponseHeader(response),true)<<"invalid response header";
    EXPECT_EQ(response.body().isEmpty(),true)<<"fail";
}

TEST_F(Q_RPC_RequestFunctionalCORS, serviceStop)
{
    EXPECT_EQ(this->serviceStop(),true)<<"fail: service stop";
}

}

#endif
