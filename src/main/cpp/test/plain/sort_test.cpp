#include "plain_base_test.h"
#include <operators/sql_input.h>
#include <operators/sort.h>
#include <operators/project.h>
#include "query_table/table_factory.h"

class SortTest : public PlainBaseTest {};





TEST_F(SortTest, testSingleIntColumn) {

    string sql = "SELECT c_custkey FROM customer ORDER BY c_address, c_custkey LIMIT 10";  // c_address "randomizes" the order
    string expected_sql = "SELECT c_custkey FROM (" + sql + ") subquery ORDER BY c_custkey";


    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, false);

    SortDefinition sort_def{ ColumnSort(0, SortDirection::ASCENDING)};
    expected->setSortOrder(sort_def);

    auto input = new SqlInput(db_name_, sql, false);
    Sort<bool> sort(input, sort_def);
    PlainTable *sorted = sort.run();
    ASSERT_EQ(*expected, *sorted);
    delete expected;

    } 


TEST_F(SortTest, tpchQ1Sort) {
    string sql = "SELECT l_returnflag, l_linestatus FROM lineitem ORDER BY l_comment, l_orderkey  LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expected_sql = "WITH input AS (" + sql + ") SELECT * FROM input ORDER BY l_returnflag, l_linestatus";
    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, false);


    SortDefinition sort_def{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::ASCENDING)};
    expected->setSortOrder(sort_def);

    auto input = new SqlInput(db_name_, sql, false);
    Sort<bool> sort(input, sort_def);
    PlainTable *sorted = sort.run();


    // no projection needed here
    ASSERT_EQ(*expected, *sorted);

    delete expected;

}


TEST_F(SortTest, tpchQ3Sort) {
    // casting revenue to float for these trials
    // TODO: set up discrete testbed to deal with floating point errors
    string sql = "SELECT l_orderkey, (l.l_extendedprice * (1 - l.l_discount))::INT revenue, o.o_shippriority, o_orderdate FROM lineitem l JOIN orders o ON l_orderkey = o_orderkey ORDER BY  l_comment, l_orderkey LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols

    string expected_sql = "WITH input AS (" + sql + ") SELECT revenue, " + DataUtilities::queryDatetime("o_orderdate")  + " FROM input ORDER BY revenue DESC, o_orderdate";
    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, false);


    SortDefinition sort_def{ColumnSort(1, SortDirection::DESCENDING), ColumnSort(3, SortDirection::ASCENDING)};


    auto input = new SqlInput(db_name_, sql, false);
    auto sort = new Sort<bool>(input, sort_def);


    ExpressionMapBuilder<bool> builder(sort->getOutputSchema());
    builder.addMapping(1, 0);
    builder.addMapping(3, 1);

    // project it down to $1, $3
    Project project(sort, builder.getExprs());

    PlainTable *observed = project.run();

    // update sort def to account for projection -- also testing sort order carryover - the metadata in querytable describing sorted order of its contents
    sort_def[0].first = 0;
    sort_def[1].first = 1;

    expected->setSortOrder(sort_def);

    ASSERT_EQ(*expected, *observed);

    delete expected;
}


TEST_F(SortTest, tpchQ5Sort) {

    string sql = "SELECT l_orderkey, l.l_extendedprice * (1 - l.l_discount) revenue FROM lineitem l  ORDER BY  l_comment, l_orderkey LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expected_sql = "WITH input AS (" + sql + ") SELECT revenue FROM input ORDER BY revenue DESC";
    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, false);

    SortDefinition sort_def{ ColumnSort(1, SortDirection::DESCENDING)};

    auto input = new SqlInput(db_name_, sql, false);
    auto sorter = new Sort<bool>(input, sort_def);

    // project it down to $1
    ExpressionMapBuilder<bool> builder(sorter->getOutputSchema());
    builder.addMapping(1, 0);

    Project project(sorter, builder.getExprs());


    // update sort def to account for projection -- also testing sort order carryover - the metadata in PlainTable  describing sorted order of its contents
    sort_def[0].first = 0;
    expected->setSortOrder(sort_def);

    PlainTable *observed = project.run();

    ASSERT_EQ(*expected, *observed);

    delete expected;

}



TEST_F(SortTest, tpchQ8Sort) {

    string sql = "SELECT  o_orderyear, o_orderkey FROM orders o  ORDER BY o_comment, o_orderkey LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expected_sql = "WITH input AS (" + sql + ") SELECT o_orderyear FROM input ORDER BY o_orderyear, o_orderkey DESC";  // orderkey DESC needed to align with psql's layout
    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, false);

    SortDefinition sort_def{ ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};

    expected->setSortOrder(SortDefinition{ColumnSort (0, SortDirection::ASCENDING)});


    auto input = new SqlInput(db_name_, sql, false);
    auto sort = new Sort<bool>(input, sort_def);

    // project it down to $0
    ExpressionMapBuilder<bool> builder(sort->getOutputSchema());
    builder.addMapping(0, 0);

    Project project(sort, builder.getExprs());
    PlainTable *observed = project.run();

    ASSERT_EQ(*expected, *observed);
    delete expected;
}




