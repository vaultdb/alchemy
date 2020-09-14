//
// Created by Salome Kariuki on 4/29/20.
//
#include "sort.h"

#include <util/emp_manager.h>
#include <operators/secure_sql_input.h>

#include <gflags/gflags.h>


#include "emp-tool/emp-tool.h"
#include "util/data_utilities.h"
#include <gtest/gtest.h>
#include <data/PsqlDataProvider.h>
#include <operators/project.h>


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43439, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for execution");
DEFINE_bool(input, false, "input value");



class SecureSortTest :  public  ::testing::Test  {
protected:
    void SetUp() override {};

    void TearDown() override {

        std::string leadDb =  FLAGS_party == 1 ? "tpch_alice" : "tpch_bob";
        pqxx::connection c("dbname=" + leadDb);
        pqxx::work w(c);

        w.exec("DROP TABLE tmp");


    };

    bool correctOrder(const QueryTuple & lhs, const QueryTuple & rhs, const SortDefinition & sortDefinition);
    bool isSorted(const std::shared_ptr<QueryTable> & table, const SortDefinition & sortDefinition);


};

// is lhs  <= rhs
// fails if either tuple is encrypted
bool SecureSortTest::correctOrder(const QueryTuple &lhs, const QueryTuple &rhs,  const SortDefinition & sortDefinition) {

    assert(lhs.getFieldCount() == rhs.getFieldCount());

    for(int i = 0; i < lhs.getFieldCount(); ++i) {
        types::Value lhsVal = lhs.getField(i).getValue();
        types::Value rhsVal = rhs.getField(i).getValue();

        if(sortDefinition[i].second == SortDirection::ASCENDING) {

            if ((lhsVal == rhsVal).getBool()) continue;
            types::Value gt = (lhs.getField(i).getValue() > rhs.getField(i).getValue());
            return (gt.getBool()) ? false : true;
        }
        else if(sortDefinition[i].second == SortDirection::DESCENDING) {
            if ((lhsVal == rhsVal).getBool()) continue;
            types::Value lt = (lhsVal < rhsVal);
            return (lt.getBool()) ? false : true;
        }
    }
    return true;
}

bool SecureSortTest::isSorted(const std::shared_ptr<QueryTable> & table, const SortDefinition & sortDefinition) {
    for(int i = 1; i < table->getTupleCount(); ++i) {

        QueryTuple previousTuple = table->getTuple(i-1);
        QueryTuple thisTuple = table->getTuple(i);

        if(!correctOrder(previousTuple, thisTuple, sortDefinition))  {
            return false;
        }// each tuple correctly ordered wrt its predecessor

    }
    return true;
}




// try the logic of Q1 in plaintext for warmup
TEST_F(SecureSortTest, tpchQ1SortClear) {

    std::string sql = "SELECT l_returnflag, l_linestatus FROM lineitem ORDER BY l_comment LIMIT 5"; // order by to ensure order is reproducible and not sorted on the sort cols

    SortDefinition sortDefinition;
    sortDefinition.emplace_back(0, SortDirection::ASCENDING);
    sortDefinition.emplace_back(1, SortDirection::ASCENDING);


    SqlInput *sqlInputPtr = new SqlInput("tpch_alice", "tpch_bob", sql);
    std::shared_ptr<Operator> input(sqlInputPtr);

    Sort *sortOp = new Sort(sortDefinition, input); // heap allocate it
    std::shared_ptr<Operator> sort = sortOp->getPtr();
    std::shared_ptr<QueryTable> result = sort->run();

    bool tableSorted = isSorted(result, sortDefinition);
    ASSERT_TRUE(tableSorted);
}

