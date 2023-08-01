#include "plain_base_test.h"
#include "util/field_utilities.h"
#include <operators/sql_input.h>
#include <operators/sort_merge_join.h>
#include <operators/sort.h>

#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <util/type_utilities.h>
#include <util/data_utilities.h>

#include <test/support/tpch_queries.h>
#include <boost/algorithm/string/replace.hpp>
#include <parser/plan_parser.h>



DEFINE_int32(cutoff, 10, "limit clause for queries");
DEFINE_string(storage, "row", "storage model for tables (row or column)");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(filter, "*", "run only the tests passing this filter");

class SortMergeJoinTest :  public PlainBaseTest  {

protected:
 

    const std::string customer_sql_ = "SELECT c_custkey, c_mktsegment <> 'HOUSEHOLD' c_dummy \n"
                                      "FROM customer  \n"
                                      "WHERE c_custkey <= " + std::to_string(FLAGS_cutoff) +
                                      " ORDER BY c_custkey";


    const std::string orders_sql_ = "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' o_dummy \n"
                                    "FROM orders \n"
                                    "WHERE o_custkey <=  " + std::to_string(FLAGS_cutoff) +
                                    " ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, o_dummy";

    const std::string lineitem_sql_ = "SELECT  l_orderkey, l_extendedprice * (1 - l_discount) revenue, l_shipdate <= date '1995-03-25' l_dummy \n"
                                      "FROM lineitem \n"
                                      "WHERE l_orderkey IN (SELECT o_orderkey FROM orders where o_custkey <= " + std::to_string(FLAGS_cutoff) + ")  \n"
                                     " ORDER BY l_orderkey, revenue, l_dummy ";


	void runTest(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name) {

	 
		//this->initializeBitPacking(FLAGS_unioned_db);

		string expected_query = generateExpectedOutputQuery(test_id, expected_sort, FLAGS_unioned_db);
		//string party_name = FLAGS_party == emp::ALICE ? "alice" : "bob";
		string local_db = FLAGS_unioned_db;
		//boost::replace_first(local_db, "unioned", party_name.c_str());

		cout << " Observed DB : "<< local_db << " - Bit Packed" << endl;
		cout << "expected query: " << expected_query << "\n";
		PlainTable *expected = DataUtilities::getExpectedResults(FLAGS_unioned_db, expected_query, false, 0);
		expected->setSortOrder(expected_sort);

		std::string sql_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_5/Fully_Optimized/fully_optimized-" + test_name + ".sql";
		std::string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/experiment_5/Fully_Optimized/fully_optimized-"  + test_name + ".json";

		time_point<high_resolution_clock> startTime = clock_start();
		clock_t secureStartClock = clock();

		PlanParser<bool> parser(local_db, sql_file, plan_file, FLAGS_cutoff);
		PlainOperator *root = parser.getRoot();

		//std::cout << root->printTree() << endl;

		PlainTable *result = root->run();

		double secureClockTicks = (double) (clock() - secureStartClock);
		double secureClockTicksPerSecond = secureClockTicks / ((double) CLOCKS_PER_SEC);
		double duration = time_from(startTime) / 1e6;

		cout << "Time: " << duration << " sec, CPU clock ticks: " << secureClockTicks << ",CPU clock ticks per second: " << secureClockTicksPerSecond << "\n";


        PlainTable *observed = result->reveal();
        DataUtilities::removeDummies(observed);

		Sort sorter(observed, DataUtilities::getDefaultSortDefinition(5));
		observed = sorter.run()->clone();
		expected->setSortOrder(observed->getSortOrder());

        ASSERT_EQ(*expected, *observed);
        ASSERT_TRUE(!observed->empty()); // want all tests to produce output	

        delete observed;
        delete expected;



	}

