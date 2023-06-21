#include <data/psql_data_provider.h>
#include <test/mpc/emp_base_test.h>
#include <util/data_utilities.h>
#include <operators/sort.h>


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54324, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");
DEFINE_string(storage, "row", "storage model for tables (row or column)");

using namespace vaultdb;

class EmpTableTest : public EmpBaseTest {
protected:
    void secretShareAndValidate(const std::string & input_query, const SortDefinition & sort = SortDefinition());
};





// test encrypting a query table with a single int in EMP
TEST_F(EmpTableTest, encrypt_table_one_column) {


    std::string input_query =  "SELECT l_orderkey FROM lineitem ORDER BY l_orderkey, l_linenumber LIMIT 10";

    SortDefinition  sort_definition = DataUtilities::getDefaultSortDefinition(0);

    secretShareAndValidate(input_query, sort_definition);



}


// test encrypting a query table with a single int in EMP
TEST_F(EmpTableTest, encrypt_table_varchar) {

    std::string input_query =  "SELECT l_comment FROM lineitem ORDER BY l_orderkey, l_linenumber LIMIT 10";
    secretShareAndValidate(input_query);

}


//TEST_F(EmpTableTest, tpch_q1_mockup) {
//    this->disableBitPacking();
//    string sql = "SELECT  l_returnflag,  l_linestatus,  SUM(l_quantity) as sum_qty, SUM(l_extendedprice) as sum_base_price, SUM(l_extendedprice * (1 - l_discount)) as sum_disc_price,  SUM(l_extendedprice * (1 - l_discount) * (1 + l_tax)) as sum_charge, AVG(l_quantity) as avg_qty,  AVG(l_extendedprice) as avg_price,  AVG(l_discount) as avg_disc,   COUNT(*)::BIGINT as count_order \n"
//                 "FROM  lineitem \n"
//                 "WHERE  l_shipdate <= date '1998-08-03' \n"
//                 "GROUP BY  l_returnflag, l_linestatus \n"
//                 "ORDER BY  l_returnflag,  l_linestatus";
//    secretShareAndValidate(sql);
//}

TEST_F(EmpTableTest, encrypt_table_two_cols) {

    std::string input_query = "SELECT l_orderkey, l_comment "
                             "FROM lineitem "
                             "ORDER BY l_orderkey, l_linenumber "
                             "LIMIT 10";
    secretShareAndValidate(input_query);


}



// test more column types
TEST_F(EmpTableTest, encrypt_table) {

    std::string input_query =  "SELECT l_orderkey, l_comment, l_returnflag, l_discount, "
                               "CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) AS l_commitdate "  // handle timestamps by converting them to longs using SQL - "CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) AS l_commitdate,
                               "FROM lineitem "
                               "ORDER BY l_orderkey "
                               "LIMIT 5";
    secretShareAndValidate(input_query);


}


TEST_F(EmpTableTest, encrypt_table_dummy_tag) {


    std::string input_query = "SELECT l_orderkey, l_comment, l_returnflag, l_discount, "
                              "CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) AS l_commitdate, "  // handle timestamps by converting them to longs using SQL - "CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) AS l_commitdate,
                              "l_returnflag <> 'N' AS dummy "  // simulate a filter for l_returnflag = 'N' -- all of the ones that dont match are dummies
                              "FROM lineitem "
                              "ORDER BY l_orderkey "
                              "LIMIT 10";
    secretShareAndValidate(input_query);


}


TEST_F(EmpTableTest, bit_packing_test) {


    std::string input_query = "SELECT c_custkey, c_nationkey FROM customer ORDER BY (1) LIMIT 20";

    PsqlDataProvider data_provider;
    PlainTable *input_table = data_provider.getQueryTable(db_name_,
                                                          input_query, false);

    SecureTable *secret_shared = input_table->secretShare();
    manager_->flush();

    // c_custkey has 150 distinct vals, should have 8 bits
    ASSERT_EQ(8, secret_shared->getSchema().getField(0).size());
    // c_nationkey has 25 distinct vals, should have 5 bits
    ASSERT_EQ(5, secret_shared->getSchema().getField(1).size());

    PlainTable *expected = DataUtilities::getUnionedResults(alice_db_, bob_db_, input_query, storage_model_, false);


    // tuple_cnt * (5+8+1 (for dummy tag) )*sizeof(emp::Bit)
    ASSERT_EQ(expected->getTupleCount()*14 * TypeUtilities::getEmpBitSize(),  secret_shared->getTupleCount() * secret_shared->tuple_size_);

    PlainTable *revealed = secret_shared->reveal(emp::PUBLIC);



    ASSERT_EQ(*expected, *revealed) << "Query table was not processed correctly.";

    delete input_table;
    delete expected;
    delete revealed;
    delete secret_shared;

}

void EmpTableTest::secretShareAndValidate(const std::string & input_query, const SortDefinition & sort) {

    PsqlDataProvider dataProvider;
    PlainTable *input_table = dataProvider.getQueryTable(db_name_,
                                                         input_query, false);

    input_table->setSortOrder(sort);
    SecureTable *secret_shared = input_table->secretShare();
    manager_->flush();
    cout << "Secret shared schema: " << secret_shared->getSchema() << ", size=" << secret_shared->tuple_size_ << endl;
    PlainTable *revealed = secret_shared->reveal(emp::PUBLIC);

    PlainTable *expected = DataUtilities::getUnionedResults(alice_db_, bob_db_, input_query, storage_model_, false);

    if(!sort.empty())  {
        Sort sorter(expected, sort);
        expected = sorter.run()->clone();

    }

    ASSERT_EQ(*expected, *revealed);

    delete revealed;
    delete input_table;
    delete secret_shared;
    delete expected;
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);
    return RUN_ALL_TESTS();
}

