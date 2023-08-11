#include <operators/sql_input.h>
#include <operators/merge_join.h>
#include <util/utilities.h>
#include "emp_base_test.h"
#include "util/field_utilities.h"
#include "operators/secure_sql_input.h"
#include "operators/sort.h"
#include <gflags/gflags.h>

DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43450, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for execution");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(alice_db, "tpch_alice_150", "alice db name");
DEFINE_string(bob_db, "tpch_bob_150", "bob db name");
DEFINE_int32(cutoff, 100, "limit clause for queries");
DEFINE_string(storage, "row", "storage model for tables (row or column)");
DEFINE_int32(ctrl_port, 65458, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");

class SecureMergeJoinTest : public EmpBaseTest {};

TEST_F(SecureMergeJoinTest, merge_q18) {
    // N/A in ZK/OMPC
    if(emp_mode_ == EmpMode::SH2PC) {
        string lhs_sql = " SELECT l_orderkey, COUNT(*) \n"
                         " FROM lineitem\n"
                         " GROUP BY l_orderkey\n"
                         " ORDER BY l_orderkey "
                         " LIMIT " + std::to_string(FLAGS_cutoff);

        string rhs_sql =
                "SELECT DISTINCT l_orderkey FROM lineitem ORDER BY l_orderkey LIMIT " + std::to_string(FLAGS_cutoff);

        string expected_sql = " SELECT l_orderkey, COUNT(*), l_orderkey \n"
                              " FROM lineitem\n"
                              " GROUP BY l_orderkey\n"
                              " ORDER BY l_orderkey "
                              " LIMIT " + std::to_string(FLAGS_cutoff);;

        auto collation = DataUtilities::getDefaultSortDefinition(1);
        auto *lhs_input = new SecureSqlInput(db_name_, lhs_sql, false, collation);
        auto *rhs_input = new SecureSqlInput(db_name_, rhs_sql, false, collation);

        Expression<Bit> *predicate = FieldUtilities::getEqualityPredicate<Bit>(lhs_input, 0,
                                                                               rhs_input, 2);

        MergeJoin join(lhs_input, rhs_input, predicate, SortDefinition(), false);
        auto joined = join.run()->reveal();

        // need unioned results to cover a corner case
        auto expected = DataUtilities::getUnionedResults(FLAGS_alice_db, FLAGS_bob_db, expected_sql, false);

        Sort sorter(expected, collation);
        expected = sorter.run();
        ASSERT_EQ(*expected, *joined);

        delete joined;
    }

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    ::testing::GTEST_FLAG(filter)=FLAGS_filter;
    return RUN_ALL_TESTS();
}