TEST_F(SecureSortTest, tpchQ1Sort) {
    EmpManager *empManager = EmpManager::getInstance();
    empManager->configureEmpManager(FLAGS_alice_host.c_str(), FLAGS_port, FLAGS_party);
    std::string dbName =  FLAGS_party == 1 ? "tpch_alice" : "tpch_bob";

    std::string sql = "SELECT l_returnflag, l_linestatus FROM lineitem ORDER BY l_comment LIMIT 5"; // order by to ensure order is reproducible and not sorted on the sort cols


    SortDefinition sortDefinition;
    sortDefinition.emplace_back(0, SortDirection::ASCENDING);
    sortDefinition.emplace_back(1, SortDirection::ASCENDING);


    std::shared_ptr<Operator> input(new SecureSqlInput(dbName, sql, false));
    Sort *sortOp = new Sort(sortDefinition, input); // heap allocate it
    std::shared_ptr<Operator> sort = sortOp->getPtr();
    std::shared_ptr<QueryTable> result = sort->run();

    std::shared_ptr<QueryTable> observed = result->reveal();
    bool tableSorted = isSorted(observed, sortDefinition);

    ASSERT_TRUE(tableSorted);

    empManager->close();

}


TEST_F(SecureSortTest, tpchQ3Sort) {
    EmpManager *empManager = EmpManager::getInstance();
    empManager->configureEmpManager(FLAGS_alice_host.c_str(), FLAGS_port, FLAGS_party);
    std::string dbName =  FLAGS_party == 1 ? "tpch_alice" : "tpch_bob";

    std::string sql = "SELECT l_orderkey, l.l_extendedprice * (1.0 - l.l_discount) revenue, o.o_shippriority, o_orderdate FROM lineitem l JOIN orders o ON l_orderkey = o_orderkey ORDER BY l_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols


    SortDefinition sortDefinition;
    sortDefinition.emplace_back(1, SortDirection::DESCENDING);
    sortDefinition.emplace_back(3, SortDirection::ASCENDING);

    std::shared_ptr<Operator> input(new SecureSqlInput(dbName, sql, false));
    Sort *sortOp = new Sort(sortDefinition, input); // heap allocate it
    std::shared_ptr<Operator> sort = sortOp->getPtr();


    // project it down to $1, $3
    Project *projectOp = new Project(sort);
    std::shared_ptr<Operator> project = projectOp->getPtr();
    projectOp->addColumnMapping(1, 0);
    projectOp->addColumnMapping(3, 1);

    std::shared_ptr<QueryTable> result = project->run();
    std::shared_ptr<QueryTable> observed = result->reveal();

    ASSERT_TRUE(isSorted(observed, sortDefinition));

    empManager->close();

}


TEST_F(SecureSortTest, tpchQ5Sort) {

    EmpManager *empManager = EmpManager::getInstance();
    empManager->configureEmpManager(FLAGS_alice_host.c_str(), FLAGS_port, FLAGS_party);
    std::string dbName =  FLAGS_party == 1 ? "tpch_alice" : "tpch_bob";
    std::string sql = "SELECT l_orderkey, l.l_extendedprice * (1 - l.l_discount) revenue FROM lineitem l  ORDER BY l_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols

    SortDefinition sortDefinition;
    sortDefinition.emplace_back(1, SortDirection::DESCENDING);

    std::shared_ptr<Operator> input(new SecureSqlInput(dbName, sql, false));
    Sort *sortOp = new Sort(sortDefinition, input); // heap allocate it
    std::shared_ptr<Operator> sort = sortOp->getPtr();


    // project it down to $1
    Project *projectOp = new Project(sort);
    std::shared_ptr<Operator> project = projectOp->getPtr();
    projectOp->addColumnMapping(1, 0);

    std::shared_ptr<QueryTable> result = project->run();
    std::shared_ptr<QueryTable> observed  = result->reveal();

    ASSERT_TRUE(isSorted(observed, sortDefinition));

    empManager->close();


}



