//
// Created by Salome Kariuki on 4/29/20.
//
#include "sort.h"
#include <util/emp_manager.h>
#include <operators/secure_sql_input.h>

#include <gflags/gflags.h>


#include "emp-tool/emp-tool.h"
#include <gtest/gtest.h>

DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43439, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for execution");
DEFINE_bool(input, false, "input value");

using namespace emp;
using namespace std;




class SecureSortTest : public ::testing::Test {
protected:
    void SetUp() override {};

    void TearDown() override {};

};
    // void testSingleIntColumn();
TEST_F(SecureSortTest,  testSingleIntColumn) {
    EmpManager *empManager = EmpManager::getInstance();
    empManager->configureEmpManager(FLAGS_alice_host.c_str(), FLAGS_port, FLAGS_party);
    std::string sql = "SELECT c_custkey FROM customer ORDER BY c_address LIMIT 5";  // c_address "randomizes" the order
    std::string expectedResult = "(#0 int32 customer.c_custkey) isEncrypted? 0\n"
                                 "(768)\n"
                                 "(1170)\n"
                                 "(1523)\n"
                                 "(2310)\n"
                                 "(2702)\n"
                                 "(3932)\n"
                                 "(6214)\n"
                                 "(6386)\n"
                                 "(6578)\n"
                                 "(7283)\n";

    std::string dbName =  FLAGS_party == 1 ? "tpch_alice" : "tpch_bob";

    SortDefinition sortDefinition;
    ColumnSort aColumnSort(0, SortDirection::ASCENDING);
    sortDefinition.columnOrders.push_back(aColumnSort);


    std::shared_ptr<Operator> input(new SecureSqlInput(dbName, sql, false));
    Sort *sortOp = new Sort(sortDefinition, input); // heap allocate it
    std::shared_ptr<Operator> sort = sortOp->getPtr();
    std::shared_ptr<QueryTable> result = sort->run();
    std::unique_ptr<QueryTable> revealed = result->reveal(emp::PUBLIC);

    std::cout << "Sorted table: \n" <<  *revealed << std::endl;

    ASSERT_EQ(revealed->toString(), expectedResult);


}

