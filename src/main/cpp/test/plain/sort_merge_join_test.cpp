#include "plain_base_test.h"
#include <operators/sort.h>

#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <util/data_utilities.h>

#include <test/support/tpch_queries.h>



DEFINE_int32(cutoff, 100, "limit clause for queries");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(filter, "*", "run only the tests passing this filter");

class SortMergeJoinTest :  public PlainBaseTest  {

protected:
 

    const std::string customer_sql_ = "SELECT c_custkey, c_mktsegment <> 'HOUSEHOLD' c_dummy \n"
                                      "FROM customer  \n"
                                      "WHERE c_custkey <= " + std::to_string(FLAGS_cutoff) +
                                      " ORDER BY c_custkey";


    const std::string orders_cust_sql_ = "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' o_dummy \n"
                                    "FROM orders \n"
                                    "WHERE o_custkey <=  " + std::to_string(FLAGS_cutoff) +
                                    " ORDER BY o_custkey, o_orderkey";

    const std::string orders_lineitem_sql_ = "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' o_dummy \n"
                                         "FROM orders \n"
                                         "WHERE o_custkey <=  " + std::to_string(FLAGS_cutoff) +
                                         " ORDER BY o_orderkey";


    const std::string lineitem_sql_ = "SELECT  l_orderkey, l_extendedprice * (1 - l_discount) revenue, l_shipdate <= date '1995-03-25' l_dummy \n"
                                      "FROM lineitem \n"
                                      "WHERE l_orderkey IN (SELECT o_orderkey FROM orders where o_custkey <= " + std::to_string(FLAGS_cutoff) + ")  \n"
                                     " ORDER BY l_orderkey, revenue, l_dummy ";

    SortDefinition customer_sort_ = DataUtilities::getDefaultSortDefinition(1);
    SortDefinition orders_cust_sort_{ColumnSort (1, SortDirection::ASCENDING), ColumnSort (0, SortDirection::ASCENDING)};
    SortDefinition orders_lineitem_sort_{ColumnSort (0, SortDirection::ASCENDING)};
    SortDefinition lineitem_sort_ = DataUtilities::getDefaultSortDefinition(2);

	void runTest(const int &test_id, const string & test_name, const SortDefinition &expected_sort, const string &db_name) {
	}
};

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

	::testing::GTEST_FLAG(filter)=FLAGS_filter;
    int i = RUN_ALL_TESTS();
    google::ShutDownCommandLineFlags();
    return i;

}
