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
    const std::string dbName = "tpch_alice";
};





TEST_F(BasicJoinTest, test_tpch_q3_customer_orders) {

    // get inputs from local oblivious ops
    // first 3 customers, propagate this constraint up the join tree for the test
    std::string customerSql = "SELECT c_custkey, c_mktsegment <> 'HOUSEHOLD' cdummy "
                              "FROM customer  "
                              "WHERE c_custkey <= 5 "
                              "ORDER BY c_custkey";

    std::string ordersSql = "SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, o_orderdate >= date '1995-03-25' odummy "
                           "FROM orders "
                           "WHERE o_custkey <= 5 ";

   std::string expectedResultSql = "WITH customerCTE AS (" + customerSql + "), "
                                        "ordersCTE AS (" + ordersSql + ") "
                                        "SELECT o_orderkey, o_custkey,(cdummy OR odummy OR o_custkey <> c_custkey) dummy "
                                        "FROM customerCTE, ordersCTE "
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


/* std::string lineitemSql = "SELECT  l_orderkey, l.l_extendedprice * (1 - l.l_discount) revenue, l.l_shipdate > date '1995-03-25 dummy "
                           "FROM lineitem"
                           "WHERE l_orderkey IN (SELECT o_orderkey FROM orders where o_custkey <= 3) "
                           "ORDER BY l_orderkey, l_line"; */