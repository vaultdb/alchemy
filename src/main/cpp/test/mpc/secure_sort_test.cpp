#include <operators/secure_sql_input.h>
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <operators/project.h>
#include <test/mpc/emp_base_test.h>
#include <operators/sort.h>
#include <query_table/secure_tuple.h>




DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54333, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for execution");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(alice_db, "tpch_alice_150", "alice db name");
DEFINE_string(bob_db, "tpch_bob_150", "bob db name");
DEFINE_int32(cutoff, 100, "limit clause for queries");
DEFINE_string(storage, "row", "storage model for tables (row or column)");
DEFINE_int32(ctrl_port, 65475, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");


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

bool SecureSortTest::isSorted(const PlainTable *table, const SortDefinition & sort_definition) {

    for(uint32_t i = 1; i < table->getTupleCount(); ++i) {
        if(!correctOrder(table, i-1, table, i, sort_definition))  {
            std::cout << "Incorrect order: " << table->getPlainTuple(i-1) << " --> " << table->getPlainTuple(i)  << std::endl;
            return false;
        }// each tuple correctly ordered wrt its predecessor

    }
    return true;
}




TEST_F(SecureSortTest, tpchQ01Sort) {

    string sql = "SELECT l_returnflag, l_linestatus FROM lineitem WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + " ORDER BY l_comment"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expected_results_sql = "WITH input AS ("
                                  + sql
                                  + ") SELECT * FROM input ORDER BY l_returnflag, l_linestatus";


    SortDefinition sort_def{
            ColumnSort(0, SortDirection::ASCENDING),
            ColumnSort(1, SortDirection::ASCENDING)
    };

    auto input = new SecureSqlInput(db_name_, sql, false);
    Sort<emp::Bit> sort(input, sort_def);

    auto sorted = sort.run();
    if(FLAGS_validation) {
        auto observed = sorted->reveal();
        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_results_sql, false);
        expected->setSortOrder(sort_def);

        ASSERT_EQ(*expected, *observed);

        delete expected;
        delete observed;

    }


}


TEST_F(SecureSortTest, tpchQ03Sort) {

    string sql = "SELECT l_orderkey, l.l_extendedprice * (1 - l.l_discount) revenue, o.o_shippriority, o_orderdate FROM lineitem l JOIN orders o ON l_orderkey = o_orderkey WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + " ORDER BY l_comment"; // order by to ensure order is reproducible and not sorted on the sort cols

    string expected_result_sql = "WITH input AS (" + sql + ") SELECT revenue, " + DataUtilities::queryDatetime("o_orderdate") + " FROM input ORDER BY revenue DESC, o_orderdate";

    SortDefinition sort_def{ColumnSort (1, SortDirection::DESCENDING), ColumnSort (3, SortDirection::ASCENDING)};

    auto input= new SecureSqlInput(db_name_, sql, false);
    auto sort = new Sort<emp::Bit>(input, sort_def);
    auto sorted = sort->run();

    if(FLAGS_validation) {

        // project it down to $1, $3
        ExpressionMapBuilder<bool> builder(sort->getOutputSchema());
        builder.addMapping(1, 0);
        builder.addMapping(3, 1);

        auto revealed = sorted->reveal();
        Project project(revealed, builder.getExprs());

        PlainTable *observed = project.run();
        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_result_sql,false);

        // copy out the projected sort order
        expected->setSortOrder(observed->getSortOrder());

        ASSERT_EQ(*expected, *observed);

        delete expected;

    }

}


TEST_F(SecureSortTest, tpchQ05Sort) {

    string sql = "SELECT l_orderkey, l.l_extendedprice * (1 - l.l_discount) revenue FROM lineitem l WHERE l_orderkey <= " + std::to_string(FLAGS_cutoff) + "  ORDER BY l_comment"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expected_results_sql = "WITH input AS (" + sql + ") SELECT revenue FROM input ORDER BY revenue DESC";

    SortDefinition sort_definition;
    sort_definition.emplace_back(1, SortDirection::DESCENDING);

    auto input = new SecureSqlInput(db_name_, sql, false);
    auto *sort = new Sort<emp::Bit>(input, sort_definition);
    auto sorted = sort->run();
    
    if(FLAGS_validation) {
        ExpressionMapBuilder<bool> builder(sort->getOutputSchema());
        builder.addMapping(1, 0);

        // project it down to $1
        Project project(sorted->reveal(), builder.getExprs());

        PlainTable *observed  = project.run();

        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_results_sql, false);
        expected->setSortOrder(observed->getSortOrder());


        ASSERT_EQ(*expected, *observed);

        delete expected;


    }


    delete sort;
}



