#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <util/data_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/filter.h>
#include <expression/comparator_expression_nodes.h>
#include <expression/generic_expression.h>
#include <query_table/secure_tuple.h>
#include <test/zk/zk_base_test.h>
#include <emp-zk/emp-zk.h>
#include <operators/zk_sql_input.h>

using namespace emp;
using namespace vaultdb;


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 65432, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");



class ZkFilterTest : public ZkTest {
protected:
    void SetUp() override;
    static const int tuple_cnt = 10;
    std::string sql;


    SecureTable *secret_share_input() const;

};

void ZkFilterTest::SetUp() {
    ZkTest::SetUp();
    sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem ORDER BY (1), (2) LIMIT " + std::to_string(tuple_cnt);

}



SecureTable *ZkFilterTest::secret_share_input() const {
    SortDefinition  order_by = DataUtilities::getDefaultSortDefinition(2);

        SqlInput read_db(db_name_, sql, false, order_by);
        PlainTable *input = read_db.run();
        return input->secretShare();

}

TEST_F(ZkFilterTest, test_table_scan) {

    // smoke test for basic infrastructure
    SortDefinition  order_by = DataUtilities::getDefaultSortDefinition(2);
    auto shared = new ZkSqlInput(db_name_, sql, false, order_by);


    PlainTable *revealed = shared->run()->reveal(emp::PUBLIC);
    PlainTable *expected = DataUtilities::getExpectedResults(unioned_db_, sql, false, 2);

    ASSERT_EQ(*expected, *revealed);
    // cheat check handled in ZkBaseTest


}




// Testing for selecting l_linenumber=1

TEST_F(ZkFilterTest, test_filter) {
    std::string expected_result_sql = "WITH input AS (" + sql + ") SELECT *, l_linenumber<>1 dummy FROM input";

    SortDefinition  order_by = DataUtilities::getDefaultSortDefinition(2);
    auto shared = new ZkSqlInput(db_name_, sql, false, order_by);


    // expression setup
    // filtering for l_linenumber = 1
    auto read_field = new InputReference<emp::Bit>(1, shared->run()->getSchema().getField(1));
    Field<emp::Bit> one(FieldType::SECURE_INT, emp::Integer(32, 1));
    auto constant_input = new LiteralNode<emp::Bit>(one);
    auto equality_check = new EqualNode<emp::Bit>(read_field, constant_input);
    auto expression = new GenericExpression<Bit>(equality_check, "selection_criteria", FieldType::SECURE_BOOL);

    Filter<emp::Bit> filter(shared, expression);  // deletion handled by shared_ptr

    SecureTable *result = filter.run();
    PlainTable *revealed = result->reveal(emp::PUBLIC);

    PlainTable *expected = DataUtilities::getExpectedResults(unioned_db_, expected_result_sql, true, 2);

    ASSERT_EQ(*expected,  *revealed);
    delete expected;
    delete revealed;
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}


