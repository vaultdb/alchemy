//
// Created by Jennie Rogers on 8/14/20.
//


#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <emp-tool/emp-tool.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <util/emp_manager.h>


using namespace emp;
using namespace vaultdb::types;



DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54321, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");

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






int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}





/*
 * TODO(jennie): demo how to use constexpr or const eval to declare and evaluate predicates w/in ops
 * May need to upgrade to C++20 to make this happen
 *
 * constexpr Value predicate(Value a, Value b, Value c)
{
    return (a + b) < c;
}


// demo passing in an expression to query operator
TEST_F(ValueExpressionTest, test_int32_expr) {

    Value a((int32_t) 5);
    Value b((int32_t) 10);
    Value c((int32_t) 25);




    ASSERT_EQ(sum.getInt32(), 15);

}*/
