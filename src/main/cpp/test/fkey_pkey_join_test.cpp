//
// Created by Jennie Rogers on 9/21/20.
//
//
// Created by Jennie Rogers on 9/13/20.
//

#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/support/binary_predicate.h>
#include <operators/support/join_equality_predicate.h>
#include <operators/fkey_pkey_join.h>
#include <operators/project.h>
#include <operators/common_table_expression_input.h>
#include <operators/sort.h>


using namespace emp;
using namespace vaultdb::types;



class ForeignKeyPrimaryKeyJoinTest : public ::testing::Test {


protected:
    void SetUp() override {};
    void TearDown() override{};
    const std::string dbName = "tpch_unioned";
};





TEST_F(ForeignKeyPrimaryKeyJoinTest, test_tpch_q3_customer_orders) {

    // get inputs from local oblivious ops
    // first 5 customers, propagate this constraint up the join tree for the test
    std::string customerSql = "SELECT c_custkey, c_mktsegment <> 'HOUSEHOLD' cdummy "
                              "FROM customer  "
                              "WHERE c_custkey <= 5 "
                              "ORDER BY c_custkey";

    // orders is outer relation
    std::string ordersSql = "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' odummy "
                            "FROM orders "
                            "WHERE o_custkey <= 5 "
                            "ORDER BY o_orderkey";

    std::string expectedResultSql = "WITH customer_cte AS (" + customerSql + "), "
                                                                             "orders_cte AS (" + ordersSql + ") "
                                                                                                             "SELECT o_orderkey, o_custkey,(cdummy OR odummy) dummy "
                                                                                                             "FROM  orders_cte JOIN customer_cte ON c_custkey = o_custkey "
                                                                                                             "ORDER BY o_orderkey";

    std::cout << "Expected results SQL: " << expectedResultSql <<  std::endl;

    std::shared_ptr<QueryTable> expected = DataUtilities::getQueryResults(dbName, expectedResultSql, true);

    std::shared_ptr<Operator> customerInput(new SqlInput(dbName, customerSql, true));
    std::shared_ptr<Operator> ordersInput(new SqlInput(dbName, ordersSql, true));


    ConjunctiveEqualityPredicate customerOrdersOrdinals;
    customerOrdersOrdinals.push_back(EqualityPredicate (1, 0)); //  o_custkey, c_custkey

    std::shared_ptr<BinaryPredicate> customerOrdersPredicate(new JoinEqualityPredicate(customerOrdersOrdinals, false));

    ForeignKeyPrimaryKeyJoin *joinOp = new ForeignKeyPrimaryKeyJoin(customerOrdersPredicate, ordersInput, customerInput);


    Project *projectOp = new Project(joinOp->getPtr());
    projectOp->addColumnMapping(0, 0); // o_orderkey
    projectOp->addColumnMapping(1, 1); // o_custkey
    std::shared_ptr<Operator> project = projectOp->getPtr();

    std::shared_ptr<QueryTable> observed = project->run();

    std::cout << "customer input: " << std::endl << customerInput->getOutput()->toString(true);
    std::cout << "orders input: " << std::endl << ordersInput->getOutput()->toString(true);

    std::cout << "join output: " << std::endl << joinOp->getOutput()->toString(false) << std::endl;

    std::cout << "project output: " << std::endl << project->getOutput()->toString(false) << std::endl;


    ASSERT_EQ(*expected, *observed);

}


