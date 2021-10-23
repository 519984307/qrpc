#ifndef FormattingUtilTestUnit_H
#define FormattingUtilTestUnit_H

#include "./qrpc_test_unit.h"

class FormattingUtilTestUnit : public SDKGoogleTestUnit {
public:
};

TEST_F(FormattingUtilTestUnit, serviceStart)
{
    EXPECT_EQ(this->serviceStart(),true)<<"fail: service start";
}

TEST_F(FormattingUtilTestUnit, serviceStop)
{
    EXPECT_EQ(this->serviceStop(),true)<<"fail: service stop";
}

#endif // FormattingUtilTestUnit_H