TEST_F(SortTest, tpchQ9Sort) {

    string sql = "SELECT o_orderyear, o_orderkey, n_name FROM orders o JOIN lineitem l ON o_orderkey = l_orderkey"
                      "  JOIN supplier s ON s_suppkey = l_suppkey"
                      "  JOIN nation on n_nationkey = s_nationkey"
                      " ORDER BY  l_comment, l_orderkey LIMIT 10"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expected_sql = "WITH input AS (" + sql + ") SELECT n_name, o_orderyear FROM input ORDER BY n_name, o_orderyear DESC";

    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, false);



    SortDefinition sort_def{ColumnSort(2, SortDirection::ASCENDING), ColumnSort(0, SortDirection::DESCENDING)};

    auto input = new SqlInput(db_name_, sql, false);
    auto sort = new Sort<bool>(input, sort_def);

    // project it down to $0
    ExpressionMapBuilder<bool> builder(sort->getOutputSchema());
    builder.addMapping(2, 0);
    builder.addMapping(0, 1);

    Project project(sort, builder.getExprs());

    PlainTable *observed = project.run();


    sort_def[0].first = 0;
    sort_def[1].first = 1;
    expected->setSortOrder(sort_def);

    ASSERT_EQ(*expected, *observed);
    delete expected;


}



// 18
TEST_F(SortTest, tpchQ18Sort) {
    // Q3 has the same program logic, but it succeeds.  Why?
    // both have 10 values.
    string sql = "SELECT o_orderkey, o_orderdate, o_totalprice FROM orders"
                      " ORDER BY o_clerk, o_custkey, o_orderkey LIMIT 10"; // order by to ensure order is reproducible and not sorted on the to-sort cols
    string expected_sql = "WITH input AS (" + sql + ") "
                    "SELECT o_totalprice, " + DataUtilities::queryDatetime("o_orderdate") + "  FROM input "
                    "ORDER BY o_totalprice DESC, o_orderdate ASC";


    PlainTable *expected = DataUtilities::getQueryResults(db_name_, expected_sql, false);

    // schema: o_orderkey, o_orderdate, o_totalprice
    SortDefinition sort_def{ColumnSort(2, SortDirection::DESCENDING),  // o_totalprince
        ColumnSort(1, SortDirection::ASCENDING)}; // o_orderdate
    expected->setSortOrder(sort_def);


    auto input = new SqlInput(db_name_, sql, false);
    auto sort = new Sort<bool>(input, sort_def);


    // project it down to $2, $1
    ExpressionMapBuilder<bool> builder(sort->getOutputSchema());
    builder.addMapping(2, 0);
    builder.addMapping(1, 1);


    Project project(sort, builder.getExprs());
    
    PlainTable *observed = project.run();


    sort_def[0].first = 0;
    sort_def[1].first = 1;
    expected->setSortOrder(sort_def);

    ASSERT_EQ(*expected, *observed);

    delete expected;

}


TEST_F(SortTest, sort_and_encrypt_table_one_column) {
    vector<int32_t> input_tuples{1, 1, 1, 1, 1, 1, 2, 3, 3, 3, 35, 35, 35, 35, 35, 33, 33, 33, 33, 4};
    SortDefinition sort_definition = DataUtilities::getDefaultSortDefinition(1);
    QuerySchema schema;
    schema.putField(QueryFieldDesc(0, "test", "test_table", FieldType::INT));
    schema.initializeFieldOffsets();


    PlainTable *input_table = TableFactory<bool>::getTable(input_tuples.size(), schema, storage_model_, sort_definition);


    for(uint32_t i = 0; i < input_tuples.size(); ++i) {
        Field<bool> val(FieldType::INT, input_tuples[i]);
        input_table->setField(i, 0, val);
        input_table->setDummyTag(i, false);
    }


    Sort<bool> sorter(input_table, sort_definition);
    PlainTable *sorted_table = sorter.run();


    //set up expected results
    std::sort(input_tuples.begin(), input_tuples.end());
    auto expected_table = TableFactory<bool>::getTable(input_tuples.size(), schema, storage_model_, sort_definition);

    for(uint32_t i = 0; i < input_tuples.size(); ++i) {
        Field<bool> val(FieldType::INT, input_tuples[i]);
        expected_table->setField(i, 0, val);
        expected_table->setDummyTag(i, false);
    }

    ASSERT_EQ(*expected_table, *sorted_table);


    delete expected_table;

}






int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}

