#include "sort.h"

#include <operators/secure_sql_input.h>
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <operators/project.h>
#include <test/support/EmpBaseTest.h>
#include <util/field_utilities.h>


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43439, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for execution");
DEFINE_bool(input, false, "input value");


using namespace vaultdb;

class SecureSortTest :  public EmpBaseTest {
protected:

    static bool correctOrder(const PlainTuple & lhs, const PlainTuple & rhs, const SortDefinition & sortDefinition);
    static bool isSorted(const std::shared_ptr<PlainTable> & table, const SortDefinition & sortDefinition);


};

// is lhs  <= rhs
// fails if either tuple is encrypted
bool SecureSortTest::correctOrder(const PlainTuple &lhs, const PlainTuple &rhs,  const SortDefinition & sortDefinition) {

    assert(lhs.getFieldCount() == rhs.getFieldCount());

    for(uint32_t i = 0; i < lhs.getFieldCount(); ++i) {
        const PlainField *lhsVal = lhs.getField(i);
        const PlainField *rhsVal = rhs.getField(i);

        if ((*lhsVal == *rhsVal).getBool())
            continue;

        if(sortDefinition[i].second == SortDirection::ASCENDING) {
            return (*lhsVal <= *rhsVal).getBool(); //!FieldUtilities::gt(lhsVal, rhsVal);
        }
        else if(sortDefinition[i].second == SortDirection::DESCENDING) {
             return (*lhsVal >  *rhsVal).getBool();
        }
    }
    return true;
}

bool SecureSortTest::isSorted(const std::shared_ptr<PlainTable> & table, const SortDefinition & sortDefinition) {
    for(uint32_t i = 1; i < table->getTupleCount(); ++i) {

        PlainTuple previousTuple = table->getTuple(i-1);
        PlainTuple thisTuple = table->getTuple(i);

        if(!correctOrder(previousTuple, thisTuple, sortDefinition))  {
            return false;
        }// each tuple correctly ordered wrt its predecessor

    }
    return true;
}




TEST_F(SecureSortTest, tpchQ1Sort) {
    std::string dbName =  FLAGS_party == 1 ? aliceDb : bobDb;

    std::string sql = "SELECT l_returnflag, l_linestatus FROM lineitem ORDER BY l_comment LIMIT 5"; // order by to ensure order is reproducible and not sorted on the sort cols

    PsqlDataProvider dataProvider;

    std::unique_ptr<PlainTable>  inputTable = dataProvider.getQueryTable(dbName,
                                                                         sql, false);

    SortDefinition sortDefinition;
    sortDefinition.emplace_back(0, SortDirection::ASCENDING);
    sortDefinition.emplace_back(1, SortDirection::ASCENDING);



    SecureSqlInput input(dbName, sql, false, netio, FLAGS_party);
    Sort<SecureBoolField> sort(&input, sortDefinition);
    std::shared_ptr<SecureTable> result = sort.run();

    std::shared_ptr<PlainTable> observed = result->reveal();


    bool tableSorted = isSorted(observed, sortDefinition);

    ASSERT_TRUE(tableSorted);


}


TEST_F(SecureSortTest, tpchQ3Sort) {
    std::string dbName =  FLAGS_party == 1 ? aliceDb : bobDb;

    std::string sql = "SELECT l_orderkey, l.l_extendedprice * (1.0 - l.l_discount) revenue, o.o_shippriority, o_orderdate FROM lineitem l JOIN orders o ON l_orderkey = o_orderkey ORDER BY l_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols


    SortDefinition sortDefinition;
    sortDefinition.emplace_back(1, SortDirection::DESCENDING);
    sortDefinition.emplace_back(3, SortDirection::ASCENDING);

    SecureSqlInput input(dbName, sql, false, netio, FLAGS_party);
    Sort<SecureBoolField> sort(&input, sortDefinition);


    // project it down to $1, $3
    Project project(&sort);
    project.addColumnMapping(1, 0);
    project.addColumnMapping(3, 1);

    std::shared_ptr<SecureTable> result = project.run();
    std::shared_ptr<PlainTable> observed = result->reveal();

    ASSERT_TRUE(isSorted(observed, sortDefinition));

}


