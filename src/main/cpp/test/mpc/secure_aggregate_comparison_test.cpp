#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <util/type_utilities.h>
#include <util/data_utilities.h>
#include <test/mpc/emp_base_test.h>
#include <query_table/secure_tuple.h>
#include <test/support/tpch_queries.h>
#include <boost/algorithm/string/replace.hpp>
#include <parser/plan_parser.h>
#include <operators/secure_sql_input.h>
#include <operators/sort.h>
#include <operators/group_by_aggregate.h>
#include <operators/nested_loop_aggregate.h>



using namespace emp;
using namespace vaultdb;



DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 7654, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for EMP execution");
DEFINE_string(storage, "row", "storage model for tables (row or column)");
DEFINE_int32(ctrl_port, 65482, "port for managing EMP control flow by passing public values");
DEFINE_string(bitpacking, "packed", "bit packed or non-bit packed");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_int32(cutoff, 10, "cutoff for Operator Comparison");


class SecureAggregateComparisonTest : public EmpBaseTest {

protected:

    // depends on truncate-tpch-set.sql
    void controlBitPacking(const string &db_name);
    //void runTest_SMA(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name);
    //void runTest_NLA(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name);
    //string  generateExpectedOutputQuery(const int & test_id,  const SortDefinition &expected_sort,   const string &db_name);

    int cutoff_ = 100;
    int input_tuple_limit_ = -1;

};

// most of these runs are not meaningful for diffing the results because they produce no tuples - joins are too sparse.
// This isn't relevant to the parser so work on this elsewhere.

/*
void
SecureAggregateComparisonTest::runTest_SMA(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name) {

    if(FLAGS_bitpacking == "packed")
        this->initializeBitPacking(unioned_db_);
    else
        this->disableBitPacking();

    string expected_query = generateExpectedOutputQuery(test_id, expected_sort, db_name);
    string party_name = FLAGS_party == emp::ALICE ? "alice" : "bob";
    string local_db = db_name;
    boost::replace_first(local_db, "unioned", party_name.c_str());

    std::cout << "Setting : " ;
    if(FLAGS_bitpacking == "packed")
        std::cout << " Bit Packed, " << db_name << ",  cutoff : " << cutoff_ << std::endl;
    else
        std::cout<< " Non Bit Packed, " << db_name << ",  cutoff : " << cutoff_ << std::endl;


    PlainTable *expected = DataUtilities::getExpectedResults(db_name, expected_query, false, 0);
    expected->setSortOrder(expected_sort);

    //ASSERT_TRUE(!expected->empty()); // want all tests to produce output

    std::string sql_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_2/SMA/queries-" + test_name + ".sql";
    std::string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_2/SMA/mpc-"  + test_name + ".json";

    PlanParser<emp::Bit> parser(local_db, sql_file, plan_file, input_tuple_limit_);
    SecureOperator *root = parser.getRoot();

    std:cout << root->printTree() << endl;

    time_point<high_resolution_clock> startTime = clock_start();
    clock_t secureStartClock = clock();

    SecureTable *observed = root->run();

    double secureClockTicks = (double) (clock() - secureStartClock);
    double secureClockTicksPerSecond = secureClockTicks / ((double) CLOCKS_PER_SEC);
    double duration = time_from(startTime) / 1e6;

    cout << "SMA : \n";
    cout << "Time: " << duration << " sec, CPU clock ticks: " << secureClockTicks << ",CPU clock ticks per second: " << secureClockTicksPerSecond << "\n";

    PlainTable *observed_plain = observed->reveal();
    DataUtilities::removeDummies(observed_plain);

    ASSERT_EQ(*expected, *observed_plain);

    ASSERT_TRUE(!observed_plain->empty()); // want all tests to produce output
    delete observed_plain;
    delete observed;
    delete expected;
}

void
SecureAggregateComparisonTest::runTest_NLA(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name) {

    if(FLAGS_bitpacking == "packed")
        this->initializeBitPacking(unioned_db_);
    else
        this->disableBitPacking();

    string expected_query = generateExpectedOutputQuery(test_id, expected_sort, db_name);
    string party_name = FLAGS_party == emp::ALICE ? "alice" : "bob";
    string local_db = db_name;
    boost::replace_first(local_db, "unioned", party_name.c_str());

    std::cout << "Setting : " ;
    if(FLAGS_bitpacking == "packed")
        std::cout << " Bit Packed, " << db_name << ",  cutoff : " << cutoff_ << std::endl;
    else
        std::cout<< " Non Bit Packed, " << db_name << ",  cutoff : " << cutoff_ << std::endl;

    PlainTable *expected = DataUtilities::getExpectedResults(db_name, expected_query, false, 0);
    expected->setSortOrder(expected_sort);

    std::string sql_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_2/NLA/queries-" + test_name + ".sql";
    std::string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_2/NLA/mpc-"  + test_name + ".json";

    PlanParser<emp::Bit> parser(local_db, sql_file, plan_file, input_tuple_limit_);
    SecureOperator *root = parser.getRoot();

    std:cout << root->printTree() << endl;

    time_point<high_resolution_clock> startTime = clock_start();
    clock_t secureStartClock = clock();

    SecureTable *observed = root->run();

    double secureClockTicks = (double) (clock() - secureStartClock);
    double secureClockTicksPerSecond = secureClockTicks / ((double) CLOCKS_PER_SEC);
    double duration = time_from(startTime) / 1e6;

    cout << "NLA : \n";
    cout << "Time: " << duration << " sec, CPU clock ticks: " << secureClockTicks << ",CPU clock ticks per second: " << secureClockTicksPerSecond << "\n";

    PlainTable *observed_plain = observed->reveal();
    DataUtilities::removeDummies(observed_plain);

    ASSERT_EQ(*expected, *observed_plain);

    ASSERT_TRUE(!observed_plain->empty()); // want all tests to produce output
    delete observed_plain;
    delete observed;
    delete expected;
}

string
SecureAggregateComparisonTest::generateExpectedOutputQuery(const int &test_id, const SortDefinition &expected_sort, const string &db_name) {
    string alice_db = db_name;
    string bob_db = db_name;
    string query = aggregate_tpch_queries[test_id];
    boost::replace_all(query, "$cutoff", std::to_string(cutoff_));

    boost::replace_first(alice_db, "unioned", "alice");
    boost::replace_first(bob_db, "unioned", "bob");

    return query;
}
*/
void
SecureAggregateComparisonTest::controlBitPacking(const string &db_name) {
    if(FLAGS_bitpacking == "packed") {
        this->initializeBitPacking(db_name);
        std::cout << "Bit Packed - DB : " << db_name << "\n";
    }
    else {
        this->disableBitPacking();
        std::cout << "Non Bit Packed" << "\n";
    }
}

