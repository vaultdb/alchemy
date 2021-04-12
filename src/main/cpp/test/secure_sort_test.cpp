#include <operators/secure_sql_input.h>
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <operators/project.h>
#include <test/support/EmpBaseTest.h>
#include <util/field_utilities.h>
#include <operators/sort.h>
#include <query_table/secure_tuple.h>




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
        const PlainField lhsVal = lhs.getField(i);
        const PlainField rhsVal = rhs.getField(i);

        if (lhsVal == rhsVal)
            continue;

        if(sortDefinition[i].second == SortDirection::ASCENDING) {
            return lhsVal <= rhsVal; //!FieldUtilities::gt(lhsVal, rhsVal);
        }
        else if(sortDefinition[i].second == SortDirection::DESCENDING) {
             return lhsVal >  rhsVal;
        }
    }
    return true;
}

bool SecureSortTest::isSorted(const std::shared_ptr<PlainTable> & table, const SortDefinition & sortDefinition) {

    for(uint32_t i = 1; i < table->getTupleCount(); ++i) {

        PlainTuple previousTuple = table->getTuple(i-1);
        PlainTuple thisTuple = table->getTuple(i);

        if(!correctOrder(previousTuple, thisTuple, sortDefinition))  {
            std::cout << "Incorrect order: " << previousTuple << " --> " << thisTuple << std::endl;
            return false;
        }// each tuple correctly ordered wrt its predecessor

    }
    return true;
}


