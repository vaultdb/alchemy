#include <operators/secure_sql_input.h>
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <operators/project.h>
#include <test/mpc/emp_base_test.h>
#include <operators/sort.h>
#include <query_table/secure_tuple.h>




DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54333, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for execution");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(alice_db, "tpch_alice_150", "alice db name");
DEFINE_string(bob_db, "tpch_bob_150", "bob db name");
DEFINE_int32(cutoff, 100, "limit clause for queries");
DEFINE_string(storage, "row", "storage model for tables (row or column)");
DEFINE_int32(ctrl_port, 65475, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");


using namespace vaultdb;

class SecureSortTest : public EmpBaseTest {
protected:



};



TEST_F(SecureSortTest, tpchQ01Sort) {

    string sql = "SELECT l_returnflag, l_linestatus FROM lineitem WHERE l_orderkey <= "   + std::to_string(FLAGS_cutoff) + " ORDER BY l_comment";


    SortDefinition sort_def{ColumnSort(0, SortDirection::ASCENDING),
            ColumnSort(1, SortDirection::ASCENDING)};


    auto input = new SecureSqlInput(db_name_, sql, false);
    Sort<emp::Bit> sort(input, sort_def);

    auto sorted = sort.run();

    if(FLAGS_validation) {

        auto observed = sorted->reveal();
        ASSERT_TRUE(DataUtilities::verifyCollation(observed));
        string expected_results_sql = "WITH input AS ("
                                      + sql
                                      + ") SELECT * FROM input ORDER BY l_returnflag, l_linestatus";

        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_results_sql, false);
        expected->setSortOrder(sort_def);

        ASSERT_EQ(*expected, *observed);

        delete expected;
        delete observed;

    }


}


TEST_F(SecureSortTest, tpchQ03Sort) {
    string sql = "SELECT l_orderkey, l_linenumber, l.l_extendedprice * (1 - l.l_discount) revenue, o.o_shippriority, o_orderdate FROM lineitem l JOIN orders o ON l_orderkey = o_orderkey WHERE l_orderkey <= "  + std::to_string(FLAGS_cutoff) +  " ORDER BY l_comment";

    SortDefinition sort_def{ColumnSort (2, SortDirection::DESCENDING),
        ColumnSort (4, SortDirection::ASCENDING)};


    auto input = new SecureSqlInput(db_name_, sql, false);

    Sort<Bit> sort(input, sort_def);
     auto sorted = sort.run();

    if(FLAGS_validation) {

        PlainTable *observed = sorted->reveal();
        string expected_sql = "WITH input AS (" + sql + ") SELECT l_orderkey, l_linenumber, revenue, o_shippriority, " + DataUtilities::queryDatetime("o_orderdate") + " FROM input ORDER BY revenue DESC, o_orderdate";

        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_sql, false);
        expected->setSortOrder(sort_def);

//     commented out because of floating point comparison issues, validating with *expected instead
//        ASSERT_TRUE(DataUtilities::verifyCollation(observed));
        ASSERT_EQ(*expected, *observed);

        delete expected;
        delete observed;

    }

}


TEST_F(SecureSortTest, tpchQ05Sort) {

    string sql = "SELECT l_orderkey, l_linenumber, l.l_extendedprice * (1 - l.l_discount) revenue FROM lineitem l WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + "  ORDER BY l_comment"; // order by to ensure order is reproducible and not sorted on the sort cols

    SortDefinition sort_definition{ColumnSort(2, SortDirection::DESCENDING)};

    auto input = new SecureSqlInput(db_name_, sql, false);

    Sort<Bit> sort(input, sort_definition);
    auto sorted = sort.run();
    
    if(FLAGS_validation) {

        PlainTable *observed  = sorted->revealInsecure();

        ASSERT_TRUE(DataUtilities::verifyCollation(observed));

        string expected_sql = "WITH input AS (" + sql + ") SELECT * FROM input ORDER BY revenue DESC";
        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_sql, false);
        expected->setSortOrder(observed->getSortOrder());

        ASSERT_EQ(*expected, *observed);

        delete expected;
        delete observed;


    }

}



TEST_F(SecureSortTest, tpchQ08Sort) {

    string sql = "SELECT  o_orderyear, o_orderkey FROM orders o  WHERE o_orderkey <= " + std::to_string(FLAGS_cutoff) + " ORDER BY o_comment, o_orderkey"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expected_sql = "WITH input AS (" + sql + ") SELECT * FROM input ORDER BY o_orderyear, o_orderkey DESC";  // orderkey DESC needed to align with psql's layout

    SortDefinition sort_def {ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};

    auto input = new SecureSqlInput(db_name_, sql, false);
    auto sort = new Sort<emp::Bit>(input, sort_def);
    auto sorted = sort->run();

    if(FLAGS_validation) {
        PlainTable *observed  = sorted->reveal();
        ASSERT_TRUE(DataUtilities::verifyCollation(observed));

        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_sql, false);
        expected->setSortOrder(observed->getSortOrder());

        ASSERT_EQ(*expected, *observed);

        delete observed;
        delete expected;

    }

    delete sort;
}


// this test is intentionally set to a large input size to test join scaling
// outside of cutoff
// sort scale up has surfaced many bugs in the past, thus having it larger than most.
TEST_F(SecureSortTest, tpchQ09Sort) {

    std::string sql = "SELECT o_orderyear, o_orderkey, n_name FROM orders o JOIN lineitem l ON o_orderkey = l_orderkey"
                      "  JOIN supplier s ON s_suppkey = l_suppkey"
                      "  JOIN nation on n_nationkey = s_nationkey"
                      " ORDER BY l_comment LIMIT 1000";
    SortDefinition sort_definition{ColumnSort(2, SortDirection::ASCENDING),
                                   ColumnSort(0, SortDirection::DESCENDING)};


    auto input = new SecureSqlInput(db_name_, sql, false);
    Sort sort(input, sort_definition);
    auto sorted = sort.run();

    if(FLAGS_validation) {
        PlainTable *observed = sorted->reveal();
        DataUtilities::verifyCollation(observed);
        delete observed;
    }




}

// 18
TEST_F(SecureSortTest, tpchQ18Sort) {
    string sql = "SELECT o_orderkey, o_orderdate, o_totalprice FROM orders WHERE o_orderkey <= " + std::to_string(FLAGS_cutoff) + " "
                        " ORDER BY o_comment, o_custkey, o_orderkey"; // order by to ensure order is reproducible and not sorted on the sort cols



    SortDefinition sort_definition;
    sort_definition.emplace_back(2, SortDirection::DESCENDING);
    sort_definition.emplace_back(1, SortDirection::ASCENDING);

    auto input = new SecureSqlInput(db_name_, sql, false);
    Sort<Bit> sort(input, sort_definition);
    auto sorted = sort.run();

    if(FLAGS_validation) {
        PlainTable *observed = sorted->reveal();
        DataUtilities::verifyCollation(observed);
        delete observed;
    }

}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

	::testing::GTEST_FLAG(filter)=FLAGS_filter;
    return RUN_ALL_TESTS();
}