TEST_F(SecureSortTest, tpchQ08Sort) {

    string sql = "SELECT  o_orderyear, o_orderkey FROM orders o  WHERE o_orderkey <= " + std::to_string(FLAGS_cutoff) + " ORDER BY o_comment, o_orderkey"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expected_sql = "WITH input AS (" + sql + ") SELECT * FROM input ORDER BY o_orderyear, o_orderkey DESC";  // orderkey DESC needed to align with psql's layout

    SortDefinition sort_def {ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};


    auto input = new SecureSqlInput(db_name_, sql, false);
    auto sort = new Sort<emp::Bit>(input, sort_def);
    auto sorted = sort->run();
    if(FLAGS_validation) {
        PlainTable *observed  = sorted->reveal();

        PlainTable *expected = DataUtilities::getQueryResults(FLAGS_unioned_db, expected_sql, false);
        expected->setSortOrder(observed->getSortOrder());

        ASSERT_EQ(*expected, *observed);

        delete observed;
        delete expected;

    }

    delete sort;
}


// this test is intentionally set to a large input size to test join scaling
// outside of cutoff
// join scale up has surfaced many bugs in the past, thus having it larger than most.
TEST_F(SecureSortTest, tpchQ09Sort) {

    std::string sql = "SELECT o_orderyear, o_orderkey, n_name FROM orders o JOIN lineitem l ON o_orderkey = l_orderkey"
                      "  JOIN supplier s ON s_suppkey = l_suppkey"
                      "  JOIN nation on n_nationkey = s_nationkey"
                      " ORDER BY l_comment LIMIT 1000"; //  order by to ensure order is reproducible and not sorted on the sort cols



    SortDefinition sort_definition;
    sort_definition.emplace_back(2, SortDirection::ASCENDING);
    sort_definition.emplace_back(0, SortDirection::DESCENDING);


    auto input = new SecureSqlInput(db_name_, sql, false);
    Sort sort(input, sort_definition);
    auto sorted = sort.run();

    if(FLAGS_validation) {
        PlainTable *observed = sorted->reveal();
        ASSERT_TRUE(isSorted(observed, sort_definition));
        delete observed;
    }




}

// 18
TEST_F(SecureSortTest, tpchQ18Sort) {

    string sql = "SELECT o_orderkey, o_orderdate, o_totalprice FROM orders WHERE o_orderkey <= " + std::to_string(FLAGS_cutoff) + " "
                 " ORDER BY o_comment, o_custkey, o_orderkey"; // order by to ensure order is reproducible and not sorted on the sort cols


    SortDefinition sort_definition;
    sort_definition.emplace_back(2, SortDirection::DESCENDING);
    sort_definition.emplace_back(1, SortDirection::ASCENDING);

    auto input = new SecureSqlInput(db_name_, sql, false);
    Sort<Bit> sort(input, sort_definition);
    auto sorted = sort.run();

    if(FLAGS_validation) {
        PlainTable *observed = sorted->reveal();
        ASSERT_TRUE(isSorted(observed, sort_definition));
        delete observed;
    }

}

TEST_F(SecureSortTest, customerSort) {
	string sql = "SELECT * FROM customer WHERE c_custkey <= " + std::to_string(FLAGS_cutoff) + " ORDER BY c_custkey";

	SortDefinition sort_definition;
	sort_definition.emplace_back(0, SortDirection::ASCENDING);

	auto input = new SecureSqlInput(db_name_, sql, false);
	Sort<Bit> sort(input, sort_definition);
	auto sorted = sort.run();
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

	::testing::GTEST_FLAG(filter)=FLAGS_filter;
    return RUN_ALL_TESTS();
}




