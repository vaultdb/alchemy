#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <operators/project.h>
#include <test/zk/zk_base_test.h>
#include <util/field_utilities.h>
#include <operators/sort.h>
#include <query_table/secure_tuple.h>
#include <operators/zk_sql_input.h>




DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43447, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for execution");
DEFINE_bool(input, false, "input value");


using namespace vaultdb;

class ZkSortTest : public ZkTest {
protected:

    static bool correctOrder(const PlainTable *lhs, const int & l_row,
                             const PlainTable *rhs, const int & r_row, const SortDefinition & sort_definition);
    static bool isSorted(const PlainTable *table, const SortDefinition & sort_definition);


};

// is lhs  <= rhs
// fails if either tuple is encrypted
bool ZkSortTest::correctOrder(const PlainTable *lhs, const int & l_row,
                              const PlainTable *rhs, const int & r_row, const SortDefinition & sort_definition) {
    assert(lhs->getSchema() == rhs->getSchema());

    for(int i = 0; i < sort_definition.size(); ++i) {
        PlainField lhs_field = lhs->getField(l_row, sort_definition[i].first);
        PlainField rhs_field = rhs->getField(r_row, sort_definition[i].first);

        if (lhs_field == rhs_field)
            continue;

        if(sort_definition[i].second == SortDirection::ASCENDING) {
            return lhs_field < rhs_field;
        }
        else if(sort_definition[i].second == SortDirection::DESCENDING) {
            return lhs_field >  rhs_field;
        }
    }
    return true; // it's a toss-up, they are equal wrt sort fields
}

bool ZkSortTest::isSorted(const  PlainTable *table, const SortDefinition & sort_definition) {

    for(uint32_t i = 1; i < table->getTupleCount(); ++i) {
        if(!correctOrder(table, i-1, table, i, sort_definition))  {
            std::cout << "Incorrect order: " << table->getPlainTuple(i-1) << " --> " << table->getPlainTuple(i)  << std::endl;
            return false;
        }// each tuple correctly ordered wrt its predecessor

    }
    return true;

}



TEST_F(ZkSortTest, tpchQ1Sort) {

    string sql = "SELECT l_returnflag, l_linestatus FROM lineitem WHERE l_orderkey <= 10 ORDER BY l_comment"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expected_sql = "WITH input AS ("
            + sql
            + ") SELECT * FROM input ORDER BY l_returnflag, l_linestatus";
    PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expected_sql, false);



    SortDefinition sortDefinition;
    sortDefinition.emplace_back(0, SortDirection::ASCENDING);
    sortDefinition.emplace_back(1, SortDirection::ASCENDING);
    expected->setSortOrder(sortDefinition);

    
    auto input = new ZkSqlInput(db_name_, sql, false);
    Sort sort(input, sortDefinition);
    SecureTable *result = sort.run();

    PlainTable *observed = result->reveal();
    ASSERT_EQ(*expected, *observed);


}


TEST_F(ZkSortTest, tpchQ3Sort) {

    string sql = "SELECT l_orderkey, l.l_extendedprice * (1 - l.l_discount) revenue, o.o_shippriority, o_orderdate FROM lineitem l JOIN orders o ON l_orderkey = o_orderkey WHERE l_orderkey <= 10 ORDER BY l_comment"; // order by to ensure order is reproducible and not sorted on the sort cols

    string expected_sql = "WITH input AS (" + sql + ") SELECT revenue, " + DataUtilities::queryDatetime("o_orderdate")  + " FROM input ORDER BY revenue DESC, o_orderdate";
    PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expected_sql, false);

    SortDefinition sortDefinition;
    sortDefinition.emplace_back(1, SortDirection::DESCENDING);
    sortDefinition.emplace_back(3, SortDirection::ASCENDING);

    auto input = new ZkSqlInput(db_name_, sql, false);
    Sort<emp::Bit> sort(input, sortDefinition);


    // project it down to $1, $3
    ExpressionMapBuilder<emp::Bit> builder(sort.getOutputSchema());
    builder.addMapping(1, 0);
    builder.addMapping(3, 1);

    // project it down to $1, $3
    Project project(&sort, builder.getExprs());

    SecureTable * result = project.run();
    PlainTable *observed = result->reveal();

    // copy out the projected sort order
    expected->setSortOrder(observed->getSortOrder());


    ASSERT_EQ(*expected, *observed);

}