TEST_F(SecureAggregateComparisonTest, tpch_q1_NLA) {

    string unioned_db_name = "tpch_unioned";
    controlBitPacking(unioned_db_name);
    string local_db_name = (FLAGS_party == ALICE) ? "tpch_alice" : "tpch_bob";
    int cutoff = FLAGS_cutoff;

    std::cout << "Expected : " << unioned_db_name << " Observed : " << local_db_name << " cutoff : " << std::to_string(cutoff) << "\n";
    string input_rows = "SELECT * FROM lineitem WHERE l_orderkey IN (SELECT o_orderkey FROM orders WHERE o_custkey <= " + std::to_string(cutoff) + ") ORDER BY l_orderkey, l_linenumber";
    string sql = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, \n"
                 " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                 " FROM (" + input_rows + ") selection \n"
                                          " ORDER BY l_returnflag, l_linestatus";

    string expected_sql =  "select \n"
                           "  l_returnflag, \n"
                           "  l_linestatus, \n"
                           "  sum(l_quantity) as sum_qty, \n"
                           "  sum(l_extendedprice) as sum_base_price, \n"
                           "  sum(l_extendedprice * (1 - l_discount)) as sum_disc_price, \n"
                           "  sum(l_extendedprice * (1 - l_discount) * (1 + l_tax)) as sum_charge, \n"
                           "  avg(l_quantity) as avg_qty, \n"
                           "  avg(l_extendedprice) as avg_price, \n"
                           "  avg(l_discount) as avg_disc, \n"
                           "  count(*)::BIGINT as count_order \n"
                           "from (" + input_rows + ") input "
                                                   " where  l_shipdate <= date '1998-08-03'  "
                                                   "group by \n"
                                                   "  l_returnflag, \n"
                                                   "  l_linestatus \n"
                                                   " \n"
                                                   "order by \n"
                                                   "  l_returnflag, \n"
                                                   "  l_linestatus";

    std::vector<int32_t> group_bys{0, 1};
    std::vector<ScalarAggregateDefinition> aggregators{
            ScalarAggregateDefinition(2, vaultdb::AggregateId::SUM, "sum_qty"),
            ScalarAggregateDefinition(3, vaultdb::AggregateId::SUM, "sum_base_price"),
            ScalarAggregateDefinition(5, vaultdb::AggregateId::SUM, "sum_disc_price"),
            ScalarAggregateDefinition(6, vaultdb::AggregateId::SUM, "sum_charge"),
            ScalarAggregateDefinition(2, vaultdb::AggregateId::AVG, "avg_qty"),
            ScalarAggregateDefinition(3, vaultdb::AggregateId::AVG, "avg_price"),
            ScalarAggregateDefinition(4, vaultdb::AggregateId::AVG, "avg_disc"),
            ScalarAggregateDefinition(-1, vaultdb::AggregateId::COUNT, "count_order")};

    SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(2);
    auto input = new SecureSqlInput(local_db_name, sql, true);

    auto aggregate = new NestedLoopAggregate(input, group_bys, aggregators, 6);
    auto aggregated = aggregate->run();

    if(FLAGS_validation) {
        aggregated->setSortOrder(sort_def);
        PlainTable *observed = aggregated->reveal(PUBLIC);

        //auto sort = new Sort(observed, sort_def);
        //observed = sort->run();
        PlainTable *expected = DataUtilities::getExpectedResults(unioned_db_name, expected_sql, false, 2);

        ASSERT_EQ(*expected, *observed);

        delete observed;
        delete expected;
    }

    delete aggregate;

}

