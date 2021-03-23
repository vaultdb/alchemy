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

    shared_ptr<PlainTable > expected = DataUtilities::getQueryResults(dbName, expectedSql, false);

    SortDefinition sortDefinition;
    ColumnSort aColumnSort(0, SortDirection::ASCENDING);
    sortDefinition.push_back(aColumnSort);

    Sort<BoolField> sort = getSort(sql, sortDefinition);
    shared_ptr<PlainTable > observed = sort.run();

    expected->setSortOrder(observed->getSortOrder());
    ASSERT_EQ(*expected, *observed);


}


TEST_F(SortTest, cmpSwap) {
    string sql = "SELECT * FROM lineitem ORDER BY l_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    shared_ptr<PlainTable > data = DataUtilities::getQueryResults(dbName, sql, false);

    PlainTuple a = (*data)[0];
    PlainTuple b = (*data)[1];
    BoolField swap(true);

    PlainTuple::compareAndSwap(swap, &a, &b);

    // swapped
    ASSERT_EQ(a, (*data)[1]);
    ASSERT_EQ(b, (*data)[0]);


    // no swap
    swap = BoolField(false);
    PlainTuple::compareAndSwap(swap, &a, &b);
    ASSERT_EQ(a, (*data)[1]);
    ASSERT_EQ(b, (*data)[0]);

}

TEST_F(SortTest, tpchQ1Sort) {
    // was LIMIT 10
    string sql = "SELECT l_returnflag, l_linestatus FROM lineitem ORDER BY l_linenumber, l_comment LIMIT 4"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expectedResultSql = "WITH input AS (" + sql + ") SELECT * FROM input ORDER BY l_returnflag, l_linestatus";
    shared_ptr<PlainTable > expected = DataUtilities::getQueryResults(dbName, expectedResultSql, false);


    SortDefinition sortDefinition;
    sortDefinition.emplace_back(0, SortDirection::ASCENDING);
    sortDefinition.emplace_back(1, SortDirection::ASCENDING);
    expected->setSortOrder(sortDefinition);

    Sort<BoolField> sort = getSort(sql, sortDefinition);
    shared_ptr<PlainTable > observed = sort.run();

    // no projection needed here
    ASSERT_EQ(*expected, *observed);

}

TEST_F(SortTest, tpchQ3Sort) {
    // casting revenue to INT for now b/c floating point is more complicated to debug - TODO: needs its own testbed
    string sql = "SELECT l_orderkey, (l.l_extendedprice * (1 - l.l_discount))::INT revenue, o.o_shippriority, o_orderdate FROM lineitem l JOIN orders o ON l_orderkey = o_orderkey ORDER BY  l_linenumber, l_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols

    string expectedResultSql = "WITH input AS (" + sql + ") SELECT revenue, " + DataUtilities::queryDatetime("o_orderdate")  + " FROM input ORDER BY revenue DESC, o_orderdate";
    shared_ptr<PlainTable> expected = DataUtilities::getQueryResults(dbName, expectedResultSql, false);


    SortDefinition sortDefinition;
    sortDefinition.emplace_back(1, SortDirection::DESCENDING);
    sortDefinition.emplace_back(3, SortDirection::ASCENDING);


    Sort sort = getSort(sql, sortDefinition);

    std::cout << "Sort output: " << *(sort.run()) << std::endl;


    // project it down to $1, $3
    Project project(&sort);
    project.addColumnMapping(1, 0);
    project.addColumnMapping(3, 1);

    shared_ptr<PlainTable> observed = project.run();

    // update sort def to account for projection -- also testing sort order carryover - the metadata in querytable describing sorted order of its contents
    sortDefinition[0].first = 0;
    sortDefinition[1].first = 1;

    expected->setSortOrder(sortDefinition);

    ASSERT_EQ(*expected, *observed);
    /* Expected:
     *  (#0 float .revenue, #1 int64 .o_orderdate) isEncrypted? 0
(64962.171875, 698457600)
(63833.101562, 856569600)
(52544.890625, 855619200)
(50856.226562, 741657600)
(48688.640625, 878688000)
(36287.386719, 779846400)
(22249.986328, 871689600)
(21736.742188, 898560000)
(15533.496094, 820108800)
(15315.331055, 794102400) */

}


