#include <util/data_utilities.h>
#include "plain/plain_base_test.h"
#include "query_table/query_table.h"


DEFINE_string(storage, "row", "storage model for tables (row or column)");

class FieldExpressionTest : public  PlainBaseTest { };





TEST_F(FieldExpressionTest, test_int32) {

    Field<bool> a(FieldType::INT, 5);
    Field<bool> b(FieldType::INT, 10);

    Field<bool> sum = a + b;
    ASSERT_EQ(sum.getValue<int32_t>(), 15);

}



TEST_F(FieldExpressionTest, test_int32_comparator) {

    Field<bool> a(FieldType::INT,  5);
    Field<bool> b(FieldType::INT, 10);

    bool gt = (a > b);
    ASSERT_EQ(gt, false);

}







// demo passing in an expression to query operator
TEST_F(FieldExpressionTest, test_int32_expr) {

    Field<bool> a(FieldType::INT, 5);
    Field<bool> b(FieldType::INT, 10);
    Field<bool> c(FieldType::INT, 25);


    Field<bool> sum = a  + b;
    bool compare = (sum < c);


    ASSERT_EQ(sum.getValue<int32_t>(), 15);
    ASSERT_EQ(compare, true);

}

TEST_F(FieldExpressionTest, cmp_swap) {
    string sql = "SELECT * FROM lineitem ORDER BY l_comment LIMIT 3"; // order by to ensure order is reproducible and not sorted on the sort cols
    PlainTable *data = DataUtilities::getQueryResults(db_name_, sql, storage_model_, false);
    QuerySchema q = data->getSchema();
    // deep copy
    PlainTuple a(&q), b(&q);
    a = data->getPlainTuple(0);
    b = data->getPlainTuple(1);
    bool swap(true);

    PlainTuple::compareSwap(swap, a, b);

    // swapped
    ASSERT_EQ(a, data->getPlainTuple(1));
    ASSERT_EQ(b, data->getPlainTuple(0));


    // no swap
    swap = false;
    PlainTuple::compareSwap(swap, a, b);
    ASSERT_EQ(a, data->getPlainTuple(1));
    ASSERT_EQ(b, data->getPlainTuple(0));

    delete data;

}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}







