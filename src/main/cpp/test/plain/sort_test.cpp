#include "plain_base_test.h"
#include <operators/sql_input.h>
#include <operators/sort.h>
#include <operators/project.h>


class SortTest : public PlainBaseTest {


protected:

    Sort<bool> getSort(const string & srcSql, const SortDefinition & sortDefinition);
    shared_ptr<PlainTable> sorted_;
};





TEST_F(SortTest, testSingleIntColumn) {

    string sql = "SELECT c_custkey FROM customer ORDER BY c_address, c_custkey LIMIT 10";  // c_address "randomizes" the order
    string expectedSql = "SELECT c_custkey FROM (" + sql + ") subquery ORDER BY c_custkey";


    shared_ptr<PlainTable > expected = DataUtilities::getQueryResults(db_name_, expectedSql, false);

    SortDefinition sortDefinition;
    ColumnSort aColumnSort(0, SortDirection::ASCENDING);
    sortDefinition.push_back(aColumnSort);

    Sort<bool> sort = getSort(sql, sortDefinition);
    shared_ptr<PlainTable > observed = sort.run();

    expected->setSortOrder(observed->getSortOrder());
    ASSERT_EQ(*expected, *observed);

    } 


TEST_F(SortTest, tpchQ1Sort) {
    string sql = "SELECT l_returnflag, l_linestatus FROM lineitem ORDER BY l_comment, l_orderkey  LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expectedResultSql = "WITH input AS (" + sql + ") SELECT * FROM input ORDER BY l_returnflag, l_linestatus";
    shared_ptr<PlainTable > expected = DataUtilities::getQueryResults(db_name_, expectedResultSql, false);


    SortDefinition sortDefinition;
    sortDefinition.emplace_back(0, SortDirection::ASCENDING);
    sortDefinition.emplace_back(1, SortDirection::ASCENDING);
    expected->setSortOrder(sortDefinition);

    Sort<bool> sort = getSort(sql, sortDefinition);
    shared_ptr<PlainTable> observed = sorted_;

    // no projection needed here
    ASSERT_EQ(*expected, *observed);

}


TEST_F(SortTest, tpchQ3Sort) {
    // casting revenue to float for these trials
    // TODO: set up discrete testbed to deal with floating point errors
    string sql = "SELECT l_orderkey, (l.l_extendedprice * (1 - l.l_discount))::INT revenue, o.o_shippriority, o_orderdate FROM lineitem l JOIN orders o ON l_orderkey = o_orderkey ORDER BY  l_comment, l_orderkey LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols

    string expectedResultSql = "WITH input AS (" + sql + ") SELECT revenue, " + DataUtilities::queryDatetime("o_orderdate")  + " FROM input ORDER BY revenue DESC, o_orderdate";
    shared_ptr<PlainTable> expected = DataUtilities::getQueryResults(db_name_, expectedResultSql, false);


    SortDefinition sortDefinition;
    sortDefinition.emplace_back(1, SortDirection::DESCENDING);
    sortDefinition.emplace_back(3, SortDirection::ASCENDING);


    Sort sort = getSort(sql, sortDefinition);

    ExpressionMapBuilder<bool> builder(sort.getOutputSchema());
    builder.addMapping(1, 0);
    builder.addMapping(3, 1);

    // project it down to $1, $3
    Project project(sorted_, builder.getExprs());

    shared_ptr<PlainTable> observed = project.run();

    // update sort def to account for projection -- also testing sort order carryover - the metadata in querytable describing sorted order of its contents
    sortDefinition[0].first = 0;
    sortDefinition[1].first = 1;

    expected->setSortOrder(sortDefinition);

    ASSERT_EQ(*expected, *observed);


}


TEST_F(SortTest, tpchQ5Sort) {

    string sql = "SELECT l_orderkey, l.l_extendedprice * (1 - l.l_discount) revenue FROM lineitem l  ORDER BY  l_comment, l_orderkey LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expectedResultSql = "WITH input AS (" + sql + ") SELECT revenue FROM input ORDER BY revenue DESC";
    shared_ptr<PlainTable > expected = DataUtilities::getQueryResults(db_name_, expectedResultSql, false);

    SortDefinition sortDefinition;
    sortDefinition.emplace_back(1, SortDirection::DESCENDING);

   Sort<bool> sort = getSort(sql, sortDefinition);


    // project it down to $1
    ExpressionMapBuilder<bool> builder(sort.getOutputSchema());
    builder.addMapping(1, 0);

    Project project(sorted_, builder.getExprs());


    // update sort def to account for projection -- also testing sort order carryover - the metadata in PlainTable  describing sorted order of its contents
    sortDefinition[0].first = 0;
    expected->setSortOrder(sortDefinition);

    shared_ptr<PlainTable > observed = project.run();

    ASSERT_EQ(*expected, *observed);

}



TEST_F(SortTest, tpchQ8Sort) {

    string sql = "SELECT  o_orderyear, o_orderkey FROM orders o  ORDER BY o_comment, o_orderkey LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expectedResultSql = "WITH input AS (" + sql + ") SELECT o_orderyear FROM input ORDER BY o_orderyear, o_orderkey DESC";  // orderkey DESC needed to align with psql's layout
    shared_ptr<PlainTable > expected = DataUtilities::getQueryResults(db_name_, expectedResultSql, false);

    SortDefinition sortDefinition;
    sortDefinition.emplace_back(0, SortDirection::ASCENDING);
    sortDefinition.emplace_back(1, SortDirection::DESCENDING);

    expected->setSortOrder(sortDefinition);


   Sort<bool> sort = getSort(sql, sortDefinition);


    // project it down to $0
    ExpressionMapBuilder<bool> builder(sort.getOutputSchema());
    builder.addMapping(0, 0);

    Project project(sorted_, builder.getExprs());
    shared_ptr<PlainTable > observed = project.run();

    ASSERT_EQ(*expected, *observed);
}