TEST_F(SecureAggregateComparisonTest, tpch_q1_SMA) {

    string unioned_db_name = "tpch_unioned";
    controlBitPacking(unioned_db_name);
    string local_db_name = (FLAGS_party == ALICE) ? "tpch_alice" : "tpch_bob";
    int cutoff = FLAGS_cutoff;

    std::cout << "Expected : " << unioned_db_name << " Observed : " << local_db_name << " cutoff : " << std::to_string(cutoff) << "\n";

    string input_rows = "SELECT * FROM lineitem WHERE l_orderkey IN (SELECT o_orderkey FROM orders WHERE o_custkey <= " + std::to_string(cutoff) + ") ORDER BY l_orderkey, l_linenumber";
    string sql = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, \n"
                 " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                 " FROM (" + input_rows + ") selection \n"
                                          " ORDER BY l_returnflag, l_linestatus";

    string expected_sql =  "select \n"
                           "  l_returnflag, \n"
                           "  l_linestatus, \n"
                           "  sum(l_quantity) as sum_qty, \n"
                           "  sum(l_extendedprice) as sum_base_price, \n"
                           "  sum(l_extendedprice * (1 - l_discount)) as sum_disc_price, \n"
                           "  sum(l_extendedprice * (1 - l_discount) * (1 + l_tax)) as sum_charge, \n"
                           "  avg(l_quantity) as avg_qty, \n"
                           "  avg(l_extendedprice) as avg_price, \n"
                           "  avg(l_discount) as avg_disc, \n"
                           "  count(*)::BIGINT as count_order \n"
                           "from (" + input_rows + ") input "
                                                   " where  l_shipdate <= date '1998-08-03'  "
                                                   "group by \n"
                                                   "  l_returnflag, \n"
                                                   "  l_linestatus \n"
                                                   " \n"
                                                   "order by \n"
                                                   "  l_returnflag, \n"
                                                   "  l_linestatus";


    std::vector<int32_t> groupByCols{0, 1};
    std::vector<ScalarAggregateDefinition> aggregators{
            ScalarAggregateDefinition(2, vaultdb::AggregateId::SUM, "sum_qty"),
            ScalarAggregateDefinition(3, vaultdb::AggregateId::SUM, "sum_base_price"),
            ScalarAggregateDefinition(5, vaultdb::AggregateId::SUM, "sum_disc_price"),
            ScalarAggregateDefinition(6, vaultdb::AggregateId::SUM, "sum_charge"),
            ScalarAggregateDefinition(2, vaultdb::AggregateId::AVG, "avg_qty"),
            ScalarAggregateDefinition(3, vaultdb::AggregateId::AVG, "avg_price"),
            ScalarAggregateDefinition(4, vaultdb::AggregateId::AVG, "avg_disc"),
            ScalarAggregateDefinition(-1, vaultdb::AggregateId::COUNT, "count_order")};

    SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(2);
    auto input = new SecureSqlInput(local_db_name, sql, true);
    auto sort = new Sort(input, sort_def);
    // sort alice + bob inputs after union
    // Sort sort(&input, sort_def);

    GroupByAggregate aggregate(sort, groupByCols, aggregators);

    auto aggregated = aggregate.run();
    if(FLAGS_validation) {

        PlainTable *observed = aggregated->reveal(PUBLIC);

        PlainTable *expected = DataUtilities::getExpectedResults(unioned_db_name, expected_sql, false, 2);
        ASSERT_EQ(*expected, *observed);
        delete observed;
        delete expected;
    }
}

