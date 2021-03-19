#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/filter.h>
#include <operators/sort.h>
#include <operators/project.h>

using namespace emp;
using namespace vaultdb;


class SortTest : public ::testing::Test {


protected:
    void SetUp() override { setup_plain_prot(false, ""); };
    void TearDown() override{  finalize_plain_prot(); };

    Sort<BoolField> getSort(const string & srcSql, const SortDefinition & sortDefinition);

    const string dbName = "tpch_unioned";

};





TEST_F(SortTest, testSingleIntColumn) {

    string sql = "SELECT c_custkey FROM customer ORDER BY c_address, c_custkey LIMIT 10";  // c_address "randomizes" the order
    string expectedSql = "SELECT c_custkey FROM (" + sql + ") subquery ORDER BY c_custkey";

    shared_ptr<QueryTable<BoolField> > expected = DataUtilities::getQueryResults(dbName, expectedSql, false);

    SortDefinition sortDefinition;
    ColumnSort aColumnSort(0, SortDirection::ASCENDING);
    sortDefinition.push_back(aColumnSort);

    Sort<BoolField> sort = getSort(sql, sortDefinition);
    shared_ptr<QueryTable<BoolField> > observed = sort.run();

    expected->setSortOrder(observed->getSortOrder());
    ASSERT_EQ(*expected, *observed);


}

TEST_F(SortTest, tpchQ1Sort) {
    string sql = "SELECT l_returnflag, l_linestatus FROM lineitem ORDER BY l_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expectedResultSql = "WITH input AS (SELECT l_returnflag, l_linestatus FROM lineitem ORDER BY l_comment LIMIT 10) SELECT * FROM input ORDER BY l_returnflag, l_linestatus";
    shared_ptr<QueryTable<BoolField> > expected = DataUtilities::getQueryResults(dbName, expectedResultSql, false);


    SortDefinition sortDefinition;
    sortDefinition.emplace_back(0, SortDirection::ASCENDING);
    sortDefinition.emplace_back(1, SortDirection::ASCENDING);
    expected->setSortOrder(sortDefinition);

    Sort<BoolField> sort = getSort(sql, sortDefinition);
    shared_ptr<QueryTable<BoolField> > observed = sort.run();

    // no projection needed here
    ASSERT_EQ(*expected, *observed);

}

TEST_F(SortTest, tpchQ3Sort) {

    string sql = "SELECT l_orderkey, l.l_extendedprice * (1 - l.l_discount) revenue, o.o_shippriority, o_orderdate FROM lineitem l JOIN orders o ON l_orderkey = o_orderkey ORDER BY l_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols

    string expectedResultSql = "WITH input AS (" + sql + ") SELECT revenue, " + DataUtilities::queryDatetime("o_orderdate")  + " FROM input ORDER BY revenue DESC, o_orderdate";
    shared_ptr<QueryTable<BoolField> > expected = DataUtilities::getQueryResults(dbName, expectedResultSql, false);


    SortDefinition sortDefinition;
    sortDefinition.emplace_back(1, SortDirection::DESCENDING);
    sortDefinition.emplace_back(3, SortDirection::ASCENDING);


   Sort<BoolField> sort = getSort(sql, sortDefinition);


    // project it down to $1, $3
    Project project(&sort);
    project.addColumnMapping(1, 0);
    project.addColumnMapping(3, 1);

    shared_ptr<QueryTable<BoolField> > observed = project.run();

    // update sort def to account for projection -- also testing sort order carryover - the metadata in QueryTable<BoolField>  describing sorted order of its contents
    sortDefinition[0].first = 0;
    sortDefinition[1].first = 1;

    expected->setSortOrder(sortDefinition);

    ASSERT_EQ(*expected, *observed);

}


TEST_F(SortTest, tpchQ5Sort) {

    string sql = "SELECT l_orderkey, l.l_extendedprice * (1 - l.l_discount) revenue FROM lineitem l  ORDER BY l_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expectedResultSql = "WITH input AS (" + sql + ") SELECT revenue FROM input ORDER BY revenue DESC";
    shared_ptr<QueryTable<BoolField> > expected = DataUtilities::getQueryResults(dbName, expectedResultSql, false);

    SortDefinition sortDefinition;
    sortDefinition.emplace_back(1, SortDirection::DESCENDING);

   Sort<BoolField> sort = getSort(sql, sortDefinition);


    // project it down to $1
    Project project(&sort);
    project.addColumnMapping(1, 0);


    // update sort def to account for projection -- also testing sort order carryover - the metadata in QueryTable<BoolField>  describing sorted order of its contents
    sortDefinition[0].first = 0;
    expected->setSortOrder(sortDefinition);

    shared_ptr<QueryTable<BoolField> > observed = project.run();

    ASSERT_EQ(*expected, *observed);

}



