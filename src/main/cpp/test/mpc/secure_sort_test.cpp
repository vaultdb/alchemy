#include <operators/secure_sql_input.h>
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <operators/project.h>
#include <test/mpc/emp_base_test.h>
#include <operators/sort.h>
#include <query_table/secure_tuple.h>




DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43441, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for execution");
DEFINE_bool(input, false, "input value");


using namespace vaultdb;

class SecureSortTest : public EmpBaseTest {
protected:

    static bool correctOrder(const PlainTable *lhs, const int & l_row,
                             const PlainTable *rhs, const int & r_row, const SortDefinition & sort_definition);
    static bool isSorted(const PlainTable *table, const SortDefinition & sort_definition);


};

// is lhs  <= rhs
// fails if either tuple is encrypted
bool SecureSortTest::correctOrder(const PlainTable *lhs, const int & l_row,
                                  const PlainTable *rhs, const int & r_row, const SortDefinition & sort_definition) {

    assert(*lhs->getSchema() == *rhs->getSchema());

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

bool SecureSortTest::isSorted(const PlainTable *table, const SortDefinition & sort_definition) {

    for(uint32_t i = 1; i < table->getTupleCount(); ++i) {


        if(!correctOrder(table, i-1, table, i, sort_definition))  {
            std::cout << "Incorrect order: " << table->getPlainTuple(i-1) << " --> " << table->getPlainTuple(i)  << std::endl;
            return false;
        }// each tuple correctly ordered wrt its predecessor

    }
    return true;
}




TEST_F(SecureSortTest, tpchQ1Sort) {
    std::string db_name_ =  FLAGS_party == 1 ? alice_db_ : bob_db_;

    string sql = "SELECT l_returnflag, l_linestatus FROM lineitem WHERE l_orderkey <= 10 ORDER BY l_comment"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expected_results_sql = "WITH input AS ("
                                  + sql
                                  + ") SELECT * FROM input ORDER BY l_returnflag, l_linestatus";
    shared_ptr<PlainTable > expected = DataUtilities::getQueryResults(unioned_db_, expected_results_sql, false);



    PsqlDataProvider provider;

    std::shared_ptr<PlainTable>  input_table = provider.getQueryTable(db_name_,
                                                                      sql, false);

    SortDefinition sortDefinition;
    sortDefinition.emplace_back(0, SortDirection::ASCENDING);
    sortDefinition.emplace_back(1, SortDirection::ASCENDING);
    expected->setSortOrder(sortDefinition);



    SecureSqlInput input(db_name_, sql, false, netio_, FLAGS_party);
    Sort<emp::Bit> sort(&input, sortDefinition);
    std::shared_ptr<SecureTable> result = sort.run();

    std::shared_ptr<PlainTable> observed = result->reveal();
    ASSERT_EQ(*expected, *observed);



}


TEST_F(SecureSortTest, tpchQ3Sort) {
    std::string db_name_ =  FLAGS_party == 1 ? alice_db_ : bob_db_;

    string sql = "SELECT l_orderkey, l.l_extendedprice * (1 - l.l_discount) revenue, o.o_shippriority, o_orderdate FROM lineitem l JOIN orders o ON l_orderkey = o_orderkey WHERE l_orderkey <= 10 ORDER BY l_comment"; // order by to ensure order is reproducible and not sorted on the sort cols

    string expected_result_sql = "WITH input AS (" + sql + ") SELECT revenue, " + DataUtilities::queryDatetime("o_orderdate") + " FROM input ORDER BY revenue DESC, o_orderdate";
    shared_ptr<PlainTable> expected = DataUtilities::getQueryResults(unioned_db_, expected_result_sql, false);

    SortDefinition sortDefinition{ColumnSort (1, SortDirection::DESCENDING), ColumnSort (3, SortDirection::ASCENDING)};


    SecureSqlInput input(db_name_, sql, false, netio_, FLAGS_party);
    Sort<emp::Bit> sort(&input, sortDefinition);


    // project it down to $1, $3
    ExpressionMapBuilder<emp::Bit> builder(sort.getOutputSchema());
    builder.addMapping(1, 0);
    builder.addMapping(3, 1);

    Project project(&sort, builder.getExprs());


    std::shared_ptr<SecureTable> result = project.run();
    std::shared_ptr<PlainTable> observed = result->reveal();

    // copy out the projected sort order
    expected->setSortOrder(observed->getSortOrder());


    ASSERT_EQ(*expected, *observed);


}


TEST_F(SecureSortTest, tpchQ5Sort) {

    string sql = "SELECT l_orderkey, l.l_extendedprice * (1 - l.l_discount) revenue FROM lineitem l WHERE l_orderkey <= 10  ORDER BY l_comment"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expected_results_sql = "WITH input AS (" + sql + ") SELECT revenue FROM input ORDER BY revenue DESC";
    shared_ptr<PlainTable > expected = DataUtilities::getQueryResults(unioned_db_, expected_results_sql, false);

    SortDefinition sort_definition;
    sort_definition.emplace_back(1, SortDirection::DESCENDING);

    SecureSqlInput input(db_name_, sql, false, netio_, FLAGS_party);
    Sort<emp::Bit> sort(&input, sort_definition);

    ExpressionMapBuilder<emp::Bit> builder(sort.getOutputSchema());
    builder.addMapping(1, 0);

    // project it down to $1
    Project project(&sort, builder.getExprs());

    std::shared_ptr<SecureTable> result = project.run();
    std::shared_ptr<PlainTable> observed  = result->reveal();

    // copy out the projected sort order
    expected->setSortOrder(observed->getSortOrder());


        ASSERT_EQ(*expected, *observed);



}



TEST_F(SecureSortTest, tpchQ8Sort) {
    std::string db_name_ =  FLAGS_party == 1 ? alice_db_ : bob_db_;

    string sql = "SELECT  o_orderyear, o_orderkey FROM orders o  WHERE o_orderkey <= 10 ORDER BY o_comment, o_orderkey"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expectedResultSql = "WITH input AS (" + sql + ") SELECT * FROM input ORDER BY o_orderyear, o_orderkey DESC";  // orderkey DESC needed to align with psql's layout
    shared_ptr<PlainTable > expected = DataUtilities::getQueryResults(unioned_db_, expectedResultSql, false);

    SortDefinition sortDefinition;
    sortDefinition.emplace_back(0, SortDirection::ASCENDING);
    sortDefinition.emplace_back(1, SortDirection::DESCENDING);


    SecureSqlInput input(db_name_, sql, false, netio_, FLAGS_party);
    Sort<emp::Bit> sort(&input, sortDefinition);

    ExpressionMapBuilder<emp::Bit> builder(sort.getOutputSchema());
    builder.addMapping(0, 0);

    std::shared_ptr<PlainTable> observed  = sort.run()->reveal();

    // copy out the projected sort order
    expected->setSortOrder(observed->getSortOrder());

        ASSERT_EQ(*expected, *observed);

}



TEST_F(SecureSortTest, tpchQ9Sort) {
    std::string db_name_ =  FLAGS_party == 1 ? alice_db_ : bob_db_;

    std::string sql = "SELECT o_orderyear, o_orderkey, n_name FROM orders o JOIN lineitem l ON o_orderkey = l_orderkey"
                      "  JOIN supplier s ON s_suppkey = l_suppkey"
                      "  JOIN nation on n_nationkey = s_nationkey"
                      " ORDER BY l_comment LIMIT 1000"; //  order by to ensure order is reproducible and not sorted on the sort cols



    SortDefinition sort_definition;
    sort_definition.emplace_back(2, SortDirection::ASCENDING);
    sort_definition.emplace_back(0, SortDirection::DESCENDING);


    SecureSqlInput input(db_name_, sql, false, netio_, FLAGS_party);
    Sort sort(&input, sort_definition);


    std::shared_ptr<PlainTable> observed = sort.run()->reveal();


    ASSERT_TRUE(isSorted(observed.get(), sort_definition));



}

// 18
TEST_F(SecureSortTest, tpchQ18Sort) {
    std::string db_name_ =  FLAGS_party == 1 ? alice_db_ : bob_db_;

    string sql = "SELECT o_orderkey, o_orderdate, o_totalprice FROM orders WHERE o_orderkey <= 10 "
                 " ORDER BY o_comment, o_custkey, o_orderkey"; // order by to ensure order is reproducible and not sorted on the sort cols


    SortDefinition sort_definition;
    sort_definition.emplace_back(2, SortDirection::DESCENDING);
    sort_definition.emplace_back(1, SortDirection::ASCENDING);

    SecureSqlInput input(db_name_, sql, false, netio_, FLAGS_party);
    Sort<emp::Bit> sort(&input, sort_definition);

    std::shared_ptr<PlainTable> observed = sort.run()->reveal();

    ASSERT_TRUE(isSorted(observed.get(), sort_definition));


}






int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}





// Alt Q9:
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
    shared_ptr<PlainTable > expected = DataUtilities::getQueryResults(unioned_db_, expectedResultSql, false);


    std::string lineitem_sql_ = "SELECT o_orderyear, l_orderkey, l_suppkey "
                                                                "  FROM orders o JOIN lineitem l ON o_orderkey = l_orderkey"
                                                                "  WHERE l_orderkey <= 10 "
                                                                " ORDER BY l_comment ";

    std::string supplierSql = "SELECT s_suppkey, n_name "
                         "  FROM  supplier JOIN nation on n_nationkey = s_nationkey"
                         " WHERE s_suppkey <= 5000 "
                         " ORDER BY s_suppkey";

    std::cout << " supplierSql: " << supplierSql << std::endl;



    SecureSqlInput lineitemInput(db_name_, lineitem_sql_, false, netio, FLAGS_party);
    SecureSqlInput supplierInput(db_name_, supplierSql, false, netio, FLAGS_party);


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