TEST_F(SortTest, tpchQ5Sort) {

    string sql = "SELECT l_orderkey, l.l_extendedprice * (1 - l.l_discount) revenue FROM lineitem l  ORDER BY  l_linenumber, l_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expectedResultSql = "WITH input AS (" + sql + ") SELECT revenue FROM input ORDER BY revenue DESC";
    shared_ptr<PlainTable > expected = DataUtilities::getQueryResults(dbName, expectedResultSql, false);

    SortDefinition sortDefinition;
    sortDefinition.emplace_back(1, SortDirection::DESCENDING);

   Sort<BoolField> sort = getSort(sql, sortDefinition);


    // project it down to $1
    Project project(&sort);
    project.addColumnMapping(1, 0);


    // update sort def to account for projection -- also testing sort order carryover - the metadata in PlainTable  describing sorted order of its contents
    sortDefinition[0].first = 0;
    expected->setSortOrder(sortDefinition);

    shared_ptr<PlainTable > observed = project.run();

    ASSERT_EQ(*expected, *observed);

}



TEST_F(SortTest, tpchQ8Sort) {

    string sql = "SELECT  o_orderyear, o_orderkey FROM orders o  ORDER BY o_comment, o_orderkey LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expectedResultSql = "WITH input AS (" + sql + ") SELECT o_orderyear FROM input ORDER BY o_orderyear, o_orderkey DESC";  // orderkey DESC needed to align with psql's layout
    shared_ptr<PlainTable > expected = DataUtilities::getQueryResults(dbName, expectedResultSql, false);

    SortDefinition sortDefinition;
    sortDefinition.emplace_back(0, SortDirection::ASCENDING);
    expected->setSortOrder(sortDefinition);


   Sort<BoolField> sort = getSort(sql, sortDefinition);


    // project it down to $0
    Project project(&sort);
    project.addColumnMapping(0, 0);
    shared_ptr<PlainTable > observed = project.run();

    ASSERT_EQ(*expected, *observed);
}




TEST_F(SortTest, tpchQ9Sort) {

    string sql = "SELECT o_orderyear, o_orderkey, n_name FROM orders o JOIN lineitem l ON o_orderkey = l_orderkey"
                      "  JOIN supplier s ON s_suppkey = l_suppkey"
                      "  JOIN nation on n_nationkey = s_nationkey"
                      " ORDER BY  l_linenumber, l_comment LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expectedResultSql = "WITH input AS (" + sql + ") SELECT n_name, o_orderyear FROM input ORDER BY n_name, o_orderyear DESC";

    shared_ptr<PlainTable > expected = DataUtilities::getQueryResults(dbName, expectedResultSql, false);



    SortDefinition sortDefinition;
    sortDefinition.emplace_back(2, SortDirection::ASCENDING);
    sortDefinition.emplace_back(0, SortDirection::DESCENDING);

   Sort<BoolField> sort = getSort(sql, sortDefinition);

    // project it down to $0
    Project project(&sort);
    project.addColumnMapping(2, 0);
    project.addColumnMapping(0, 1);

    shared_ptr<PlainTable > observed = project.run();


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


    shared_ptr<PlainTable > expected = DataUtilities::getQueryResults(dbName, expectedResultSql, false);


    SortDefinition sortDefinition;
    sortDefinition.emplace_back(2, SortDirection::DESCENDING);
    sortDefinition.emplace_back(1, SortDirection::ASCENDING);
    expected->setSortOrder(sortDefinition);


   Sort<BoolField> sort = getSort(sql, sortDefinition);

    // project it down to $2, $1
    Project project(&sort);
    project.addColumnMapping(2, 0);
    project.addColumnMapping(1, 1);

    shared_ptr<PlainTable > observed = project.run();


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

