#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <group_by_aggregate.h>
#include <fkey_pkey_join.h>
#include "support/tpch_queries.h"
#include <operators/support/binary_predicate.h>
#include <operators/support/join_equality_predicate.h>
#include <sort.h>
#include <limit.h>
#include <project.h>


using namespace emp;
using namespace vaultdb;


class TpcHTest : public ::testing::Test {


protected:
    void SetUp() override{
        setup_plain_prot(false, "");
    };

    void TearDown() override{
        finalize_plain_prot();
    };

    std::string dbName = "tpch_unioned";
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

    std::shared_ptr<PlainTable> expected = DataUtilities::getExpectedResults(dbName, expectedOutputQuery, false, 2);

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

    SortDefinition sortDefinition = DataUtilities::getDefaultSortDefinition(2);
    SqlInput input(dbName, inputQuery, true, sortDefinition);

    GroupByAggregate aggregate(&input, groupByCols, aggregators);

    std::shared_ptr<PlainTable> aggregated = aggregate.run();

    // need to delete dummies from observed output to compare it to expected
    std::shared_ptr<PlainTable> observed = DataUtilities::removeDummies(aggregated);

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
    const std::string customerSql = "SELECT c_custkey, c_mktsegment <> 'HOUSEHOLD' cdummy "
                                    "FROM customer  "
                                    "WHERE c_custkey <= 5 "
                                    "ORDER BY c_custkey";

    const std::string ordersSql = "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' odummy "
                                  "FROM orders "
                                  "WHERE o_custkey <= 5 "
                                  "ORDER BY o_orderkey, o_orderdate, o_shippriority, o_custkey";

    const std::string lineitemSql = "SELECT  l_orderkey, l_extendedprice * (1 - l_discount) revenue, l_shipdate <= date '1995-03-25' ldummy "
                                    "FROM lineitem "
                                    "WHERE l_orderkey IN (SELECT o_orderkey FROM orders where o_custkey <= 5)  "
                                    "ORDER BY l_orderkey, revenue ";

    std::string expectedResultSql = "WITH orders_cte AS (" + ordersSql + "), \n"
                                          "lineitem_cte AS (" + lineitemSql + "), \n"
                                          "customer_cte AS (" + customerSql + "),\n "
                                          "cross_product AS (SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, c_custkey,  (o_orderkey=l_orderkey AND c_custkey = o_custkey) matched, (odummy OR ldummy OR cdummy) dummy \n"
                                          "FROM lineitem_cte, orders_cte, customer_cte  \n"
                                          "ORDER BY l_orderkey, revenue, o_orderdate, o_shippriority) \n"
                                      "SELECT l_orderkey, sum(revenue) revenue, o_orderdate, o_shippriority \n"
                                      "FROM cross_product \n"
                                      "WHERE matched \n"
                                      "GROUP BY l_orderkey, o_orderdate, o_shippriority \n"
                                      "ORDER BY revenue DESC, o_orderdate \n"
                                      "LIMIT 10";

    SqlInput customerInput(dbName, customerSql, true);
    SqlInput ordersInput(dbName, ordersSql, true);
    SqlInput lineitemInput(dbName, lineitemSql, true);


    ConjunctiveEqualityPredicate customerOrdersOrdinals;
    customerOrdersOrdinals.push_back(EqualityPredicate (1, 0)); //  o_custkey, c_custkey
    std::shared_ptr<BinaryPredicate<bool> > customerOrdersPredicate(new JoinEqualityPredicate<bool>(customerOrdersOrdinals));

    ConjunctiveEqualityPredicate lineitemOrdersOrdinals;
    lineitemOrdersOrdinals.push_back(EqualityPredicate (0, 0)); //  l_orderkey, o_orderkey
    std::shared_ptr<BinaryPredicate<bool> > lineitemOrdersPredicate(new JoinEqualityPredicate<bool> (lineitemOrdersOrdinals));


    KeyedJoin customerOrdersJoin(&ordersInput, &customerInput, customerOrdersPredicate);

    KeyedJoin fullJoin(&lineitemInput, &customerOrdersJoin, lineitemOrdersPredicate);
    std::shared_ptr<PlainTable > joined = fullJoin.run();

    // align the sort definitions for aggregate
    // TODO: make sort order checker more forgiving later
    SortDefinition aggInputSort { ColumnSort(0, SortDirection::ASCENDING), ColumnSort(4, SortDirection::ASCENDING), ColumnSort(5, SortDirection::ASCENDING)};
    joined->setSortOrder(aggInputSort);

    // Join output schema:
    // (#0 int32 lineitem.l_orderkey, #1 float .revenue, #2 int32 orders.o_orderkey, #3 int32 orders.o_custkey, #4 int64 .o_orderdate, #5 int32 orders.o_shippriority, #6 int32 customer.c_custkey)

    // aggregate group-bys:
    // l_orderkey (0), o_orderdate (4), o_shippriority (5)
    // aggregate: sum(revenue(1) )

    std::vector<int32_t> groupByCols{0, 4, 5};
    std::vector<ScalarAggregateDefinition> aggregators{ ScalarAggregateDefinition(1, vaultdb::AggregateId::SUM, "revenue")};
    GroupByAggregate aggregate(joined, groupByCols, aggregators);

    shared_ptr<PlainTable> aggregated = aggregate.run();
    std::shared_ptr<PlainTable> aggNoDummies = DataUtilities::removeDummies(aggregated);

    std::cout << "After aggregate: " << aggNoDummies->toString(true) << std::endl;

    // sort by revenue desc, o.o_orderdate ASC
    SortDefinition sortDefinition{ ColumnSort(3, SortDirection::DESCENDING), ColumnSort(1, SortDirection::ASCENDING)};
    Sort sort(aggNoDummies, sortDefinition);

    // project to    l.l_orderkey,\n"
    //                "  sum(l.l_extendedprice * (1 - l.l_discount)) as revenue,\n"
    //                "  o.o_orderdate,\n"
    //                "  o.o_shippriority\n"
    // from: l_orderkey, o_orderdate, o_shippriority, revenue
    Project project(&aggregate);

    project.addColumnMapping(0, 0);
    project.addColumnMapping(3, 1);
    project.addColumnMapping(1, 2);
    project.addColumnMapping(2, 3);


    Limit limit(&project, 10);

    shared_ptr<PlainTable> observed = limit.run();
    std::cout << "Observed: " << observed->toString(true) << std::endl;
    shared_ptr<PlainTable> expected = DataUtilities::getQueryResults(dbName, expectedResultSql, false);
    expected->setSortOrder(observed->getSortOrder());

    ASSERT_EQ(*expected, *observed);
}