TEST_F(SortTest, tpchQ8Sort) {

    string sql = "SELECT  o_orderyear, o_orderkey FROM orders o  ORDER BY o_comment, o_orderkey LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expectedResultSql = "WITH input AS (" + sql + ") SELECT o_orderyear FROM input ORDER BY o_orderyear, o_orderkey DESC";  // orderkey DESC needed to align with psql's layout
    shared_ptr<QueryTable<BoolField> > expected = DataUtilities::getQueryResults(dbName, expectedResultSql, false);

    SortDefinition sortDefinition;
    sortDefinition.emplace_back(0, SortDirection::ASCENDING);
    expected->setSortOrder(sortDefinition);


   Sort<BoolField> sort = getSort(sql, sortDefinition);


    // project it down to $0
    Project project(&sort);
    project.addColumnMapping(0, 0);
    shared_ptr<QueryTable<BoolField> > observed = project.run();

    ASSERT_EQ(*expected, *observed);
}




TEST_F(SortTest, tpchQ9Sort) {

    string sql = "SELECT o_orderyear, o_orderkey, n_name FROM orders o JOIN lineitem l ON o_orderkey = l_orderkey"
                      "  JOIN supplier s ON s_suppkey = l_suppkey"
                      "  JOIN nation on n_nationkey = s_nationkey"
                      " ORDER BY l_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expectedResultSql = "WITH input AS (" + sql + ") SELECT n_name, o_orderyear FROM input ORDER BY n_name, o_orderyear DESC";

    shared_ptr<QueryTable<BoolField> > expected = DataUtilities::getQueryResults(dbName, expectedResultSql, false);



    SortDefinition sortDefinition;
    sortDefinition.emplace_back(2, SortDirection::ASCENDING);
    sortDefinition.emplace_back(0, SortDirection::DESCENDING);

   Sort<BoolField> sort = getSort(sql, sortDefinition);

    // project it down to $0
    Project project(&sort);
    project.addColumnMapping(2, 0);
    project.addColumnMapping(0, 1);

    shared_ptr<QueryTable<BoolField> > observed = project.run();


    sortDefinition[0].first = 0;
    sortDefinition[1].first = 1;
    expected->setSortOrder(sortDefinition);

    ASSERT_EQ(*expected, *observed);


}



// 18
TEST_F(SortTest, tpchQ18Sort) {

    string sql = "SELECT o_orderkey, o_orderdate, o_totalprice FROM orders"
                      " ORDER BY o_comment, o_custkey LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expectedResultSql = "WITH input AS (" + sql + ") SELECT o_totalprice, " + DataUtilities::queryDatetime("o_orderdate") + "  FROM input ORDER BY o_totalprice DESC, o_orderdate";


    shared_ptr<QueryTable<BoolField> > expected = DataUtilities::getQueryResults(dbName, expectedResultSql, false);


    SortDefinition sortDefinition;
    sortDefinition.emplace_back(2, SortDirection::DESCENDING);
    sortDefinition.emplace_back(1, SortDirection::ASCENDING);
    expected->setSortOrder(sortDefinition);


   Sort<BoolField> sort = getSort(sql, sortDefinition);

    // project it down to $2, $1
    Project project(&sort);
    project.addColumnMapping(2, 0);
    project.addColumnMapping(1, 1);

    shared_ptr<QueryTable<BoolField> > observed = project.run();


    sortDefinition[0].first = 0;
    sortDefinition[1].first = 1;
    expected->setSortOrder(sortDefinition);

    ASSERT_EQ(*expected, *observed);


}






Sort<BoolField> SortTest::getSort(const string &srcSql, const SortDefinition &sortDefinition) {
    SqlInput input(dbName, srcSql, false);
    Sort<BoolField> sort(&input, sortDefinition); // heap allocate it

    // cache sort result
    sort.run();
    return sort;
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}

