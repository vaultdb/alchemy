#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <group_by_aggregate.h>
#include <keyed_join.h>
#include "support/tpch_queries.h"
#include <operators/support/binary_predicate.h>
#include <operators/support/join_equality_predicate.h>
#include <sort.h>
#include <limit.h>
#include <project.h>
#include <algorithm>
#include <boost/algorithm/string/replace.hpp>

using namespace emp;
using namespace vaultdb;

// TODO: create a utility that invokes the SQL-to-MPC translator to create a JSON plan

class TpcHTest : public ::testing::Test {


protected:
    void SetUp() override{
        setup_plain_prot(false, "");
        string q3_limit = std::to_string(q3_cutoff);


        boost::replace_all(q3_customer_sql, "$CUTOFF", q3_limit.c_str());
        boost::replace_all(q3_orders_sql, "$CUTOFF", q3_limit.c_str());
        boost::replace_all(q3_lineitem_sql, "$CUTOFF", q3_limit.c_str());

    };

    void TearDown() override{
        finalize_plain_prot();
    };

    string db_name = "tpch_unioned";


    void validate_q3_join(const shared_ptr<PlainTable> & observed);

    size_t q3_cutoff = 100;
    string q3_customer_sql = "SELECT c_custkey, c_mktsegment <> 'HOUSEHOLD' cdummy "
                          "FROM customer  "
                          "WHERE c_custkey <=  $CUTOFF"
                          " ORDER BY c_custkey";

    string q3_orders_sql = "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' odummy "
                        "FROM orders "
                        "WHERE o_custkey <= $CUTOFF "
                        "ORDER BY o_orderkey, o_orderdate, o_shippriority"; // was o_orderkey, o_custkey, o_orderdate, o_shippriority


    string q3_lineitem_sql = "SELECT  l_orderkey, l_extendedprice * (1 - l_discount) revenue, l_shipdate <= date '1995-03-25' ldummy "
                          "FROM lineitem "
                          "WHERE l_orderkey IN (SELECT o_orderkey FROM orders where o_custkey <= $CUTOFF)  "
                          "ORDER BY l_orderkey, revenue ";


};


TEST_F(TpcHTest, testQ1Truncated) {
    string inputTuples = "SELECT * FROM lineitem WHERE l_orderkey <= 500 ORDER BY l_orderkey, l_linenumber";
    string inputQuery = "SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice,  l_discount, l_extendedprice * (1 - l_discount) AS disc_price, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, \n"
                        " l_shipdate > date '1998-08-03' AS dummy\n"  // produces true when it is a dummy, reverses the logic of the sort predicate
                        " FROM (" + inputTuples + ") selection \n"
                                                  " ORDER BY l_returnflag, l_linestatus";

    string expectedOutputQuery =  "select \n"
                                  "  l_returnflag, \n"
                                  "  l_linestatus, \n"
                                  "  sum(l_quantity) as sum_qty, \n"
                                  "  sum(l_extendedprice) as sum_base_price, \n"
                                  "  sum(l_extendedprice * (1 - l_discount)) as sum_disc_price, \n"
                                  "  sum(l_extendedprice * (1 - l_discount) * (1 + l_tax)) as sum_charge, \n"
                                  "  avg(l_quantity) as avg_qty, \n"
                                  "  avg(l_extendedprice) as avg_price, \n"
                                  "  avg(l_discount) as avg_disc, \n"
                                  "  count(*) as count_order \n"
                                  "from (" + inputTuples + ") input "
                                                           " where  l_shipdate <= date '1998-08-03'  "
                                                           "group by \n"
                                                           "  l_returnflag, \n"
                                                           "  l_linestatus \n"
                                                           " \n"
                                                           "order by \n"
                                                           "  l_returnflag, \n"
                                                           "  l_linestatus";

    shared_ptr<PlainTable> expected = DataUtilities::getExpectedResults(db_name, expectedOutputQuery, false, 2);

    vector<int32_t> groupByCols{0, 1};
    vector<ScalarAggregateDefinition> aggregators{
            ScalarAggregateDefinition(2, vaultdb::AggregateId::SUM, "sum_qty"),
            ScalarAggregateDefinition(3, vaultdb::AggregateId::SUM, "sum_base_price"),
            ScalarAggregateDefinition(5, vaultdb::AggregateId::SUM, "sum_disc_price"),
            ScalarAggregateDefinition(6, vaultdb::AggregateId::SUM, "sum_charge"),
            ScalarAggregateDefinition(2, vaultdb::AggregateId::AVG, "avg_qty"),
            ScalarAggregateDefinition(3, vaultdb::AggregateId::AVG, "avg_price"),
            ScalarAggregateDefinition(4, vaultdb::AggregateId::AVG, "avg_disc"),
            ScalarAggregateDefinition(-1, vaultdb::AggregateId::COUNT, "count_order")};

    SortDefinition sortDefinition = DataUtilities::getDefaultSortDefinition(2);
    SqlInput input(db_name, inputQuery, true, sortDefinition);

    GroupByAggregate aggregate(&input, groupByCols, aggregators);

    shared_ptr<PlainTable> aggregated = aggregate.run();

    // need to delete dummies from observed output to compare it to expected
    shared_ptr<PlainTable> observed = DataUtilities::removeDummies(aggregated);

    ASSERT_EQ(*expected, *observed);
}





