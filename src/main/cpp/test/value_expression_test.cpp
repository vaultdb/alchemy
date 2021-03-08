#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <emp-tool/emp-tool.h>

#include <query_table/field/field.h>

using namespace emp;
using namespace vaultdb;


class ValueExpressionTest : public ::testing::Test {


protected:
    void SetUp() override{
        emp::setup_plain_prot(false, "");
    };

    void TearDown() override{
        emp::finalize_plain_prot();
    };
};





TEST_F(ValueExpressionTest, test_int32) {

    Field a = Field::createInt32((int32_t) 5);
    Field b = Field::createInt32((int32_t) 5);

    IntField b((int32_t) 10);

    IntField sum = a + b;
    ASSERT_EQ(sum.primitive(), 15);

}



TEST_F(ValueExpressionTest, test_int32_comparator) {

    IntField a((int32_t) 5);
    IntField b((int32_t) 10);

    BoolField gt = &(a < b);
    ASSERT_EQ(gt.primitive(), true);

}







// demo passing in an expression to query operator
TEST_F(ValueExpressionTest, test_int32_expr) {

    IntField a((int32_t) 5);
    IntField b((int32_t) 10);
    IntField c((int32_t) 25);

    IntField sum = a  + b;
    BoolField compare = &(sum < c);




    ASSERT_EQ(sum.primitive(), 15);
    ASSERT_EQ(compare.primitive(), true);

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}