TEST_F(SecureAggregateComparisonTest, tpch_q5_NLA) {

    string unioned_db_name = "tpch_unioned";
    controlBitPacking(unioned_db_name);
    string local_db_name = (FLAGS_party == ALICE) ? "tpch_unioned" : "tpch_empty";
    int cutoff = FLAGS_cutoff;

    std::cout << "Expected : " << unioned_db_name << " Observed : " << local_db_name << " cutoff : " << std::to_string(cutoff) << "\n";

    string sql = "SELECT n.n_name, l.l_extendedprice * (1 - l.l_discount) as revenue, NOT (c.c_nationkey = s.s_nationkey  AND o.o_orderdate >= date '1993-01-01' AND o.o_orderdate < date '1994-01-01') AS dummy_tag\n"
                 "                 FROM  customer c JOIN orders o ON c.c_custkey = o.o_custkey\n"
                 "                     JOIN lineitem l ON l.l_orderkey = o.o_orderkey\n"
                 "                     JOIN supplier s ON l.l_suppkey = s.s_suppkey\n"
                 "                     JOIN nation n ON s.s_nationkey = n.n_nationkey\n"
                 "                     JOIN region r ON n.n_regionkey = r.r_regionkey\n"
                 "                WHERE r.r_name = 'EUROPE' AND c_custkey < " + std::to_string(cutoff) + " ORDER BY n.n_name";


    string expected_sql =  "SELECT n.n_name, SUM(l.l_extendedprice * (1 - l.l_discount)) as revenue\n"
                           " FROM  customer c JOIN orders o ON c.c_custkey = o.o_custkey\n"
                           "     JOIN lineitem l ON l.l_orderkey = o.o_orderkey\n"
                           "     JOIN supplier s ON l.l_suppkey = s.s_suppkey\n"
                           "     JOIN nation n ON s.s_nationkey = n.n_nationkey\n"
                           "     JOIN region r ON n.n_regionkey = r.r_regionkey\n"
                           "WHERE c.c_nationkey = s.s_nationkey  AND r.r_name = 'EUROPE' AND o.o_orderdate >= date '1993-01-01' AND o.o_orderdate < date '1994-01-01' AND c_custkey < " + std::to_string(cutoff) +
                           " GROUP BY   n.n_name\n"
                           " ORDER BY n.n_name";

    std::vector<int32_t> group_bys{0};
    std::vector<ScalarAggregateDefinition> aggregators{
            ScalarAggregateDefinition(1, vaultdb::AggregateId::SUM, "revenue")};

    SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(1);
    auto input = new SecureSqlInput(local_db_name, sql, true);

    auto aggregate = new NestedLoopAggregate(input, group_bys, aggregators, 5);
    auto aggregated = aggregate->run();

    if(FLAGS_validation) {
        aggregated->setSortOrder(sort_def);
        PlainTable *observed = aggregated->reveal(PUBLIC);

        //auto sort = new Sort(observed, sort_def);
        //observed = sort->run();
        PlainTable *expected = DataUtilities::getExpectedResults(unioned_db_name, expected_sql, false, 1);

        ASSERT_EQ(*expected, *observed);

        delete observed;
        delete expected;
    }

    delete aggregate;

}
TEST_F(SecureAggregateComparisonTest, tpch_q5_SMA) {
    string unioned_db_name = "tpch_unioned";
    controlBitPacking(unioned_db_name);
    string local_db_name = (FLAGS_party == ALICE) ? "tpch_unioned" : "tpch_empty";
    int cutoff = FLAGS_cutoff;
    
    std::cout << "Expected : " << unioned_db_name << " Observed : " << local_db_name << " cutoff : " << std::to_string(cutoff) << "\n";

    string sql = "SELECT n.n_name, l.l_extendedprice * (1 - l.l_discount) as revenue, NOT (c.c_nationkey = s.s_nationkey  AND o.o_orderdate >= date '1993-01-01' AND o.o_orderdate < date '1994-01-01') AS dummy_tag\n"
                 "                 FROM  customer c JOIN orders o ON c.c_custkey = o.o_custkey\n"
                 "                     JOIN lineitem l ON l.l_orderkey = o.o_orderkey\n"
                 "                     JOIN supplier s ON l.l_suppkey = s.s_suppkey\n"
                 "                     JOIN nation n ON s.s_nationkey = n.n_nationkey\n"
                 "                     JOIN region r ON n.n_regionkey = r.r_regionkey\n"
                 "                WHERE r.r_name = 'EUROPE' AND c_custkey < " + std::to_string(cutoff) + " ORDER BY n.n_name";


    string expected_sql =  "SELECT n.n_name, SUM(l.l_extendedprice * (1 - l.l_discount)) as revenue\n"
                           " FROM  customer c JOIN orders o ON c.c_custkey = o.o_custkey\n"
                           "     JOIN lineitem l ON l.l_orderkey = o.o_orderkey\n"
                           "     JOIN supplier s ON l.l_suppkey = s.s_suppkey\n"
                           "     JOIN nation n ON s.s_nationkey = n.n_nationkey\n"
                           "     JOIN region r ON n.n_regionkey = r.r_regionkey\n"
                           "WHERE c.c_nationkey = s.s_nationkey  AND r.r_name = 'EUROPE' AND o.o_orderdate >= date '1993-01-01' AND o.o_orderdate < date '1994-01-01' AND c_custkey < " + std::to_string(cutoff) +
                           " GROUP BY   n.n_name\n"
                           " ORDER BY n.n_name";

    std::vector<int32_t> group_bys{0};
    std::vector<ScalarAggregateDefinition> aggregators{
            ScalarAggregateDefinition(1, vaultdb::AggregateId::SUM, "revenue")};

    SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(1);
    auto input = new SecureSqlInput(local_db_name, sql, true);
    auto sort = new Sort(input, sort_def);

    auto aggregate = new GroupByAggregate(sort, group_bys, aggregators);
    auto aggregated = aggregate->run();

    if(FLAGS_validation) {
        aggregated->setSortOrder(sort_def);
        PlainTable *observed = aggregated->reveal(PUBLIC);

        //auto sort = new Sort(observed, sort_def);
        //observed = sort->run();
        PlainTable *expected = DataUtilities::getExpectedResults(unioned_db_name, expected_sql, false, 1);

        ASSERT_EQ(*expected, *observed);

        delete observed;
        delete expected;
    }

    delete aggregate;

}