/*
 * "select\n"
                "  l.l_orderkey,\n"
                "  sum(l.l_extendedprice * (1 - l.l_discount)) as revenue,\n"
                "  o.o_orderdate,\n"
                "  o.o_shippriority\n"
                " from\n"
                "  customer c JOIN orders o ON  c.c_custkey = o.o_custkey\n"
                "     JOIN lineitem l ON l.l_orderkey = o.o_orderkey\n"
                " where\n"
                "  c.c_mktsegment = 'HOUSEHOLD'\n"
                "  and o.o_orderdate < date '1995-03-25'\n"
                "  and l.l_shipdate > date '1995-03-25'\n"
                " and c_custkey <= $CUTOFF" -- added for test
                "\n"
                " group by\n"
                "  l.l_orderkey,\n"
                "  o.o_orderdate,\n"
                "  o.o_shippriority\n"
                " order by\n"
                "  revenue desc,\n"
                "  o.o_orderdate\n"
                " limit 10"
 */
TEST_F(TpcHTest, testQ3Truncated)  {

    string expected_join_sql = "WITH orders_cte AS (" + q3_orders_sql + "), \n"
                                  "lineitem_cte AS (" + q3_lineitem_sql + "), \n"
                                  "customer_cte AS (" + q3_customer_sql + "),\n "
                                  "cross_product AS (SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey,  (o_orderkey=l_orderkey AND c_custkey = o_custkey) matched, (odummy OR ldummy OR cdummy) dummy \n"
                                                     "FROM lineitem_cte, orders_cte, customer_cte  \n"
                                                     "ORDER BY l_orderkey, revenue, o_orderdate, o_shippriority) \n"
                                  "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey, dummy \n"
                                  "FROM cross_product \n"
                                  "WHERE matched";


    string expected_results_sql = tpch_queries[3];
    boost::replace_first(expected_results_sql, "where", "where c_custkey <= " + std::to_string(q3_cutoff) + " AND ");


    SqlInput customer_input(db_name, q3_customer_sql, true);
    SqlInput orders_input(db_name, q3_orders_sql, true);
    SqlInput lineitem_input(db_name, q3_lineitem_sql, true);



    BoolExpression<bool> customer_orders_predicate = Utilities::getEqualityPredicate<bool>(1, 4);
    BoolExpression<bool> lineitem_orders_predicate = Utilities::getEqualityPredicate<bool>(0, 2);


    // preserves sort order of orders
    KeyedJoin customer_orders_join(&orders_input, &customer_input, customer_orders_predicate);

    // follows ORDER  BY of lineitem
    KeyedJoin full_join(&lineitem_input, &customer_orders_join, lineitem_orders_predicate);
    shared_ptr<PlainTable> joined = full_join.run();
    validate_q3_join(joined);

    // to avoid extra sort, need lineitem sorted on l_orderkey, order sorted on o_orderkey, o_orderdate, o_shippriority
    // functional dependencies and transitivity address the rest
    SortDefinition joined_sort_order{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(4, SortDirection::ASCENDING),ColumnSort(5, SortDirection::ASCENDING)};
    joined->setSortOrder(joined_sort_order);


    vector<int32_t> group_by_cols{0, 4, 5};
    vector<ScalarAggregateDefinition> aggregators{ ScalarAggregateDefinition(1, vaultdb::AggregateId::SUM, "revenue")};

    GroupByAggregate aggregate(joined, group_by_cols, aggregators);
    shared_ptr<PlainTable> aggregated = aggregate.run();


    // project to    l.l_orderkey,\n"
    //                "  sum(l.l_extendedprice * (1 - l.l_discount)) as revenue,\n"
    //                "  o.o_orderdate,\n"
    //                "  o.o_shippriority\n"
    // from: l_orderkey, o_orderdate, o_shippriority, revenue

    Project project(&aggregate);
    project.addColumnMapping(0, 0); // l_orderkey
    project.addColumnMapping(3, 1); // revenue
    project.addColumnMapping(1, 2); // o_orderdate
    project.addColumnMapping(2, 3); // o_shippriority



    // sort by dummy_tag, revenue desc, o.o_orderdate ASC
    SortDefinition sortDefinition{ ColumnSort(-1, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING), ColumnSort(2, SortDirection::ASCENDING)};
    Sort sort(&project, sortDefinition);
    Limit limit(&sort, 10);




    shared_ptr<PlainTable> observed = limit.run();
    observed = DataUtilities::removeDummies(observed);
    cout << "Observed: " << observed->toString(true) << endl;
    shared_ptr<PlainTable> expected = DataUtilities::getQueryResults(db_name, expected_results_sql, false);
    expected->setSortOrder(observed->getSortOrder());

    ASSERT_EQ(*expected, *observed);
}