TEST_F(SortTest, tpchQ9Sort) {

    string sql = "SELECT o_orderyear, o_orderkey, n_name FROM orders o JOIN lineitem l ON o_orderkey = l_orderkey"
                      "  JOIN supplier s ON s_suppkey = l_suppkey"
                      "  JOIN nation on n_nationkey = s_nationkey"
                      " ORDER BY  l_comment, l_orderkey LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expectedResultSql = "WITH input AS (" + sql + ") SELECT n_name, o_orderyear FROM input ORDER BY n_name, o_orderyear DESC";

    shared_ptr<PlainTable > expected = DataUtilities::getQueryResults(db_name_, expectedResultSql, false);



    SortDefinition sortDefinition;
    sortDefinition.emplace_back(2, SortDirection::ASCENDING);
    sortDefinition.emplace_back(0, SortDirection::DESCENDING);

   Sort<bool> sort = getSort(sql, sortDefinition);

    // project it down to $0
    ExpressionMapBuilder<bool> builder(sort.getOutputSchema());
    builder.addMapping(2, 0);
    builder.addMapping(0, 1);

    Project project(sorted_, builder.getExprs());

    shared_ptr<PlainTable > observed = project.run();


    sortDefinition[0].first = 0;
    sortDefinition[1].first = 1;
    expected->setSortOrder(sortDefinition);

    ASSERT_EQ(*expected, *observed);


}



// 18
TEST_F(SortTest, tpchQ18Sort) {
    // Q3 has the same program logic, but it succeeds.  Why?
    // both have 10 values.
    string sql = "SELECT o_orderkey, o_orderdate, o_totalprice FROM orders"
                      " ORDER BY o_clerk, o_custkey, o_orderkey LIMIT 10"; // order by to ensure order is reproducible and not sorted on the to-sort cols
    string expectedResultSql = "WITH input AS (" + sql + ") "
                    "SELECT o_totalprice, " + DataUtilities::queryDatetime("o_orderdate") + "  FROM input "
                    "ORDER BY o_totalprice DESC, o_orderdate ASC";


    shared_ptr<PlainTable > expected = DataUtilities::getQueryResults(db_name_, expectedResultSql, false);

    // schema: o_orderkey, o_orderdate, o_totalprice
    SortDefinition sortDefinition;
    sortDefinition.emplace_back(2, SortDirection::DESCENDING); // o_totalprince
    sortDefinition.emplace_back(1, SortDirection::ASCENDING); // o_orderdate
    expected->setSortOrder(sortDefinition);


   Sort<bool> sort = getSort(sql, sortDefinition);


    // project it down to $2, $1
    ExpressionMapBuilder<bool> builder(sort.getOutputSchema());
    builder.addMapping(2, 0);
    builder.addMapping(1, 1);


    Project project(sorted_, builder.getExprs());
    
    shared_ptr<PlainTable > observed = project.run();


    sortDefinition[0].first = 0;
    sortDefinition[1].first = 1;
    expected->setSortOrder(sortDefinition);

    ASSERT_EQ(*expected, *observed);


}


TEST_F(SortTest, sort_and_encrypt_table_one_column) {
    vector<int32_t> input_tuples{1, 1, 1, 1, 1, 1, 2, 3, 3, 3, 35, 35, 35, 35, 35, 33, 33, 33, 33, 4};
    SortDefinition sort_definition = DataUtilities::getDefaultSortDefinition(1);
    QuerySchema schema;
    schema.putField(QueryFieldDesc(0, "test", "test_table", FieldType::INT));
    schema.initializeFieldOffsets();


    std::shared_ptr<PlainTable> input_table(new PlainTable(input_tuples.size(), schema, sort_definition));


    for(uint32_t i = 0; i < input_tuples.size(); ++i) {
        Field<bool> val(FieldType::INT, input_tuples[i]);
        PlainTuple tuple = (*input_table)[i];
        tuple.setDummyTag(false);
        tuple.setField(0, val);
    }


    Sort<bool> sorter(input_table, sort_definition);
    std::shared_ptr<PlainTable> sorted_table = sorter.run();


    //set up expected results
    std::sort(input_tuples.begin(), input_tuples.end());
    std::unique_ptr<PlainTable > expected_table(new PlainTable(input_tuples.size(), schema, sort_definition));

    for(uint32_t i = 0; i < input_tuples.size(); ++i) {
        Field<bool> val(FieldType::INT, input_tuples[i]);
        PlainTuple tuple = (*expected_table)[i];
        tuple.setDummyTag(false);
        tuple.setField(0, val);
    }

    ASSERT_EQ(*expected_table, *sorted_table);



}





Sort<bool> SortTest::getSort(const string &srcSql, const SortDefinition &sortDefinition) {
    SqlInput input(db_name_, srcSql, false);

    Sort<bool> sort(&input, sortDefinition); // heap allocate it

              // cache sort result
    sorted_ = sort.run();
    return sort;
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}

