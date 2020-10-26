//
// Created by Jennie Rogers on 8/14/20.
//


#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>


using namespace emp;
using namespace vaultdb::types;



class ValueExpressionTest : public ::testing::Test {


protected:
    void SetUp() override {};
    void TearDown() override{};
};





TEST_F(ValueExpressionTest, test_int32) {

    Value a((int32_t) 5);
    Value b((int32_t) 10);

    Value sum = a + b;
    ASSERT_EQ(sum.getInt32(), 15);

}



TEST_F(ValueExpressionTest, test_int32_comparator) {

    Value a((int32_t) 5);
    Value b((int32_t) 10);

    Value gt = a < b;
    ASSERT_EQ(gt.getBool(), true);

}







// demo passing in an expression to query operator
TEST_F(ValueExpressionTest, test_int32_expr) {

    Value a((int32_t) 5);
    Value b((int32_t) 10);
    Value c((int32_t) 25);

    Value sum = a  + b;
    Value compare = sum < c;




    ASSERT_EQ(sum.getInt32(), 15);
    ASSERT_EQ(compare.getBool(), true);

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}