/*
TEST_F(SecureSortTest,  testTwoIntColumns) {
    EmpManager *empManager = EmpManager::getInstance();
    empManager->configureEmpManager(FLAGS_alice_host.c_str(), FLAGS_port,  FLAGS_party);
    std::string sql = "SELECT o_orderkey, o_custkey FROM orders LIMIT 10";
    std::string dbName =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";

    std::shared_ptr<SecureSqlInput> input(new SecureSqlInput(dbName, sql, false));
    std::shared_ptr<QueryTable> sqlOutput = input->run();


  vector<int> ordinals{0, 1};
  SortDefinition sortdef;
  sortdef.order = SortDirection::ASCENDING;
  sortdef.columnOrders = ordinals;
  int gates1 = ((HalfGateGen<NetIO> *)CircuitExecution::circ_exec)->gid;
  Sort(sqlOutput.get(), sortdef);
  int gates2 = ((HalfGateGen<NetIO> *)CircuitExecution::circ_exec)->gid;
  cout << gates2 - gates1 << endl;

    /** TODO: INSERT CORRECTNESS CHECKS **


}

TEST_F(SecureSortTest,  testSingleFloatColumnEncrypted) {


    EmpManager *empManager = EmpManager::getInstance();
    empManager->configureEmpManager(FLAGS_alice_host.c_str(), FLAGS_port, FLAGS_party);
    std::string sql = "SELECT c_acctbal FROM customer LIMIT 10";
    std::string dbName =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";

    std::shared_ptr<SecureSqlInput> input(new SecureSqlInput(dbName, sql, false));
    std::shared_ptr<QueryTable> sqlOutput = input->run();

  vector<int> ordinals{0};
  SortDefinition sortdef;
  sortdef.order = SortDirection::ASCENDING;
  sortdef.columnOrders = ordinals;
  int gates1 = ((HalfGateGen<NetIO> *)CircuitExecution::circ_exec)->gid;
  Sort(sqlOutput.get(), sortdef);
  int gates2 = ((HalfGateGen<NetIO> *)CircuitExecution::circ_exec)->gid;
  cout << gates2 - gates1 << endl;

    /** TODO: INSERT CORRECTNESS CHECKS **


}

TEST_F(SecureSortTest,  testSingleVarcharColumn) {

    EmpManager *empManager = EmpManager::getInstance();
    empManager->configureEmpManager(FLAGS_alice_host.c_str(), FLAGS_port, FLAGS_party);
    std::string sql = "SELECT o_orderkey, o_custkey FROM orders LIMIT 10";
    std::string dbName =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";

    std::shared_ptr<SecureSqlInput> input(new SecureSqlInput(dbName, sql, false));
    std::shared_ptr<QueryTable> sqlOutput = input->run();

  vector<int> ordinals{0};
  SortDefinition sortdef;
  sortdef.order = SortDirection::ASCENDING;
  sortdef.columnOrders = ordinals;
  int gates1 = ((HalfGateGen<NetIO> *)CircuitExecution::circ_exec)->gid;
  Sort(sqlOutput.get(), sortdef);
  int gates2 = ((HalfGateGen<NetIO> *)CircuitExecution::circ_exec)->gid;
  cout << gates2 - gates1 << endl;
    /** TODO: INSERT CORRECTNESS CHECKS **

}


// TODO: Sort by timestamp
// TODO: Sort by fixed length string
TEST_F(SecureSortTest, testSingleFloatColumnUnencrypted) {

    EmpManager *empManager = EmpManager::getInstance();
    empManager->configureEmpManager(FLAGS_alice_host.c_str(), FLAGS_port, FLAGS_party);
    std::string sql = "SELECT c_acctbal FROM customer LIMIT 10";
    std::string dbName =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";

    std::shared_ptr<SqlInput> input(new SqlInput(dbName, sql, false));
    std::shared_ptr<QueryTable> sqlOutput = input->run();


    vector<int> ordinals{0};
  SortDefinition sortdef;
  sortdef.order = SortDirection::ASCENDING;
  sortdef.columnOrders = ordinals;
  Sort(sqlOutput.get(), sortdef);

  /** TODO: INSERT CORRECTNESS CHECKS **

}

TEST_F(SecureSortTest, testLineItemSortDummyTag) {
    EmpManager *empManager = EmpManager::getInstance();
    empManager->configureEmpManager(FLAGS_alice_host.c_str(), FLAGS_port,  FLAGS_party);
    std::string sql = "SELECT l_orderkey FROM lineitem LIMIT 10";
    std::string dbName =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";

    std::shared_ptr<SqlInput> input(new SqlInput(dbName, sql, false));
    std::shared_ptr<QueryTable> sqlOutput = input->run();

  vector<int> ordinals{0, -1}; // -1 == dummy tag?
  SortDefinition sortdef;
  sortdef.dummyOrder = SortDirection::ASCENDING;
  sortdef.order = SortDirection::DESCENDING;
  sortdef.columnOrders = ordinals;
  Sort(sqlOutput.get(), sortdef);

    /** TODO: INSERT CORRECTNESS CHECKS **

}

*/

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}



//int main(int argc, char **argv) {

  //gflags::ParseCommandLineFlags(&argc, &argv, true /* remove_flags */);

 // testLineItemSort();
  // emp::NetIO *io = new emp::NetIO(
  // FLAGS_party == emp::ALICE ? nullptr : FLAGS_hostname.c_str(), FLAGS_port);
  // setup_semi_honest(io, FLAGS_party);
  // testSingleIntColumn();
  // testTwoIntColumns();
  //  testSingleFloatColumnEncrypted();
  // testSingleFloatColumnUnencrypted();
  // testSingleVarcharColumn();
//}