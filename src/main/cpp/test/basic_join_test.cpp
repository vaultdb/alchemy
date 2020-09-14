//
// Created by Jennie Rogers on 9/13/20.
//

#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/support/binary_predicate.h>
#include <operators/support/join_equality_predicate.h>
#include <operators/basic_join.h>
#include <operators/project.h>


using namespace emp;
using namespace vaultdb::types;



class BasicJoinTest : public ::testing::Test {


protected:
    void SetUp() override {};
    void TearDown() override{};
    const std::string dbName = "tpch_unioned";
};





TEST_F(BasicJoinTest, test_tpch_q3_customer_orders) {

    // get inputs from local oblivious ops
    // first 5 customers, propagate this constraint up the join tree for the test
    std::string customerSql = "SELECT c_custkey, c_mktsegment <> 'HOUSEHOLD' cdummy "
                              "FROM customer  "
                              "WHERE c_custkey <= 5 "
                              "ORDER BY c_custkey";

    std::string ordersSql = "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' odummy "
                           "FROM orders "
                           "WHERE o_custkey <= 5 ";

   std::string expectedResultSql = "WITH customer_cte AS (" + customerSql + "), "
                                        "orders_cte AS (" + ordersSql + ") "
                                        "SELECT o_orderkey, o_custkey,(cdummy OR odummy OR o_custkey <> c_custkey) dummy "
                                        "FROM customer_cte, orders_cte "
                                        "ORDER BY o_orderkey, o_custkey";

   std::cout << "Expected results SQL: " << expectedResultSql <<  std::endl;

   std::shared_ptr<QueryTable> expected = DataUtilities::getQueryResults(dbName, expectedResultSql, true);

    std::shared_ptr<Operator> customerInput(new SqlInput(dbName, customerSql, true));
    std::shared_ptr<Operator> ordersInput(new SqlInput(dbName, ordersSql, true));


    ConjunctiveEqualityPredicate customerOrdersOrdinals;
    customerOrdersOrdinals.push_back(EqualityPredicate (1, 0)); //  o_custkey, c_custkey

    std::shared_ptr<BinaryPredicate> customerOrdersPredicate(new JoinEqualityPredicate(customerOrdersOrdinals, false));

    BasicJoin *joinOp = new BasicJoin(customerOrdersPredicate, ordersInput, customerInput);


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


TEST_F(BasicJoinTest, test_tpch_q3_lineitem_orders) {

    // get inputs from local oblivious ops
    // first 3 customers, propagate this constraint up the join tree for the test


    std::string ordersSql = "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' odummy "
                            "FROM orders "
                            "WHERE o_custkey <= 5 "
                            "ORDER BY o_orderkey, o_orderdate";

    std::string lineitemSql = "SELECT  l_orderkey, l_extendedprice * (1 - l_discount) revenue, l_shipdate <= date '1995-03-25' ldummy "
                           "FROM lineitem "
                           "WHERE l_orderkey IN (SELECT o_orderkey FROM orders where o_custkey <= 5)  "
                           "ORDER BY l_orderkey, l_linenumber";
    std::string expectedResultSql = "WITH orders_cte AS (" + ordersSql + "), "
                                        "lineitem_cte AS (" + lineitemSql + ") "
                                        "SELECT l_orderkey, revenue, o_orderdate, o_shippriority,(odummy OR ldummy OR o_orderkey <> l_orderkey) dummy "
                                         "FROM lineitem_cte, orders_cte "
                                          "ORDER BY l_orderkey, o_orderdate";

    std::cout << "Expected results SQL: " << expectedResultSql <<  std::endl;

    std::shared_ptr<QueryTable> expected = DataUtilities::getQueryResults(dbName, expectedResultSql, true);

    std::shared_ptr<Operator> lineitemInput(new SqlInput(dbName, lineitemSql, true));
    std::shared_ptr<Operator> ordersInput(new SqlInput(dbName, ordersSql, true));


    ConjunctiveEqualityPredicate lineitemOrdersOrdinals;
    lineitemOrdersOrdinals.push_back(EqualityPredicate (0, 0)); //  l_orderkey, o_orderkey

    std::shared_ptr<BinaryPredicate> customerOrdersPredicate(new JoinEqualityPredicate(lineitemOrdersOrdinals, false));

    BasicJoin *joinOp = new BasicJoin(customerOrdersPredicate, lineitemInput, ordersInput);


    Project *projectOp = new Project(joinOp->getPtr());
    projectOp->addColumnMapping(0, 0); // l_orderkey
    projectOp->addColumnMapping(1, 1); // revenue
    projectOp->addColumnMapping(4, 2); // o_orderdate
    projectOp->addColumnMapping(5, 3); // o_shippriority
    std::shared_ptr<Operator> project = projectOp->getPtr();

    std::shared_ptr<QueryTable> observed = project->run();

    std::cout << "customer input: " << std::endl << lineitemInput->getOutput()->toString(true);
    std::cout << "orders input: " << std::endl << ordersInput->getOutput()->toString(true);

    std::cout << "join output: " << std::endl << joinOp->getOutput()->toString(false) << std::endl;

    std::cout << "project output: " << std::endl << project->getOutput()->toString(false) << std::endl;


    ASSERT_EQ(project->getOutput()->toString(false), expected->toString(false));
    ASSERT_EQ(*expected, *observed);

}
// compose C-O-L join shoud produce one output tuple, order ID 210945



