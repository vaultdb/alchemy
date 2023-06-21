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
DEFINE_string(storage, "row", "storage model for tables (row or column)");
//DEFINE_bool(input, false, "input value");


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
    std::string db_name_ =  FLAGS_party == 1 ? alice_db_ : bob_db_;

    string sql = "SELECT l_returnflag, l_linestatus FROM lineitem WHERE l_orderkey <= 10 ORDER BY l_comment"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expected_results_sql = "WITH input AS ("
                                  + sql
                                  + ") SELECT * FROM input ORDER BY l_returnflag, l_linestatus";
    PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expected_results_sql, false);


    SortDefinition sort_def{
            ColumnSort(0, SortDirection::ASCENDING),
            ColumnSort(1, SortDirection::ASCENDING)
    };
    expected->setSortOrder(sort_def);

    auto input = new SecureSqlInput(db_name_, sql, false);
    Sort<emp::Bit> sort(input, sort_def);

    PlainTable *observed = sort.run()->reveal();
    ASSERT_EQ(*expected, *observed);


    delete expected;
    delete observed;

}

//
//TEST_F(SecureSortTest, tpch_orders_mockup) {
//    string sql = "SELECT o_orderkey, o_custkey, o_orderstatus, o_totalprice, o_orderdate, o_orderpriority, o_clerk, o_shippriority, o_comment, o_orderyear "
//                 "FROM orders "
//                 "WHERE o_orderkey <= 10 ORDER BY o_orderkey";
//
//    SecureSqlInput input(db_name_, sql, false);
//    SecureTable *secret_shared = input.run();
//    cout << "Output schema: " << secret_shared->getSchema() << " schema width=" << secret_shared->getSchema().size() << endl;
//    cout << "Output rows: " << secret_shared->getTupleCount() << endl;
//}
//
//
//TEST_F(SecureSortTest, tpch_customer_mockup) {
//    string sql = "SELECT * "
//                 "FROM customer "
//                 "WHERE c_custkey <= 10";
//
//    SecureSqlInput input(db_name_, sql, false);
//    SecureTable *secret_shared = input.run();
//    cout << "Output schema: " << secret_shared->getSchema() << " schema width=" << secret_shared->getSchema().size() << endl;
//    cout << "Output rows: " << secret_shared->getTupleCount() << endl;
//}
//
//
//TEST_F(SecureSortTest, tpch_partsupp_mockup) {
//    string sql = "SELECT * "
//                 "FROM partsupp "
//                 "WHERE ps_partkey <= 10";
//
//    SecureSqlInput input(db_name_, sql, false);
//    SecureTable *secret_shared = input.run();
//    cout << "Output schema: " << secret_shared->getSchema() << " schema width=" << secret_shared->getSchema().size() << endl;
//    cout << "Output rows: " << secret_shared->getTupleCount() << endl;
//}
//
//
//TEST_F(SecureSortTest, tpch_supplier_mockup) {
//    string sql = "SELECT * "
//                 "FROM supplier "
//                 "WHERE s_suppkey <= 10";
//
//    SecureSqlInput input(db_name_, sql, false);
//    SecureTable *secret_shared = input.run();
//    cout << "Output schema: " << secret_shared->getSchema() << " schema width=" << secret_shared->getSchema().size() << endl;
//    cout << "Output rows: " << secret_shared->getTupleCount() << endl;
//}
//
//TEST_F(SecureSortTest, tpch_part_mockup) {
//    string sql = "SELECT * "
//                 "FROM part "
//                 "WHERE p_partkey <= 10";
//
//    SecureSqlInput input(db_name_, sql, false);
//    SecureTable *secret_shared = input.run();
//    cout << "Output schema: " << secret_shared->getSchema() << " schema width=" << secret_shared->getSchema().size() << endl;
//    cout << "Output rows: " << secret_shared->getTupleCount() << endl;
//}
//
//TEST_F(SecureSortTest, tpch_nation_mockup) {
//    string sql = "SELECT * "
//                 "FROM nation ";
//    SecureSqlInput input(db_name_, sql, false);
//    SecureTable *secret_shared = input.run();
//    cout << "Output schema: " << secret_shared->getSchema() << " schema width=" << secret_shared->getSchema().size() << endl;
//    cout << "Output rows: " << secret_shared->getTupleCount() << endl;
//}
//
//TEST_F(SecureSortTest, tpch_region_mockup) {
//    string sql = "SELECT * "
//                 "FROM region ";
//    SecureSqlInput input(db_name_, sql, false);
//    SecureTable *secret_shared = input.run();
//    cout << "Output schema: " << secret_shared->getSchema() << " schema width=" << secret_shared->getSchema().size() << endl;
//    cout << "Output rows: " << secret_shared->getTupleCount() << endl;
//}
//
//
//
//
//
//TEST_F(SecureSortTest, tpchQ1SortMockup) {
//    this->disableBitPacking();
//    string sql = "SELECT  l_orderkey, l_partkey, l_suppkey, l_linenumber, l_quantity, l_extendedprice, l_discount, l_tax, l_returnflag, l_linestatus, l_shipdate, l_commitdate, l_receiptdate, l_shipinstruct, l_shipmode, l_comment,\n"
//                 "  l_extendedprice * (1 - l_discount) * (1 + l_tax) charge, l_extendedprice * (1 - l_discount) disc_price FROM lineitem  WHERE l_orderkey <= 10 "; // order by to ensure order is reproducible and not sorted on the sort cols
//
//    SortDefinition sort_def{ColumnSort(8, SortDirection::ASCENDING), ColumnSort(9, SortDirection::ASCENDING)};
//
//    auto input = new SecureSqlInput(db_name_, sql, false);
//    Sort sort(input, sort_def);
//    PlainTable *sorted = sort.run()->reveal();
//
//    cout << "Sort output schema: " << sorted->getSchema() << "size=" << sorted->getSchema().size() << endl;
//
//    cout << "Sort rows: " << sort.getOutput()->getTupleCount() << endl;
//
//    delete sorted;
//}
//
//TEST_F(SecureSortTest, tpchQ1SortMockupOutput) {
//    this->disableBitPacking();
//    string sql = "SELECT  l_returnflag,  l_linestatus,  SUM(l_quantity) as sum_qty, SUM(l_extendedprice) as sum_base_price, SUM(l_extendedprice * (1 - l_discount)) as sum_disc_price,  SUM(l_extendedprice * (1 - l_discount) * (1 + l_tax)) as sum_charge, AVG(l_quantity) as avg_qty,  AVG(l_extendedprice) as avg_price,  AVG(l_discount) as avg_disc,   COUNT(*)::BIGINT as count_order \n"
//                 "FROM  lineitem \n"
//                 "WHERE  l_shipdate <= date '1998-08-03' \n"
//                 "GROUP BY  l_returnflag, l_linestatus \n"
//                 "ORDER BY  l_returnflag,  l_linestatus";
//
////    SortDefinition sort_def{ColumnSort(8, SortDirection::ASCENDING), ColumnSort(9, SortDirection::ASCENDING)};
//
//    auto input = new SecureSqlInput(db_name_, sql, false);
//
//    cout << "Schema: " << input->getOutputSchema() << ", size=" << input->getOutputSchema().size() << endl;
////    Sort sort(input, sort_def);
////    PlainTable *sorted = sort.run()->reveal();
//
////    cout << "Sort output schema: " << sorted->getSchema() << "size=" << sorted->getSchema().size() << endl;
//
//
//    delete input;
//}