void TpcHTest::validate_q3_join(const shared_ptr<PlainTable> &joined) {

    string expected_join_sql =    "WITH orders_cte AS (" + q3_orders_sql + "), \n"
                                        "lineitem_cte AS (" + q3_lineitem_sql + "), \n"
                                        "customer_cte AS (" + q3_customer_sql + "),\n "
                                        "cross_product AS (SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey,  (o_orderkey=l_orderkey AND c_custkey = o_custkey) matched, (odummy OR ldummy OR cdummy) dummy \n"
                                                           "FROM lineitem_cte, orders_cte, customer_cte  \n"
                                                           "ORDER BY l_orderkey, revenue, o_orderdate, o_shippriority) \n"
                                        "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey, dummy \n"
                                        "FROM cross_product \n"
                                        "WHERE matched";

    shared_ptr<PlainTable > expected_join_result = DataUtilities::getQueryResults(db_name, expected_join_sql, true);
    ASSERT_EQ(*joined, *expected_join_result);



}


//                "select\n"
//                "  n.n_name,\n"
//                "  sum(l.l_extendedprice * (1 - l.l_discount)) as revenue\n"
//                " from\n"
//                "  customer c JOIN orders o ON c.c_custkey = o.o_custkey\n"
//                "     JOIN lineitem l ON l.l_orderkey = o.o_orderkey\n"
//                "     JOIN supplier s ON l.l_suppkey = s.s_suppkey\n"
//                "     JOIN nation n ON s.s_nationkey = n.n_nationkey\n"
//                "     JOIN region r ON n.n_regionkey = r.r_regionkey\n"
//                "where\n"
//                "   c.c_nationkey = s.s_nationkey\n"
//                "  and r.r_name = 'EUROPE'\n"
//                "  and o.o_orderdate >= date '1997-01-01'\n"
//                "  and o.o_orderdate < date '1998-01-01'\n"
//                " group by\n"
//                "  n.n_name\n"
//                " order by\n"
//                "  revenue desc",
// basically customer -- orders -- lineitem -- supplier -- nation -- region
//  supplier -- nation -- region in plaintext
//
// Consider using something like MapD for getting from SQL to C++
// they used a JSON spec for their query trees to invoke them in C++
// https://www.omnisci.com/blog/fast-and-flexible-query-analysis-at-mapd-with-apache-calcite-2
TEST_F(TpcHTest, testQ5Truncated)  {



}