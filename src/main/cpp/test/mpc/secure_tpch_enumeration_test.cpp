#include "operators/basic_join.h"
#include "operators/keyed_join.h"
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <util/type_utilities.h>
#include <util/data_utilities.h>
#include <test/mpc/emp_base_test.h>
#include <test/support/tpch_queries.h>
#include <boost/algorithm/string/replace.hpp>
#include <parser/plan_parser.h>
#include <opt/join_graph.h>

using namespace emp;
using namespace vaultdb;



DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 7654, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for EMP execution");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(alice_db, "tpch_alice_150", "alice db name");
DEFINE_string(bob_db, "tpch_bob_150", "bob db name");
DEFINE_int32(ctrl_port, 65478, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*.tpch_q05", "run only the tests passing this filter");
DEFINE_string(storage, "column", "storage model for columns (column, wire_packed or compressed)");


class SecureTpcHEnumerationTest : public EmpBaseTest {


protected:

    // depends on truncate-tpch-set.sql
    void runTest(const int &test_id, const SortDefinition &expected_sort);
    string  generateExpectedOutputQuery(const int & test_id);

    int input_tuple_limit_ = 150;

};

void SecureTpcHEnumerationTest::runTest(const int &test_id, const SortDefinition &expected_sort) {

    string expected_sql = generateExpectedOutputQuery(test_id);
    PlainTable *expected = DataUtilities::getExpectedResults(FLAGS_unioned_db, expected_sql, false, 0);
    expected->order_by_ = expected_sort;

    ASSERT_TRUE(!expected->empty()); // want all tests to produce output

    string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/mpc-q" + std::to_string(test_id) + ".json";

    PlanParser<Bit> parser(db_name_, plan_file, input_tuple_limit_, true);
   SecureOperator *root = parser.getRoot();

   cout << "Query tree: " << root->printTree() << endl;

   // 3 phases:
   // * Identify all leafs - subtrees that are below all joins
   // * Parse each join predicate and identify the leaf(s) associated with it.  Create one edge per distinct pair of leaf nodes.
   // * Take closure and add any edges that can be derived from transitivity over join predicates. TODO: implement this
   JoinGraph<Bit> graph(root, db_name_);
   cout << "Join graph: \n" << graph.toString() << endl;

   // Q5 bug:   * Operator 8: $1 == $3 bool schema: (#0 shared-int32(13) lineitem.l_orderkey, #1 shared-int32(4) lineitem.l_suppkey, #2 shared-float revenue, #3 shared-int32(4) supplier.s_suppkey, #4 shared-int32(5) supplier.s_nationkey, #5 shared-varchar(25) nation.n_name), FK side: -1
   // FK should be 1
   // key missing because of how we're partitioning the data.
   // need to add a table like:
   // nation.n_nationkey <-- supplier.s_nationkey
    //nation.n_nationkey <-- customer.c_nationkey
    //orders.o_orderkey <-- lineitem.l_orderkey
    //region.r_regionkey <-- nation.n_regionkey
    //customer.c_custkey <-- orders.o_custkey
    //part.p_partkey <-- partsupp.ps_partkey
    //supplier.s_suppkey <-- partsupp.ps_suppkey
    // supplier.s_suppkey <-- lineitem.ps_suppkey
    // (ps_partkey, ps_suppkey) <-- (lineitem.l_partkey, lineitem.l_suppkey)

}

string SecureTpcHEnumerationTest::generateExpectedOutputQuery(const int &test_id) {
    string query = tpch_queries[test_id];
    if (input_tuple_limit_ > 0) {
        query = (emp_mode_ == EmpMode::SH2PC) ? truncated_tpch_queries[test_id] : truncated_tpch_queries_single_input_party[test_id];
        boost::replace_all(query, "$LIMIT", std::to_string(input_tuple_limit_));
    }
    return query;
}

TEST_F(SecureTpcHEnumerationTest, tpch_q01) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest(1, expected_sort);
}

 TEST_F(SecureTpcHEnumerationTest, tpch_q03) {

    input_tuple_limit_ = (emp_mode_ == EmpMode::SH2PC) ? 500 : 1200;
    // dummy_tag (-1), 1 DESC, 2 ASC
    // aka revenue desc,  o.o_orderdate
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING)};

//    runTest(3, expected_sort);

    string expected_sql = generateExpectedOutputQuery(3);
    PlainTable *expected = DataUtilities::getExpectedResults(FLAGS_unioned_db, expected_sql, false, 0);
    expected->order_by_ = expected_sort;

    ASSERT_TRUE(!expected->empty()); // want all tests to produce output

    string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/mpc-q" + std::to_string(3) + ".json";
    PlanParser<Bit> parser(db_name_, plan_file, input_tuple_limit_, true);
    SecureOperator *root = parser.getRoot();
    // 3 phases:
    // * Identify all leafs - subtrees that are below all joins
    // * Parse each join predicate and identify the leaf(s) associated with it.  Create one edge per distinct pair of leaf nodes.
    // * Take closure and add any edges that can be derived from transitivity over join predicates. TODO: implement this
    JoinGraph<Bit> graph(root, db_name_);
    cout << "Join graph: \n" << graph.toString() << endl;

    // compose first join, just using BasicJoin until we store PK-FK relationships in the graph or use the schema to infer these relationships
    JoinEdge<Bit> &e = graph.edges_.back();
    // // fkey = 0 --> lhs, fkey = 1 --> rhs
    //        KeyedJoin(Operator<B> *lhs, Operator<B> *rhs,  const int & fkey, Expression<B> *predicate,const SortDefinition & sort = SortDefinition());

    graph.joinInEdge(e);
//    int pk_flag = !e.pkey_input_;
//    KeyedJoin<Bit> j1(e.lhs_->node_, e.rhs_->node_, pk_flag, e.join_condition_);
//
//    // substitute new join into graph by replacing reference to its children with the new join
//    graph.replaceEdge(e, (Join<Bit> *) &j1);

}


 // passes on codd2 in about 2.5 mins
TEST_F(SecureTpcHEnumerationTest, tpch_q05) {

    input_tuple_limit_ = (emp_mode_ == EmpMode::SH2PC) ? 200 : 400;


    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest(5, expected_sort);

}

//  passes in 56 secs on codd12
TEST_F(SecureTpcHEnumerationTest, tpch_q08) {
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING)};
    input_tuple_limit_ = (emp_mode_ == EmpMode::SH2PC) ? 400 : 800;
    runTest(8, expected_sort);
}

 // *passes in around ~42 secs on codd2
TEST_F(SecureTpcHEnumerationTest, tpch_q09) {
    // $0 ASC, $1 DESC
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest(9, expected_sort);

}
// passes in ~2.5 mins
TEST_F(SecureTpcHEnumerationTest, tpch_q18) {
    input_tuple_limit_ = (emp_mode_ == EmpMode::SH2PC) ? 200 : 400;

    // -1 ASC, $4 DESC, $3 ASC
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(4, SortDirection::DESCENDING),
                                 ColumnSort(3, SortDirection::ASCENDING)};


    runTest(18, expected_sort);
}




int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

	::testing::GTEST_FLAG(filter)=FLAGS_filter;
    int i = RUN_ALL_TESTS();
    google::ShutDownCommandLineFlags();
    return i;

}

