#include <operators/sql_input.h>
#include <operators/merge_join.h>
#include <util/utilities.h>
#include "plain_base_test.h"
#include "util/field_utilities.h"
#include <gflags/gflags.h>

DEFINE_int32(cutoff, 100, "limit clause for queries");
DEFINE_string(storage, "row", "storage model for tables (row or column)");
DEFINE_string(filter, "*", "run only the tests passing this filter");

class MergeJoinTest : public PlainBaseTest {};

TEST_F(MergeJoinTest, merge_q18) {
    string lhs_sql = " SELECT l_orderkey, COUNT(*) \n"
                     " FROM lineitem\n"
                     " GROUP BY l_orderkey\n"
                     " ORDER BY l_orderkey";

    string rhs_sql = "SELECT DISTINCT l_orderkey FROM lineitem ORDER BY l_orderkey";
    string expected_sql = " SELECT l_orderkey, COUNT(*), l_orderkey \n"
                          " FROM lineitem\n"
                          " GROUP BY l_orderkey\n"
                          " ORDER BY l_orderkey";

    auto collation = DataUtilities::getDefaultSortDefinition(1);

    SqlInput *lhs_input = new SqlInput(db_name_, lhs_sql, false, collation);
    SqlInput *rhs_input = new SqlInput(db_name_, rhs_sql, false, collation);

    Expression<bool> *predicate = FieldUtilities::getEqualityPredicate<bool>(lhs_input, 0,
                                                                                              rhs_input, 2);

    MergeJoin join(lhs_input, rhs_input, predicate);
    auto joined = join.run();

    auto expected = DataUtilities::getQueryResults(db_name_, expected_sql, false);
    expected->setSortOrder(collation);

    ASSERT_EQ(*expected, *joined);

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    ::testing::GTEST_FLAG(filter)=FLAGS_filter;
    return RUN_ALL_TESTS();
}