	string generateExpectedOutputQuery(const int &test_id, const SortDefinition &expected_sort, const string &db_name) {
		string alice_db = FLAGS_unioned_db;
		string bob_db = "tpch_empty";
		//boost::replace_first(alice_db, "unioned", "alice");
		//boost::replace_first(bob_db, "unioned", "bob");

		string query = 	"WITH lhs AS (SELECT t1.n_name, t2.c_custkey, t2.c_nationkey\n"
						"FROM (SELECT r_regionkey, r_name\n"
						"	FROM region\n"
						"	WHERE r_name = 'EUROPE') AS t0\n"
						"	INNER JOIN (SELECT n_nationkey, n_name, n_regionkey\n"
						"	FROM nation) AS t1 ON t0.r_regionkey = t1.n_regionkey\n"
						"	INNER JOIN (SELECT c_custkey, c_nationkey\n"
						"FROM customer LIMIT $LIMIT) AS t2 ON t1.n_nationkey = t2.c_nationkey\n"
						"ORDER BY t1.n_name, t2.c_custkey, t2.c_nationkey\n"
						"),\n"
						"rhs AS (SELECT o_orderkey, o_custkey, NOT (o_orderdate >= DATE '1993-01-01' AND o_orderdate < DATE '1994-01-01') AS dummy_tag\n"
						"FROM orders\n"
						"ORDER BY o_orderkey, o_custkey\n"
						"LIMIT $LIMIT\n"
						")\n"
						"SELECT n_name, c_custkey, c_nationkey,o_orderkey,o_custkey\n"
						"FROM lhs JOIN rhs ON c_custkey = o_custkey\n"
						"WHERE NOT dummy_tag\n"
						"ORDER BY n_name, c_custkey, c_nationkey, o_orderkey, o_custkey ASC;";

		if(FLAGS_cutoff > 0) {
			//query = truncated_tpch_queries[test_id];
			boost::replace_all(query, "$LIMIT", std::to_string(FLAGS_cutoff));
			boost::replace_all(query, "$ALICE_DB", alice_db);
			boost::replace_all(query, "$BOB_DB", bob_db);
		}

		return query;
	}

};




TEST_F(SortMergeJoinTest, test_tpch_q3_customer_orders) {

    std::string expected_sql = "WITH customer_cte AS (" + customer_sql_ + "), "
                                          "orders_cte AS (" + orders_sql_ + ") "
                                   "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey "
                                   "FROM  orders_cte JOIN customer_cte ON c_custkey = o_custkey "
                                   "WHERE NOT o_dummy AND NOT c_dummy "
                                    "ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey";


    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, false);

    auto *customer_input = new SqlInput(db_name_, customer_sql_, true);
    auto *orders_input = new SqlInput(db_name_, orders_sql_, true);

    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    Expression<bool> *predicate = FieldUtilities::getEqualityPredicate<bool>(orders_input, 1, customer_input, 4);

    auto join = new SortMergeJoin(orders_input, customer_input,  predicate);
    Sort sorter(join, DataUtilities::getDefaultSortDefinition(2));
    PlainTable *observed = sorter.run();
    expected->setSortOrder(observed->getSortOrder());
    DataUtilities::removeDummies(observed);

    ASSERT_EQ(*expected, *observed);

    delete expected;


}


TEST_F(SortMergeJoinTest, test_tpch_q3_lineitem_orders) {

    std::string expected_sql = "WITH orders_cte AS (" + orders_sql_ + "), "
                                                                         "lineitem_cte AS (" + lineitem_sql_ + "), "
                                                                                                             "cross_product AS (SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, (o_orderkey=l_orderkey) matched, (o_dummy OR l_dummy) dummy \n"
                                                                                                             "FROM lineitem_cte, orders_cte \n"
                                                                                                             "ORDER BY l_orderkey, revenue, o_orderdate, o_shippriority) \n"
                                                                                                             "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, dummy \n"
                                                                                                             "FROM cross_product \n"
                                                                                                             "WHERE matched";


    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, true);

    SortDefinition lineitem_sort = DataUtilities::getDefaultSortDefinition(2);
    SortDefinition orders_sort = DataUtilities::getDefaultSortDefinition(4);

    auto *lineitem_input = new SqlInput(db_name_, lineitem_sql_, true, lineitem_sort);
    auto *orders_input = new SqlInput(db_name_, orders_sql_, true, orders_sort);

    // output schema: lineitem, orders
    // l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority
    Expression<bool> *predicate  = FieldUtilities::getEqualityPredicate<bool>(lineitem_input, 0, orders_input,
                                                                              2);

    auto join = new SortMergeJoin(lineitem_input, orders_input, 0, predicate);
    Sort<bool> sorter(join, DataUtilities::getDefaultSortDefinition(2));
    PlainTable *observed = sorter.run();

    DataUtilities::removeDummies(observed);
    DataUtilities::removeDummies(expected);
    expected->setSortOrder(observed->getSortOrder());

    ASSERT_EQ(*expected, *observed);

    delete expected;

}