/*

TEST_F(SecureSortTest, tpchQ1Sort) {
    std::string dbName =  FLAGS_party == 1 ? aliceDb : bobDb;

    string sql = "SELECT l_returnflag, l_linestatus FROM lineitem WHERE l_orderkey <= 10 ORDER BY l_comment"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expectedResultSql = "WITH input AS ("
            + sql
            + ") SELECT * FROM input ORDER BY l_returnflag, l_linestatus";
    shared_ptr<PlainTable > expected = DataUtilities::getQueryResults("tpch_unioned", expectedResultSql, false);


//    std::string sql = "SELECT l_returnflag, l_linestatus FROM lineitem WHERE l_orderkey <= 10 ORDER BY l_comment"; // order by to ensure order is reproducible and not sorted on the sort cols
//    std::string expectedSql = "SELECT l_returnflag, l_linestatus FROM lineitem WHERE l_orderkey <= 10 ORDER BY l_returnflag, l_linestatus";

    PsqlDataProvider dataProvider;

    std::shared_ptr<PlainTable>  inputTable = dataProvider.getQueryTable(dbName,
                                                                         sql, false);

    SortDefinition sortDefinition;
    sortDefinition.emplace_back(0, SortDirection::ASCENDING);
    sortDefinition.emplace_back(1, SortDirection::ASCENDING);
    expected->setSortOrder(sortDefinition);



    SecureSqlInput input(dbName, sql, false, netio, FLAGS_party);
    Sort<emp::Bit> sort(&input, sortDefinition);
    std::shared_ptr<SecureTable> result = sort.run();

    std::shared_ptr<PlainTable> observed = result->reveal();
    ASSERT_EQ(*expected, *observed);


}


TEST_F(SecureSortTest, tpchQ3Sort) {
    std::string dbName =  FLAGS_party == 1 ? aliceDb : bobDb;

    string sql = "SELECT l_orderkey, l.l_extendedprice * (1 - l.l_discount) revenue, o.o_shippriority, o_orderdate FROM lineitem l JOIN orders o ON l_orderkey = o_orderkey WHERE l_orderkey <= 10 ORDER BY l_comment"; // order by to ensure order is reproducible and not sorted on the sort cols

    string expectedResultSql = "WITH input AS (" + sql + ") SELECT revenue, " + DataUtilities::queryDatetime("o_orderdate")  + " FROM input ORDER BY revenue DESC, o_orderdate";
    shared_ptr<PlainTable> expected = DataUtilities::getQueryResults("tpch_unioned", expectedResultSql, false);

    SortDefinition sortDefinition;
    sortDefinition.emplace_back(1, SortDirection::DESCENDING);
    sortDefinition.emplace_back(3, SortDirection::ASCENDING);


    SecureSqlInput input(dbName, sql, false, netio, FLAGS_party);
    Sort<emp::Bit> sort(&input, sortDefinition);


    // project it down to $1, $3
    Project project(&sort);
    project.addColumnMapping(1, 0);
    project.addColumnMapping(3, 1);

    std::shared_ptr<SecureTable> result = project.run();
    std::shared_ptr<PlainTable> observed = result->reveal();

    // copy out the projected sort order
    expected->setSortOrder(observed->getSortOrder());


    ASSERT_EQ(*expected, *observed);

}


TEST_F(SecureSortTest, tpchQ5Sort) {

    string sql = "SELECT l_orderkey, l.l_extendedprice * (1 - l.l_discount) revenue FROM lineitem l WHERE l_orderkey <= 10  ORDER BY l_comment"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expectedResultSql = "WITH input AS (" + sql + ") SELECT revenue FROM input ORDER BY revenue DESC";
    shared_ptr<PlainTable > expected = DataUtilities::getQueryResults("tpch_unioned", expectedResultSql, false);

    SortDefinition sortDefinition;
    sortDefinition.emplace_back(1, SortDirection::DESCENDING);

    SecureSqlInput input(dbName, sql, false, netio, FLAGS_party);
    Sort<emp::Bit> sort(&input, sortDefinition);


    // project it down to $1
    Project project(&sort);
    project.addColumnMapping(1, 0);

    std::shared_ptr<SecureTable> result = project.run();
    std::shared_ptr<PlainTable> observed  = result->reveal();

    // copy out the projected sort order
    expected->setSortOrder(observed->getSortOrder());

    ASSERT_EQ(*expected, *observed);



}



TEST_F(SecureSortTest, tpchQ8Sort) {
    std::string dbName =  FLAGS_party == 1 ? aliceDb : bobDb;

    string sql = "SELECT  o_orderyear, o_orderkey FROM orders o  WHERE o_orderkey <= 10 ORDER BY o_comment, o_orderkey"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expectedResultSql = "WITH input AS (" + sql + ") SELECT o_orderyear FROM input ORDER BY o_orderyear, o_orderkey DESC";  // orderkey DESC needed to align with psql's layout
    shared_ptr<PlainTable > expected = DataUtilities::getQueryResults("tpch_unioned", expectedResultSql, false);

    SortDefinition sortDefinition;
    sortDefinition.emplace_back(0, SortDirection::ASCENDING);


    SecureSqlInput input(dbName, sql, false, netio, FLAGS_party);
    Sort<emp::Bit> sort(&input, sortDefinition);

    Project project(&sort);
    project.addColumnMapping(0, 0);

    std::shared_ptr<SecureTable> result = project.run();
    std::shared_ptr<PlainTable> observed  = result->reveal();

    // copy out the projected sort order
    expected->setSortOrder(observed->getSortOrder());


    ASSERT_EQ(*expected, *observed);


}


*/
TEST_F(SecureSortTest, tpchQ9Sort) {
    std::string dbName =  FLAGS_party == 1 ? aliceDb : bobDb;

    std::string sql = "SELECT o_orderyear, o_orderkey, n_name FROM orders o JOIN lineitem l ON o_orderkey = l_orderkey"
                      "  JOIN supplier s ON s_suppkey = l_suppkey"
                      "  JOIN nation on n_nationkey = s_nationkey"
                      " ORDER BY l_comment LIMIT 1000"; //  order by to ensure order is reproducible and not sorted on the sort cols



    SortDefinition sortDefinition;
    sortDefinition.emplace_back(2, SortDirection::ASCENDING);
    sortDefinition.emplace_back(0, SortDirection::DESCENDING);


    SecureSqlInput input(dbName, sql, false, netio, FLAGS_party);
    Sort sort(&input, sortDefinition);

    // project it down to $2, $0
    Project project(&sort);

    project.addColumnMapping(2, 0);
    project.addColumnMapping(0, 1);

    std::shared_ptr<SecureTable> result = project.run();
    std::shared_ptr<PlainTable> observed  = result->reveal();

    ASSERT_TRUE(isSorted(observed, sortDefinition));

    // JMR: can't easily do this one securely owing to lineitem-supplier join -- tried with pkey/fkey join, but takes 1000s of tuples.
    // validating this one manually instead

    /*  With join (too expensive @ TPC-H SF1)
     * string sql = "SELECT o_orderyear, o_orderkey, n_name "
                 "  FROM orders o JOIN lineitem l ON o_orderkey = l_orderkey"
                 "  JOIN supplier s ON s_suppkey = l_suppkey"
                 "  JOIN nation on n_nationkey = s_nationkey"
                 "  WHERE l_orderkey <= 10 AND s_suppkey <= 5000"
                 " ORDER BY l_comment "; // order by to ensure order is reproducible and not sorted on the ORDER BY cols

                 std::cout << "Input SQL: " << sql << std::endl;


                 // lineitem and orders are partitioned-alike
                 // need to do secure join between lineitem and supplier

    string expectedResultSql = "WITH input AS (" + sql + ") SELECT n_name, o_orderyear FROM input ORDER BY n_name, o_orderyear DESC";
    shared_ptr<PlainTable > expected = DataUtilities::getQueryResults("tpch_unioned", expectedResultSql, false);


    std::string lineitemSql = "SELECT o_orderyear, l_orderkey, l_suppkey "
                                                                "  FROM orders o JOIN lineitem l ON o_orderkey = l_orderkey"
                                                                "  WHERE l_orderkey <= 10 "
                                                                " ORDER BY l_comment ";

    std::string supplierSql = "SELECT s_suppkey, n_name "
                         "  FROM  supplier JOIN nation on n_nationkey = s_nationkey"
                         " WHERE s_suppkey <= 5000 "
                         " ORDER BY s_suppkey";

    std::cout << " supplierSql: " << supplierSql << std::endl;



    SecureSqlInput lineitemInput(dbName, lineitemSql, false, netio, FLAGS_party);
    SecureSqlInput supplierInput(dbName, supplierSql, false, netio, FLAGS_party);


    ConjunctiveEqualityPredicate lineitemSupplierOrdinals = {EqualityPredicate(2, 0)}; //  l_suppkey, s_suppkey

    std::shared_ptr<BinaryPredicate<emp::Bit> > lineitemSupplierPredicate(new JoinEqualityPredicate<emp::Bit>(lineitemSupplierOrdinals));

    BasicJoin<emp::Bit> join(&lineitemInput, &supplierInput, lineitemSupplierPredicate);


    SortDefinition sortDefinition;
    sortDefinition.emplace_back(4, SortDirection::ASCENDING);
    sortDefinition.emplace_back(0, SortDirection::DESCENDING);

    Sort<emp::Bit> sort(&join, sortDefinition);

    // project it down to $5, $0
    Project project(&sort);

    project.addColumnMapping(4, 0); // n_name
    project.addColumnMapping(0, 1); // o_orderyear

    std::shared_ptr<SecureTable> result = project.run();
    std::shared_ptr<PlainTable> observed  = result->reveal();

    // copy out the projected sort order
    expected->setSortOrder(observed->getSortOrder());

    ASSERT_EQ(*expected, *observed); */


}
/*
// 18
TEST_F(SecureSortTest, tpchQ18Sort) {
    std::string dbName =  FLAGS_party == 1 ? aliceDb : bobDb;

    string sql = "SELECT o_orderkey, o_orderdate, o_totalprice FROM orders WHERE o_orderkey <= 10 "
                 " ORDER BY o_comment, o_custkey"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expectedResultSql = "WITH input AS (" + sql + ") SELECT o_totalprice, " + DataUtilities::queryDatetime("o_orderdate") + "  FROM input ORDER BY o_totalprice DESC, o_orderdate";


    shared_ptr<PlainTable > expected = DataUtilities::getQueryResults("tpch_unioned", expectedResultSql, false);

    SortDefinition sortDefinition;
    sortDefinition.emplace_back(2, SortDirection::DESCENDING);
    sortDefinition.emplace_back(1, SortDirection::ASCENDING);

    SecureSqlInput input(dbName, sql, false, netio, FLAGS_party);
    Sort<emp::Bit> sort(&input, sortDefinition);


    // project it down to $2, $1

    Project project(&sort);
    project.addColumnMapping(2, 0);
    project.addColumnMapping(1, 1);

    std::shared_ptr<SecureTable> result = project.run();
    std::shared_ptr<PlainTable> observed  = result->reveal();

    // copy out the projected sort order
    expected->setSortOrder(observed->getSortOrder());


    // verify that first col is DESC, second is ASC
    ASSERT_EQ(*expected, *observed);


}
*/





int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}