/*
TEST_F(SecureAggregateComparisonTest, tpch_q1_SMA) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest_SMA(1, "q1", expected_sort, unioned_db_);
}


TEST_F(SecureAggregateComparisonTest, tpch_q1_NLA) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest_NLA(1, "q1", expected_sort, unioned_db_);
}



TEST_F(SecureAggregateComparisonTest, tpch_q3_SMA) {


    // dummy_tag (-1), 1 DESC, 2 ASC
    // aka revenue desc,  o.o_orderdate
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING)};
    runTest_SMA(3, "q3", expected_sort, unioned_db_);
}


TEST_F(SecureAggregateComparisonTest, tpch_q3_NLA) {


    // dummy_tag (-1), 1 DESC, 2 ASC
    // aka revenue desc,  o.o_orderdate
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING)};
    runTest_NLA(3, "q3", expected_sort, unioned_db_);
}


// passes on codd2 in about 2.5 mins
TEST_F(SecureAggregateComparisonTest, tpch_q5_SMA) {
    //input_tuple_limit_ = 1000;

    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest_SMA(5, "q5", expected_sort, unioned_db_);
}


// passes on codd2 in about 2.5 mins
TEST_F(SecureAggregateComparisonTest, tpch_q5_NLA) {
    //input_tuple_limit_ = 1000;

    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest_NLA(5, "q5", expected_sort, unioned_db_);
}


TEST_F(SecureAggregateComparisonTest, tpch_q8_SMA) {

    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest_SMA(8, "q8", expected_sort, unioned_db_);
}


TEST_F(SecureAggregateComparisonTest, tpch_q8_NLA) {

    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest_NLA(8, "q8", expected_sort, unioned_db_);
}


TEST_F(SecureAggregateComparisonTest, tpch_q9_SMA) {
    // $0 ASC, $1 DESC
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest_SMA(9, "q9", expected_sort, unioned_db_);

}

TEST_F(SecureAggregateComparisonTest, tpch_q9_NLA) {
    // $0 ASC, $1 DESC
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest_NLA(9, "q9", expected_sort, unioned_db_);

}

TEST_F(SecureAggregateComparisonTest, tpch_q18_SMA) {
    // -1 ASC, $4 DESC, $3 ASC
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(4, SortDirection::DESCENDING),
                                 ColumnSort(3, SortDirection::ASCENDING)};

    runTest_SMA(18, "q18", expected_sort, unioned_db_);
}

TEST_F(SecureAggregateComparisonTest, tpch_q18_NLA) {
    // -1 ASC, $4 DESC, $3 ASC
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(4, SortDirection::DESCENDING),
                                 ColumnSort(3, SortDirection::ASCENDING)};

    runTest_NLA(18, "q18", expected_sort, unioned_db_);
}
*/


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}