TEST_F(SortMergeJoinTest, test_tpch_q3_lineitem_orders_customer) {


    std::string expected_sql = "WITH orders_cte AS (" + orders_sql_ + "), \n"
									"lineitem_cte AS (" + lineitem_sql_ + "), \n"
                                    "customer_cte AS (" + customer_sql_ + "),\n "
                                    "cross_product AS (SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey,  (o_orderkey=l_orderkey AND c_custkey = o_custkey) matched, (o_dummy OR l_dummy OR c_dummy) dummy \n"
                                    "FROM lineitem_cte, orders_cte, customer_cte  \n"
                                    "ORDER BY l_orderkey, revenue, o_orderdate, o_shippriority) \n"
									"SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey, dummy \n"
                                    "FROM cross_product \n"
                                    "WHERE matched \n"
									"ORDER BY l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey";


    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, true);

    SortDefinition lineitem_sort = DataUtilities::getDefaultSortDefinition(2);
    SortDefinition orders_sort = DataUtilities::getDefaultSortDefinition(4);

    auto *customer_input = new SqlInput(db_name_, customer_sql_, true);
    auto *orders_input = new SqlInput(db_name_, orders_sql_, true, orders_sort);
    auto *lineitem_input = new SqlInput(db_name_, lineitem_sql_, true, lineitem_sort);

    // join output schema: (orders, customer)
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey

    Expression<bool> *customer_orders_predicate = FieldUtilities::getEqualityPredicate<bool>(orders_input, 1,
                                                                                             customer_input, 4);
    auto *customer_orders_join = new SortMergeJoin (orders_input, customer_input, 0, customer_orders_predicate);

    // join output schema:
    //  l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey
    Expression<bool> *lineitem_orders_predicate = FieldUtilities::getEqualityPredicate<bool>(lineitem_input, 0,
                                                                                             customer_orders_join, 2);



    auto *full_join = new SortMergeJoin(lineitem_input, customer_orders_join, 0, lineitem_orders_predicate);


	SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(7);
    Sort<bool> observed_sort(full_join, sort_def);
    auto observed = observed_sort.run();

    DataUtilities::removeDummies(observed);

    expected->setSortOrder(observed->getSortOrder());

	DataUtilities::removeDummies(expected);



    ASSERT_EQ(*expected, *observed);

    delete expected;

}



TEST_F(SortMergeJoinTest, test_tpch_q3_customer_orders_reversed) {

    std::string expected_sql = "WITH customer_cte AS (" + customer_sql_ + "), "
                                                                             "orders_cte AS (" + orders_sql_ + ") "
                                                                                                             "SELECT c_custkey, o_orderkey, o_custkey, o_orderdate, o_shippriority,(c_dummy OR o_dummy) dummy "
                                                                                                             "FROM  orders_cte JOIN customer_cte ON c_custkey = o_custkey "
                                                                                                             "WHERE NOT c_dummy AND NOT o_dummy "
                                                                                                             "ORDER BY c_custkey, o_orderkey, o_custkey, o_orderdate, o_shippriority ";


    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, true);

    auto *customer_input = new SqlInput(db_name_, customer_sql_, true);
    auto *orders_input = new SqlInput(db_name_, orders_sql_, true);

    // join output schema: (orders, customer)
    // c_custkey, o_orderkey, o_custkey, o_orderdate, o_shippriority
    Expression<bool> *predicate = FieldUtilities::getEqualityPredicate<bool>(customer_input, 0, orders_input,
                                                                             2);

    auto join = new SortMergeJoin(customer_input, orders_input, 1, predicate);
    Sort sorter(join, DataUtilities::getDefaultSortDefinition(2));
    PlainTable *observed = sorter.run();

    DataUtilities::removeDummies(observed);
    expected->setSortOrder(observed->getSortOrder());

    ASSERT_EQ(*expected, *observed);

    delete expected;


}