TEST_F(SecureSortTest, tpchQ03Sort) {
    std::string db_name_ =  FLAGS_party == 1 ? alice_db_ : bob_db_;

    string sql = "SELECT l_orderkey, l.l_extendedprice * (1 - l.l_discount) revenue, o.o_shippriority, o_orderdate FROM lineitem l JOIN orders o ON l_orderkey = o_orderkey WHERE l_orderkey <= 10 ORDER BY l_comment"; // order by to ensure order is reproducible and not sorted on the sort cols

    string expected_result_sql = "WITH input AS (" + sql + ") SELECT revenue, " + DataUtilities::queryDatetime("o_orderdate") + " FROM input ORDER BY revenue DESC, o_orderdate";
    PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expected_result_sql,false);

    SortDefinition sortDefinition{ColumnSort (1, SortDirection::DESCENDING), ColumnSort (3, SortDirection::ASCENDING)};


    auto input= new SecureSqlInput(db_name_, sql, false);
    auto sort = new Sort<emp::Bit>(input, sortDefinition);


    // project it down to $1, $3
    ExpressionMapBuilder<emp::Bit> builder(sort->getOutputSchema());
    builder.addMapping(1, 0);
    builder.addMapping(3, 1);

    Project project(sort, builder.getExprs());


    PlainTable *observed = project.run()->reveal();

    // copy out the projected sort order
    expected->setSortOrder(observed->getSortOrder());


    ASSERT_EQ(*expected, *observed);

    delete expected;
    delete observed;

}