TEST_F(ForeignKeyPrimaryKeyJoinTest, test_tpch_q3_lineitem_orders) {

    // get inputs from local oblivious ops
    // first 3 customers, propagate this constraint up the join tree for the test


    std::string ordersSql = "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' odummy "
                            "FROM orders "
                            "WHERE o_custkey <= 5 "
                            "ORDER BY o_orderkey, o_orderdate "
                            "LIMIT 10";

    std::string lineitemSql = "SELECT  l_orderkey, l_extendedprice * (1 - l_discount) revenue, l_shipdate <= date '1995-03-25' ldummy "
                              "FROM lineitem "
                              "WHERE l_orderkey IN (SELECT o_orderkey FROM orders where o_custkey <= 5)  "
                              "ORDER BY l_orderkey, l_linenumber "
                              "LIMIT 10";
    std::string expectedResultSql = "WITH orders_cte AS (" + ordersSql + "), "
                                                                         "lineitem_cte AS (" + lineitemSql + "), "
                                                                                                             "cross_product AS (SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, (o_orderkey=l_orderkey) matched, (odummy OR ldummy) dummy \n"
                                                                                                             "FROM lineitem_cte, orders_cte \n"
                                                                                                             "ORDER BY l_orderkey, revenue, o_orderdate, o_shippriority) \n"
                                                                                                             "SELECT l_orderkey, revenue, o_orderkey, o_custkey, o_orderdate, o_shippriority, dummy \n"
                                                                                                             "FROM cross_product \n"
                                                                                                             "WHERE matched";

    std::cout << "Expected results SQL: " << expectedResultSql <<  std::endl;

    std::shared_ptr<QueryTable> expected = DataUtilities::getQueryResults(dbName, expectedResultSql, true);

    std::shared_ptr<Operator> lineitemInput(new SqlInput(dbName, lineitemSql, true));
    std::shared_ptr<Operator> ordersInput(new SqlInput(dbName, ordersSql, true));


    ConjunctiveEqualityPredicate lineitemOrdersOrdinals;
    lineitemOrdersOrdinals.push_back(EqualityPredicate (0, 0)); //  l_orderkey, o_orderkey

    std::shared_ptr<BinaryPredicate> lineitemOrdersPredicate(new JoinEqualityPredicate(lineitemOrdersOrdinals, false));

    ForeignKeyPrimaryKeyJoin *joinOp = new ForeignKeyPrimaryKeyJoin(lineitemOrdersPredicate, lineitemInput, ordersInput);


    /*Project *projectOp = new Project(joinOp->getPtr());
    projectOp->addColumnMapping(0, 0); // l_orderkey
    projectOp->addColumnMapping(1, 1); // revenue
    projectOp->addColumnMapping(4, 2); // o_orderdate
    projectOp->addColumnMapping(5, 3); // o_shippriority
    std::shared_ptr<Operator> project = projectOp->getPtr();

    std::shared_ptr<QueryTable> observedUnordered = project->run();*/

    std::cout << "lineitem input: " << std::endl << lineitemInput->getOutput()->toString(true);
    std::cout << "orders input: " << std::endl << ordersInput->getOutput()->toString(true);

    std::cout << "join output: " << std::endl << joinOp->getOutput()->toString(false) << std::endl;

    //std::cout << "project output: " << std::endl << observedUnordered->toString(false) << std::endl;

    // sort them both on l_orderkey, revenue, orderdate, shippriority
    SortDefinition sortDefinition;
    sortDefinition.push_back(std::pair<uint32_t , SortDirection>(0, SortDirection::ASCENDING));
    sortDefinition.push_back(std::pair<uint32_t , SortDirection>(1, SortDirection::ASCENDING));
    sortDefinition.push_back(std::pair<uint32_t , SortDirection>(4, SortDirection::ASCENDING));
    sortDefinition.push_back(std::pair<uint32_t , SortDirection>(5, SortDirection::ASCENDING));


    std::shared_ptr<Operator> observedInput(joinOp->getPtr());
    Sort *sortedObservedOutput = new Sort(sortDefinition, observedInput);
    std::shared_ptr<QueryTable> observed = sortedObservedOutput->run();

    std::cout << "Observed: " << observed->toString(true) << std::endl;
    std::cout << "Expected: " << expected->toString(true) << std::endl;

    ASSERT_EQ(observed->toString(false), expected->toString(false));
    ASSERT_EQ(*expected, *observed);

}
// compose C-O-L join should produce one output tuple, order ID 210945