TEST_F(SortMergeJoinTest, test_tpch_q3_lineitem_orders_reversed) {

    std::string expected_sql = "WITH orders_cte AS (" + orders_sql_ + "), "
                                                     "lineitem_cte AS (" + lineitem_sql_ + ") "
                                                     " SELECT  o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue "
                                                     " FROM orders_cte JOIN lineitem_cte ON l_orderkey = o_orderkey "
                                                     " WHERE NOT o_dummy AND NOT l_dummy "
                                                     " ORDER BY o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue";


    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, false);

    SortDefinition lineitem_sort = DataUtilities::getDefaultSortDefinition(2);
    SortDefinition orders_sort = DataUtilities::getDefaultSortDefinition(4);

    auto *lineitem_input = new SqlInput(db_name_, lineitem_sql_, true, lineitem_sort);
    auto *orders_input = new SqlInput(db_name_, orders_sql_, true, orders_sort);


    // output schema:  orders, lineitem
    // o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue
    Expression<bool> *predicate  = FieldUtilities::getEqualityPredicate<bool>(orders_input, 0, lineitem_input,
                                                                              4);
    auto join = new SortMergeJoin(orders_input, lineitem_input, 1, predicate);
    Sort<bool> sorter(join, DataUtilities::getDefaultSortDefinition(6));
    PlainTable *observed = sorter.run();

    DataUtilities::removeDummies(observed);
    expected->setSortOrder(observed->getSortOrder());
	DataUtilities::removeDummies(expected);


    ASSERT_EQ(*expected, *observed);


    delete expected;

}



TEST_F(SortMergeJoinTest, test_tpch_q3_lineitem_orders_customer_reversed) {


    std::string expected_sql = "WITH orders_cte AS (" + orders_sql_ + "), \n"
									"lineitem_cte AS (" + lineitem_sql_ + "), \n"
                                    "customer_cte AS (" + customer_sql_ + "),\n "
                                    "cross_product AS (SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey,  (o_orderkey=l_orderkey AND c_custkey = o_custkey) matched, (o_dummy OR l_dummy OR c_dummy) dummy \n"
                                    "FROM lineitem_cte, orders_cte, customer_cte  \n"
                                    "ORDER BY l_orderkey, revenue, o_orderdate, o_shippriority) \n"
                                    "SELECT c_custkey, o_orderkey, o_custkey, o_orderdate, o_shippriority,  l_orderkey, revenue, dummy \n"
                                    "FROM cross_product \n"
                                    "WHERE matched \n"
									"ORDER BY c_custkey, o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue";


    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, true);

    SortDefinition lineitem_sort = DataUtilities::getDefaultSortDefinition(2);
    SortDefinition orders_sort = DataUtilities::getDefaultSortDefinition(4);

    auto *customer_input = new SqlInput(db_name_, customer_sql_, true);
    auto *orders_input = new SqlInput(db_name_, orders_sql_, true, orders_sort);
    auto *lineitem_input = new SqlInput(db_name_, lineitem_sql_, true, lineitem_sort);

    // join output schema: (orders, customer)
    // c_custkey, o_orderkey, o_custkey, o_orderdate, o_shippriority

    Expression<bool> *customer_orders_predicate = FieldUtilities::getEqualityPredicate<bool>(customer_input, 0,
                                                                                             orders_input, 2);

    auto customer_orders_join = new SortMergeJoin(customer_input, orders_input, 1, customer_orders_predicate);

    // join output schema:
    //   c_custkey, o_orderkey, o_custkey, o_orderdate, o_shippriority, l_orderkey, revenue
    Expression<bool> *lineitem_orders_predicate = FieldUtilities::getEqualityPredicate<bool>(customer_orders_join, 1,
                                                                                             lineitem_input, 5);



    auto full_join = new SortMergeJoin(customer_orders_join, lineitem_input, 1, lineitem_orders_predicate);


    PlainTable *observed = full_join->run()->clone();
	SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(7);
    Sort<bool> observed_sort(observed, sort_def);
    observed = observed_sort.run();

	DataUtilities::removeDummies(observed);

    expected->setSortOrder(observed->getSortOrder());

	DataUtilities::removeDummies(expected);

    ASSERT_EQ(*expected, *observed);
    delete expected;
    delete full_join;




}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

	::testing::GTEST_FLAG(filter)=FLAGS_filter;
    return RUN_ALL_TESTS();
}