TEST_F(SecureSortTest, tpchQ8Sort) {
    EmpManager *empManager = EmpManager::getInstance();
    empManager->configureEmpManager(FLAGS_alice_host.c_str(), FLAGS_port, FLAGS_party);
    std::string dbName =  FLAGS_party == 1 ? "tpch_alice" : "tpch_bob";

    std::string sql = "SELECT  o_orderyear, o_orderkey FROM orders o  ORDER BY o_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols

    SortDefinition sortDefinition;
    sortDefinition.emplace_back(0, SortDirection::ASCENDING);

    std::shared_ptr<Operator> input(new SecureSqlInput(dbName, sql, false));
    Sort *sortOp = new Sort(sortDefinition, input); // heap allocate it
    std::shared_ptr<Operator> sort = sortOp->getPtr();

    Project *projectOp = new Project(sort);
    std::shared_ptr<Operator> project = projectOp->getPtr();
    projectOp->addColumnMapping(0, 0);

    std::shared_ptr<QueryTable> result = project->run();
    std::shared_ptr<QueryTable> observed  = result->reveal();

    ASSERT_TRUE(isSorted(observed, sortDefinition));

    empManager->close();

}




TEST_F(SecureSortTest, tpchQ9Sort) {
    EmpManager *empManager = EmpManager::getInstance();
    empManager->configureEmpManager(FLAGS_alice_host.c_str(), FLAGS_port, FLAGS_party);
    std::string dbName =  FLAGS_party == 1 ? "tpch_alice" : "tpch_bob";

    std::string sql = "SELECT o_orderyear, o_orderkey, n_name FROM orders o JOIN lineitem l ON o_orderkey = l_orderkey"
                      "  JOIN supplier s ON s_suppkey = l_suppkey"
                      "  JOIN nation on n_nationkey = s_nationkey"
                      " ORDER BY l_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols



    SortDefinition sortDefinition;
    sortDefinition.emplace_back(2, SortDirection::ASCENDING);
    sortDefinition.emplace_back(0, SortDirection::DESCENDING);


    std::shared_ptr<Operator> input(new SecureSqlInput(dbName, sql, false));
    Sort *sortOp = new Sort(sortDefinition, input); // heap allocate it
    std::shared_ptr<Operator> sort = sortOp->getPtr();

    // project it down to $2, $0
    Project *projectOp = new Project(sort->getPtr());
    std::shared_ptr<Operator> project = projectOp->getPtr();
    projectOp->addColumnMapping(2, 0);
    projectOp->addColumnMapping(0, 1);

    std::shared_ptr<QueryTable> result = project->run();
    std::shared_ptr<QueryTable> observed  = result->reveal();

    std::cout << "Observed output: " << *observed << std::endl;
    ASSERT_TRUE(isSorted(observed, sortDefinition));

    empManager->close();

}


// 18
TEST_F(SecureSortTest, tpchQ18Sort) {
    EmpManager *empManager = EmpManager::getInstance();
    empManager->configureEmpManager(FLAGS_alice_host.c_str(), FLAGS_port, FLAGS_party);
    std::string dbName =  FLAGS_party == 1 ? "tpch_alice" : "tpch_bob";

    std::string sql = "SELECT o_orderkey, o_orderdate, o_totalprice FROM orders"
                      " ORDER BY o_comment, o_custkey LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols

    SortDefinition sortDefinition;
    sortDefinition.emplace_back(2, SortDirection::DESCENDING);
    sortDefinition.emplace_back(1, SortDirection::ASCENDING);

    std::shared_ptr<Operator> input(new SecureSqlInput(dbName, sql, false));
    Sort *sortOp = new Sort(sortDefinition, input); // heap allocate it
    std::shared_ptr<Operator> sort = sortOp->getPtr();


    // project it down to $2, $1
    Project *projectOp = new Project(sort->getPtr());
    std::shared_ptr<Operator> project = projectOp->getPtr();
    projectOp->addColumnMapping(2, 0);
    projectOp->addColumnMapping(1, 1);

    std::shared_ptr<QueryTable> result = project->run();
    std::shared_ptr<QueryTable> observed  = result->reveal();


    std::cout << "Observed output: " << *observed << std::endl;
    ASSERT_TRUE(isSorted(observed, sortDefinition));

    empManager->close();

}









int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}





