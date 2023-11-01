#include <operators/sql_input.h>
#include <operators/merge_join.h>
#include <util/utilities.h>
#include "plain_base_test.h"
#include "util/field_utilities.h"
#include <gflags/gflags.h>

DEFINE_int32(cutoff, 100, "limit clause for queries");
DEFINE_string(filter, "*", "run only the tests passing this filter");

class MergeJoinTest : public PlainBaseTest {};

/*TEST_F(MergeJoinTest, merge_q18_without_cutoff) {
    string lhs_sql = "SELECT\n"
                     "    l.l_orderkey,\n"
                     "    CASE WHEN lq.total_quantity > 7 THEN FALSE ELSE TRUE END AS dummy_tag\n"
                     "FROM\n"
                     "    lineitem l\n"
                     "LEFT JOIN (\n"
                     "    SELECT\n"
                     "        l_orderkey,\n"
                     "        SUM(l_quantity) AS total_quantity\n"
                     "    FROM\n"
                     "        lineitem\n"
                     "    GROUP BY\n"
                     "        l_orderkey\n"
                     ") lq ON l.l_orderkey = lq.l_orderkey\n"
                     "ORDER BY\n"
                     "    l.l_orderkey;";

    string rhs_sql = "SELECT o_orderkey FROM orders ORDER BY o_orderkey";

    string expected_sql = "WITH high_quantity AS (\n"
                          "    SELECT l_orderkey\n"
                          "    FROM lineitem\n"
                          "    GROUP BY l_orderkey\n"
                          "    HAVING SUM(l_quantity) > 7\n"
                          ")\n"
                          "SELECT hq.l_orderkey, l.l_orderkey, l.l_quantity \n"
                          "FROM high_quantity hq JOIN lineitem l  ON l.l_orderkey = hq.l_orderkey\n"
                          "ORDER BY l.l_orderkey";


    auto collation = DataUtilities::getDefaultSortDefinition(1);

    SqlInput *lhs_input = new SqlInput(db_name_, lhs_sql, true, collation);
    SqlInput *rhs_input = new SqlInput(db_name_, rhs_sql, false, collation);

    Expression<bool> *predicate = FieldUtilities::getEqualityPredicate<bool>(lhs_input, 0,
                                                                                              rhs_input, 2);

    MergeJoin join(lhs_input, rhs_input, predicate, SortDefinition(), false);
    auto joined = join.run();

    auto expected = DataUtilities::getQueryResults(db_name_, expected_sql, false);
    expected->setSortOrder(collation);

    ASSERT_EQ(*expected, *joined);

}
*/

TEST_F(MergeJoinTest, merge_q18) {
    string lhs_sql = " SELECT l_orderkey, COUNT(*) \n"
                     " FROM lineitem\n"
                     " GROUP BY l_orderkey\n"
                     " ORDER BY l_orderkey "
                     " LIMIT "  + std::to_string(FLAGS_cutoff);

    string rhs_sql = "SELECT DISTINCT l_orderkey FROM lineitem ORDER BY l_orderkey LIMIT " + std::to_string(FLAGS_cutoff);

    string expected_sql = " SELECT l_orderkey, COUNT(*), l_orderkey \n"
                          " FROM lineitem\n"
                          " GROUP BY l_orderkey\n"
                          " ORDER BY l_orderkey "
                          " LIMIT "  + std::to_string(FLAGS_cutoff);


    auto collation = DataUtilities::getDefaultSortDefinition(1);

    SqlInput *lhs_input = new SqlInput(db_name_, lhs_sql, false, collation);
    SqlInput *rhs_input = new SqlInput(db_name_, rhs_sql, false, collation);

    Expression<bool> *predicate = FieldUtilities::getEqualityPredicate<bool>(lhs_input, 0,
                                                                                              rhs_input, 2);

    MergeJoin join(lhs_input, rhs_input, predicate, SortDefinition(), false);
    auto joined = join.run();

    auto expected = DataUtilities::getQueryResults(db_name_, expected_sql, false);
    expected->setSortOrder(collation);

    ASSERT_EQ(*expected, *joined);

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    ::testing::GTEST_FLAG(filter)=FLAGS_filter;
    int i = RUN_ALL_TESTS();
    google::ShutDownCommandLineFlags();
    return i;

}