TEST_F(SecureSortTest, tpchQ5Sort) {

    std::string dbName =  FLAGS_party == 1 ? aliceDb : bobDb;
    std::string sql = "SELECT l_orderkey, l.l_extendedprice * (1 - l.l_discount) revenue FROM lineitem l  ORDER BY l_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols

    SortDefinition sortDefinition;
    sortDefinition.emplace_back(1, SortDirection::DESCENDING);

    SecureSqlInput input(dbName, sql, false, netio, FLAGS_party);
    Sort<SecureBoolField> sort(&input, sortDefinition);


    // project it down to $1
    Project project(&sort);
    project.addColumnMapping(1, 0);

    std::shared_ptr<SecureTable> result = project.run();
    std::shared_ptr<PlainTable> observed  = result->reveal();

    ASSERT_TRUE(isSorted(observed, sortDefinition));



}



TEST_F(SecureSortTest, tpchQ8Sort) {
    std::string dbName =  FLAGS_party == 1 ? aliceDb : bobDb;

    std::string sql = "SELECT  o_orderyear, o_orderkey FROM orders o  ORDER BY o_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols

    SortDefinition sortDefinition;
    sortDefinition.emplace_back(0, SortDirection::ASCENDING);

    SecureSqlInput input(dbName, sql, false, netio, FLAGS_party);
    Sort<SecureBoolField> sort(&input, sortDefinition);

    Project project(&sort);
    project.addColumnMapping(0, 0);

    std::shared_ptr<SecureTable> result = project.run();
    std::shared_ptr<PlainTable> observed  = result->reveal();

    ASSERT_TRUE(isSorted(observed, sortDefinition));


}



TEST_F(SecureSortTest, tpchQ9Sort) {
    std::string dbName =  FLAGS_party == 1 ? aliceDb : bobDb;

    std::string sql = "SELECT o_orderyear, o_orderkey, n_name FROM orders o JOIN lineitem l ON o_orderkey = l_orderkey"
                      "  JOIN supplier s ON s_suppkey = l_suppkey"
                      "  JOIN nation on n_nationkey = s_nationkey"
                      " ORDER BY l_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols



    SortDefinition sortDefinition;
    sortDefinition.emplace_back(2, SortDirection::ASCENDING);
    sortDefinition.emplace_back(0, SortDirection::DESCENDING);


    SecureSqlInput input(dbName, sql, false, netio, FLAGS_party);
    Sort<SecureBoolField> sort(&input, sortDefinition);

    // project it down to $2, $0
    Project project(&sort);

    project.addColumnMapping(2, 0);
    project.addColumnMapping(0, 1);

    std::shared_ptr<SecureTable> result = project.run();
    std::shared_ptr<PlainTable> observed  = result->reveal();

    ASSERT_TRUE(isSorted(observed, sortDefinition));

}

// 18
TEST_F(SecureSortTest, tpchQ18Sort) {
    std::string dbName =  FLAGS_party == 1 ? aliceDb : bobDb;


    std::string sql = "SELECT o_orderkey, o_orderdate, o_totalprice FROM orders"
                      " ORDER BY o_comment, o_custkey LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols

    SortDefinition sortDefinition;
    sortDefinition.emplace_back(2, SortDirection::DESCENDING);
    sortDefinition.emplace_back(1, SortDirection::ASCENDING);

    SecureSqlInput input(dbName, sql, false, netio, FLAGS_party);
    Sort<SecureBoolField> sort(&input, sortDefinition);


    // project it down to $2, $1

    Project project(&sort);
    project.addColumnMapping(2, 0);
    project.addColumnMapping(1, 1);

    std::shared_ptr<SecureTable> result = project.run();
    std::shared_ptr<PlainTable> observed  = result->reveal();

    ASSERT_TRUE(isSorted(observed, sortDefinition));


}







int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}





