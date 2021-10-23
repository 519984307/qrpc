#ifndef Q_RPC_RequestFunctionalBrokerDatabase_H
#define Q_RPC_RequestFunctionalBrokerDatabase_H

#include "./qrpc_test_functional.h"

namespace QRpc {

class Q_RPC_RequestFunctionalBrokerDatabase : public SDKGoogleTestFunctional {
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


QRPC_DECLARE_REQUEST_CLASS(Q_RPC_RequestFunctionalBrokerDatabaseV1,QRpc::AppJson,/)

#define Q_RPC_RequestFunctionalBrokerDatabaseV1(request)\
        Q_RPC_RequestFunctionalBrokerDatabaseV1 request;\
        request.header().addRawHeader("Origin", "localhost");\
        request.header().addRawHeader("Accept", "*/*");\
        request.header().addRawHeader("Accept-Language: en-US,en;q=0.5", "*/*");\
        request.header().addRawHeader("Accept-Encoding: gzip, deflate", "*/*");\
        request.header().addRawHeader("Access-Control-Request-Method: POST", "*/*");\
        request.header().addRawHeader("Access-Control-Request-Headers: authorization,content-type", "*/*");\
        request.header().addRawHeader("Referer: http://localhost:9999", "*/*");\
        request.header().addRawHeader("Connection","keep-alive");\
        request.setProtocol(QRpc::DataBase);\
        request.setDriver(public_record.server_broker_database_driver);\
        request.setUserName(public_record.server_broker_database_username);\
        request.setPassword(public_record.server_broker_database_password);\
        request.setPort(public_record.server_broker_database_port);

TEST_F(Q_RPC_RequestFunctionalBrokerDatabase, serviceStart)
{
    EXPECT_EQ(this->serviceStart(),true)<<"fail: service start";
}
/*


TEST_F(Q_RPC_RequestFunctionalBrokerDatabase, check_header)
{
    Q_RPC_RequestFunctionalBrokerDatabaseV1(request);

    auto&response=request.call(QRpc::Options,"check_header",QVariant());
    EXPECT_EQ(response.isOk(),true)<<"fail";
    EXPECT_EQ(this->checkResponseHeader(response),true)<<"invalid response header";
    EXPECT_EQ(response.body().isEmpty(),true)<<"fail";
}

TEST_F(Q_RPC_RequestFunctionalBrokerDatabase, check_get)
{
    Q_RPC_RequestFunctionalBrokerDatabaseV1(request);
    auto&response=request.call(QRpc::Options,"check_get",QVariant());
    EXPECT_EQ(response.isOk(),true)<<"fail";
    EXPECT_EQ(this->checkResponseHeader(response),true)<<"invalid response header";
    EXPECT_EQ(response.body().isEmpty(),true)<<"fail";
}

TEST_F(Q_RPC_RequestFunctionalBrokerDatabase, check_post)
{
    Q_RPC_RequestFunctionalBrokerDatabaseV1(request);
    auto&response=request.call(QRpc::Options,"check_post",QVariant());
    EXPECT_EQ(response.isOk(),true)<<"fail";
    EXPECT_EQ(this->checkResponseHeader(response),true)<<"invalid response header";
    EXPECT_EQ(response.body().isEmpty(),true)<<"fail";
}

TEST_F(Q_RPC_RequestFunctionalBrokerDatabase, check_put)
{
    Q_RPC_RequestFunctionalBrokerDatabaseV1(request);
    auto&response=request.call(QRpc::Options,"check_put",QVariant());
    EXPECT_EQ(response.isOk(),true)<<"fail";
    EXPECT_EQ(this->checkResponseHeader(response),true)<<"invalid response header";
    EXPECT_EQ(response.body().isEmpty(),true)<<"fail";
}

TEST_F(Q_RPC_RequestFunctionalBrokerDatabase, check_delete)
{
    Q_RPC_RequestFunctionalBrokerDatabaseV1(request);
    auto&response=request.call(QRpc::Options,"check_delete",QVariant());
    EXPECT_EQ(response.isOk(),true)<<"fail";
    EXPECT_EQ(this->checkResponseHeader(response),true)<<"invalid response header";
    EXPECT_EQ(response.body().isEmpty(),true)<<"fail";
}
*/

TEST_F(Q_RPC_RequestFunctionalBrokerDatabase, serviceStop)
{
    EXPECT_EQ(this->serviceStop(),true)<<"fail: service stop";
}

}

#endif