TEST_F(SecureSortTest, tpchQ05Sort) {

    string sql = "SELECT l_orderkey, l.l_extendedprice * (1 - l.l_discount) revenue FROM lineitem l WHERE l_orderkey <= 10  ORDER BY l_comment"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expected_results_sql = "WITH input AS (" + sql + ") SELECT revenue FROM input ORDER BY revenue DESC";
    PlainTable *expected = DataUtilities::getQueryResults(unioned_db_, expected_results_sql, false);

    SortDefinition sort_definition;
    sort_definition.emplace_back(1, SortDirection::DESCENDING);

    auto input = new SecureSqlInput(db_name_, sql, false);
    auto *sort = new Sort<emp::Bit>(input, sort_definition);

    ExpressionMapBuilder<emp::Bit> builder(sort->getOutputSchema());
    builder.addMapping(1, 0);

    // project it down to $1
    Project project(sort, builder.getExprs());

    PlainTable *observed  = project.run()->reveal();

    // copy out the projected sort order
    expected->setSortOrder(observed->getSortOrder());


    ASSERT_EQ(*expected, *observed);

    delete expected;
    delete observed;



}



TEST_F(SecureSortTest, tpchQ08Sort) {
    std::string db_name_ =  FLAGS_party == 1 ? alice_db_ : bob_db_;

    string sql = "SELECT  o_orderyear, o_orderkey FROM orders o  WHERE o_orderkey <= 10 ORDER BY o_comment, o_orderkey"; // order by to ensure order is reproducible and not sorted on the sort cols
    string expectedResultSql = "WITH input AS (" + sql + ") SELECT * FROM input ORDER BY o_orderyear, o_orderkey DESC";  // orderkey DESC needed to align with psql's layout
    PlainTable * expected = DataUtilities::getQueryResults(unioned_db_, expectedResultSql, false);

    SortDefinition sortDefinition {ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};


    auto input = new SecureSqlInput(db_name_, sql, false);
    auto sort = new Sort<emp::Bit>(input, sortDefinition);

    PlainTable *observed  = sort->run()->reveal();

    // copy out the projected sort order
    expected->setSortOrder(observed->getSortOrder());

        ASSERT_EQ(*expected, *observed);
        delete sort;
        delete observed;
        delete expected;
}



TEST_F(SecureSortTest, tpchQ09Sort) {
    std::string db_name_ =  FLAGS_party == 1 ? alice_db_ : bob_db_;

    std::string sql = "SELECT o_orderyear, o_orderkey, n_name FROM orders o JOIN lineitem l ON o_orderkey = l_orderkey"
                      "  JOIN supplier s ON s_suppkey = l_suppkey"
                      "  JOIN nation on n_nationkey = s_nationkey"
                      " ORDER BY l_comment LIMIT 1000"; //  order by to ensure order is reproducible and not sorted on the sort cols



    SortDefinition sort_definition;
    sort_definition.emplace_back(2, SortDirection::ASCENDING);
    sort_definition.emplace_back(0, SortDirection::DESCENDING);


    auto input = new SecureSqlInput(db_name_, sql, false);
    Sort sort(input, sort_definition);


    PlainTable *observed = sort.run()->reveal();


    ASSERT_TRUE(isSorted(observed, sort_definition));

    delete observed;



}

// 18
TEST_F(SecureSortTest, tpchQ18Sort) {
    std::string db_name_ =  FLAGS_party == 1 ? alice_db_ : bob_db_;

    string sql = "SELECT o_orderkey, o_orderdate, o_totalprice FROM orders WHERE o_orderkey <= 10 "
                 " ORDER BY o_comment, o_custkey, o_orderkey"; // order by to ensure order is reproducible and not sorted on the sort cols


    SortDefinition sort_definition;
    sort_definition.emplace_back(2, SortDirection::DESCENDING);
    sort_definition.emplace_back(1, SortDirection::ASCENDING);

    auto input = new SecureSqlInput(db_name_, sql, false);
    Sort<emp::Bit> sort(input, sort_definition);

    PlainTable *observed = sort.run()->reveal();

    ASSERT_TRUE(isSorted(observed, sort_definition));


    delete observed;

}






int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}