TEST_F(ZkSortTest, tpchQ5Sort) {

    string sql = "SELECT l_orderkey, l.l_extendedprice * (1 - l.l_discount) revenue FROM lineitem l WHERE l_orderkey <= 10  ORDER BY l_comment"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expected_sql = "WITH input AS (" + sql + ") SELECT revenue FROM input ORDER BY revenue DESC";
    PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expected_sql, false);

    SortDefinition sortDefinition;
    sortDefinition.emplace_back(1, SortDirection::DESCENDING);

    auto input = new ZkSqlInput(db_name_, sql, false);
    Sort<emp::Bit> sort(input, sortDefinition);


    // project it down to $1
    ExpressionMapBuilder<emp::Bit> builder(sort.getOutputSchema());
    builder.addMapping(1, 0);

    // project it down to $1
    Project project(&sort, builder.getExprs());

    SecureTable * result = project.run();
    PlainTable *observed  = result->reveal();

    // copy out the projected sort order
    expected->setSortOrder(observed->getSortOrder());

    ASSERT_EQ(*expected, *observed);



}



TEST_F(ZkSortTest, tpchQ8Sort) {

    string sql = "SELECT  o_orderyear, o_orderkey FROM orders o  WHERE o_orderkey <= 10 ORDER BY o_comment, o_orderkey"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expected_sql = "WITH input AS (" + sql + ") SELECT o_orderyear FROM input ORDER BY o_orderyear, o_orderkey DESC";  // orderkey DESC needed to align with psql's layout
    PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expected_sql, false);

    SortDefinition sortDefinition;
    sortDefinition.emplace_back(0, SortDirection::ASCENDING);


    auto input = new ZkSqlInput(db_name_, sql, false);
    Sort<emp::Bit> sort(input, sortDefinition);

    ExpressionMapBuilder<emp::Bit> builder(sort.getOutputSchema());
    builder.addMapping(0, 0);

    Project project(&sort, builder.getExprs());

    SecureTable * result = project.run();
    PlainTable *observed  = result->reveal();

    // copy out the projected sort order
    expected->setSortOrder(observed->getSortOrder());


    ASSERT_EQ(*expected, *observed);


}



TEST_F(ZkSortTest, tpchQ9Sort) {

    std::string sql = "SELECT o_orderyear, o_orderkey, n_name FROM orders o JOIN lineitem l ON o_orderkey = l_orderkey"
                      "  JOIN supplier s ON s_suppkey = l_suppkey"
                      "  JOIN nation on n_nationkey = s_nationkey"
                      " ORDER BY l_comment LIMIT 1000"; //  order by to ensure order is reproducible and not sorted on the sort cols



    SortDefinition sortDefinition;
    sortDefinition.emplace_back(2, SortDirection::ASCENDING);
    sortDefinition.emplace_back(0, SortDirection::DESCENDING);


    auto input = new ZkSqlInput(db_name_, sql, false);
    Sort sort(input, sortDefinition);

// project it down to $2, $0
    ExpressionMapBuilder<emp::Bit> builder(sort.getOutputSchema());
    builder.addMapping(2, 0);
    builder.addMapping(0, 1);

    Project project(&sort, builder.getExprs());

    SecureTable * result = project.run();
    PlainTable *observed  = result->reveal();

    ASSERT_TRUE(isSorted(observed, sortDefinition));



}

// 18
TEST_F(ZkSortTest, tpchQ18Sort) {

    string sql = "SELECT o_orderkey, o_orderdate, o_totalprice FROM orders WHERE o_orderkey <= 10 "
                 " ORDER BY o_comment, o_custkey"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expected_sql = "WITH input AS (" + sql + ") SELECT o_totalprice, " + DataUtilities::queryDatetime("o_orderdate") + "  FROM input ORDER BY o_totalprice DESC, o_orderdate";


    PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expected_sql, false);

    SortDefinition sortDefinition;
    sortDefinition.emplace_back(2, SortDirection::DESCENDING);
    sortDefinition.emplace_back(1, SortDirection::ASCENDING);

    auto input = new ZkSqlInput(db_name_, sql, false);
    Sort<emp::Bit> sort(input, sortDefinition);


    // project it down to $2, $1
    ExpressionMapBuilder<emp::Bit> builder(sort.getOutputSchema());
    builder.addMapping(2, 0);
    builder.addMapping(1, 1);

    Project project(&sort, builder.getExprs());


    SecureTable * result = project.run();
    PlainTable *observed  = result->reveal();

    // copy out the projected sort order
    expected->setSortOrder(observed->getSortOrder());


    // verify that first col is DESC, second is ASC
    ASSERT_EQ(*expected, *observed);


}






int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}





