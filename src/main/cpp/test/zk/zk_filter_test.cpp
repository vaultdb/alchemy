#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <util/data_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/filter.h>
#include <expression/comparator_expression_nodes.h>
#include <query_table/secure_tuple.h>
#include <test/zk/zk_base_test.h>
#include <emp-zk/emp-zk.h>


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

   // BoolIO<NetIO>* ios[threads_];

    shared_ptr<SecureTable> secret_share_input() const;

};

void ZkFilterTest::SetUp() {
    ZkTest::SetUp();
    sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem ORDER BY (1), (2) LIMIT " + std::to_string(tuple_cnt);

}



shared_ptr<SecureTable> ZkFilterTest::secret_share_input() const {
    SortDefinition  order_by = DataUtilities::getDefaultSortDefinition(2);

    // only alice can secret share in ZK land
    if(FLAGS_party == emp::ALICE) {
        SqlInput read_db(db_name, sql, false, order_by);
        shared_ptr<PlainTable> input = read_db.run();
        return PlainTable::secret_share_send_table(input, ios_[0]->io, FLAGS_party);
    }
    else if(FLAGS_party == emp::BOB){ // BOB
        // not input, just grabbing schema
        SqlInput read_db(db_name, sql, false);
        shared_ptr<PlainTable> input = read_db.run();
        QuerySchema schema = *input->getSchema();
        return PlainTable ::secret_share_recv_table( schema, order_by, ios_[0]->io, emp::ALICE);
    }

    throw std::invalid_argument("Not a valid party for secret sharing!");

}

TEST_F(ZkFilterTest, test_table_scan) {

    // smoke test for basic infrastructure
    SortDefinition  order_by = DataUtilities::getDefaultSortDefinition(2);
    std::shared_ptr<SecureTable> shared = ZkTest::secret_share_input(sql, false, order_by);

    std::unique_ptr<PlainTable> revealed = shared->reveal(emp::PUBLIC);
    std::shared_ptr<PlainTable> expected = DataUtilities::getExpectedResults(alice_db, sql, false, 2);

    ASSERT_EQ(*expected, *revealed);
    // cheat check handled in ZkBaseTest


}




// Testing for selecting l_linenumber=1

TEST_F(ZkFilterTest, test_filter) {
    std::string expected_result_sql = "WITH input AS (" + sql + ") SELECT *, l_linenumber<>1 dummy FROM input";

    SortDefinition  order_by = DataUtilities::getDefaultSortDefinition(2);
    std::shared_ptr<SecureTable> shared = ZkTest::secret_share_input(sql, false, order_by);

    // expression setup
    // filtering for l_linenumber = 1
    shared_ptr<InputReferenceNode<emp::Bit> > read_field(new InputReferenceNode<emp::Bit>(1));
    Field<emp::Bit> one(FieldType::SECURE_INT, emp::Integer(32, 1));
    shared_ptr<LiteralNode<emp::Bit> > constant_input(new LiteralNode<emp::Bit>(one));
    shared_ptr<ExpressionNode<emp::Bit> > equality_check(new EqualNode<emp::Bit>(read_field, constant_input));
    Expression<emp::Bit> * expression(equality_check, "selection_criteria", FieldType::SECURE_BOOL);

    Filter<emp::Bit> filter(shared, expression);  // deletion handled by shared_ptr

    std::shared_ptr<SecureTable> result = filter.run();
    std::unique_ptr<PlainTable> revealed = result->reveal(emp::PUBLIC);

    std::shared_ptr<PlainTable> expected = DataUtilities::getExpectedResults(alice_db, expected_result_sql, true, 2);

    ASSERT_EQ(*expected,  *revealed);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}


