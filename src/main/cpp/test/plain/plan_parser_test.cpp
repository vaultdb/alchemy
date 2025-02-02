#include "plain_base_test.h"
#include "operators/sql_input.h"
#include "data/csv_reader.h"
#include "parser/plan_parser.h"
#include "test/support/tpch_queries.h"
#include <boost/algorithm/string/replace.hpp>


// e.g., ./bin/plan_parser_test --filter="PlanParserTest.tpch_q1"
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "column", "storage model for columns (column or compressed)");


class PlanParserTest : public PlainBaseTest {


protected:

    // depends on truncate-tpch.sql
    // limit input to first N tuples per SQL statement
    int limit_ = 10;

    void runTest(const int &test_id, const std::string &expected_plan);

};

// most of these runs are not meaningful for diffing the results because they produce no tuples - joins are too sparse.
// This isn't relevant to the parser so work on this elsewhere.
void PlanParserTest::runTest(const int &test_id, const std::string &expected_plan) {
    string test_name = "q" + std::to_string(test_id);
    string plan_file = Utilities::getCurrentWorkingDirectory() + "/conf/plans/mpc-" + test_name + ".json";

    PlanParser<bool> plan_reader(db_name_, plan_file, limit_, true);
    PlainOperator *root = plan_reader.getRoot();
    std::string observed_plan = root->printTree();

    ASSERT_EQ(expected_plan, observed_plan);

    delete root;

}



// sort not really needed in Q1 MPC pipeline, retaining it to demo the hand-off from one op to the next
TEST_F(PlanParserTest, tpch_q01) {
    std::string expected_plan = "#1: SortMergeAggregate<bool> (group-by: (0, 1) aggs: (SUM($2) sum_qty, SUM($3) sum_base_price, SUM($4) sum_disc_price, SUM($5) sum_charge, AVG($2) avg_qty, AVG($3) avg_price, AVG($6) avg_disc, COUNT(*) count_order)) : (#0 varchar(1) lineitem.l_returnflag, #1 varchar(1) lineitem.l_linestatus, #2 float sum_qty, #3 float sum_base_price, #4 float sum_disc_price, #5 float sum_charge, #6 float avg_qty, #7 float avg_price, #8 float avg_disc, #9 int64 count_order) order by: {<0, ASC> , <1, ASC> }\n"
         "    #0: SqlInput<bool> (\"SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice, l_extendedprice * (1 - l_discount) AS discount, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, l_discount, NOT l_shipdate <= DATE '1998-08-03' AS dummy_tag FROM lineitem ORDER BY  l_returnflag, l_linestatus, l_orderkey, l_linenumber\", tuple_count=10) : (#0 varchar(1) lineitem.l_returnflag, #1 varchar(1) lineitem.l_linestatus, #2 float lineitem.l_quantity, #3 float lineitem.l_extendedprice, #4 float discount, #5 float charge, #6 float lineitem.l_discount) order by: {<0, ASC> , <1, ASC> }\n";
    runTest(1, expected_plan);
}


TEST_F(PlanParserTest, tpch_q03) {
           string expected_plan =   "#9: Project<bool> ((<0, $0 int32>, <1, $3 float>, <2, $1 int64>, <3, $2 int32>)) : (#0 int32 orders.l_orderkey, #1 float revenue, #2 int64 orders.o_orderdate, #3 int32 orders.o_shippriority) order by: {<-1, ASC> , <1, DESC> , <2, ASC> }\n"
    "    #8: Sort<bool> ({<-1, ASC> , <3, DESC> , <1, ASC> }) : (#0 int32 orders.o_orderkey, #1 int64 orders.o_orderdate, #2 int32 orders.o_shippriority, #3 float revenue) order by: {<-1, ASC> , <3, DESC> , <1, ASC> }\n"
    "        #7: SortMergeAggregate<bool> (group-by: (0, 1, 2) aggs: (SUM($3) revenue)) : (#0 int32 orders.o_orderkey, #1 int64 orders.o_orderdate, #2 int32 orders.o_shippriority, #3 float revenue) order by: {<0, ASC> }\n"
    "            #-1: Sort<bool> ({<0, ASC> , <1, ASC> , <2, ASC> }) : (#0 int32 orders.o_orderkey, #1 int64 orders.o_orderdate, #2 int32 orders.o_shippriority, #3 float revenue) order by: {<0, ASC> , <1, ASC> , <2, ASC> }\n"
    "                #6: Project<bool> ((<0, $0 int32>, <1, $1 int64>, <2, $2 int32>, <3, $4 float>)) : (#0 int32 orders.o_orderkey, #1 int64 orders.o_orderdate, #2 int32 orders.o_shippriority, #3 float revenue) order by: {}\n"
    "                    #5: KeyedJoin<bool> ($0 == $3) : (#0 int32 orders.o_orderkey, #1 int64 orders.o_orderdate, #2 int32 orders.o_shippriority, #3 int32 lineitem.l_orderkey, #4 float revenue) order by: {<3, ASC> , <0, ASC> }\n"
    "                        #3: Project<bool> ((<0, $0 int32>, <1, $2 int64>, <2, $3 int32>)) : (#0 int32 orders.o_orderkey, #1 int64 orders.o_orderdate, #2 int32 orders.o_shippriority) order by: {<0, ASC> }\n"
    "                            #2: KeyedJoin<bool> ($1 == $4) : (#0 int32 orders.o_orderkey, #1 int32 orders.o_custkey, #2 int64 orders.o_orderdate, #3 int32 orders.o_shippriority, #4 int32 customer.c_custkey) order by: {<0, ASC> , <4, ASC> }\n"
    "                                #0: SqlInput<bool> (\"SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, NOT o_orderdate < DATE '1995-03-25' AS dummy_tag FROM orders ORDER BY o_orderkey, o_orderdate, o_shippriority\", tuple_count=10) : (#0 int32 orders.o_orderkey, #1 int32 orders.o_custkey, #2 int64 orders.o_orderdate, #3 int32 orders.o_shippriority) order by: {<0, ASC> }\n"
    "                                #1: SqlInput<bool> (\"SELECT c_custkey, NOT c_mktsegment = 'HOUSEHOLD ' AS dummy_tag FROM customer ORDER BY c_custkey\", tuple_count=10) : (#0 int32 customer.c_custkey) order by: {<0, ASC> }\n"
    "                        #4: SqlInput<bool> (\"SELECT l_orderkey, l_extendedprice * (1 - l_discount) AS revenue, NOT l_shipdate > DATE '1995-03-25' AS dummy_tag FROM lineitem ORDER BY l_orderkey\", tuple_count=10) : (#0 int32 lineitem.l_orderkey, #1 float revenue) order by: {<0, ASC> }\n";
    runTest(3, expected_plan);
}


TEST_F(PlanParserTest, tpch_q05) {
    std::string expected_plan = "#11: Sort<bool> ({<1, DESC> }) : (#0 varchar(25) nation.n_name, #1 float revenue) order by: {<1, DESC> }\n"
    "    #10: NestedLoopAggregate<bool> (group-by: (0) aggs: (SUM($1) revenue)) : (#0 varchar(25) nation.n_name, #1 float revenue) order by: {}\n"
    "        #9: Project<bool> ((<0, $5 varchar>, <1, $2 float>)) : (#0 varchar(25) nation.n_name, #1 float revenue) order by: {}\n"
    "            #8: KeyedJoin<bool> (($0 == $4) AND ($1 == $3)) : (#0 int32 customer.c_nationkey, #1 int32 lineitem.l_suppkey, #2 float revenue, #3 int32 supplier.s_suppkey, #4 int32 supplier.s_nationkey, #5 varchar(25) nation.n_name) order by: {}\n"
    "                #6: Project<bool> ((<0, $0 int32>, <1, $3 int32>, <2, $4 float>)) : (#0 int32 customer.c_nationkey, #1 int32 lineitem.l_suppkey, #2 float revenue) order by: {}\n"
    "                    #5: KeyedJoin<bool> ($1 == $2) : (#0 int32 customer.c_nationkey, #1 int32 orders.o_orderkey, #2 int32 lineitem.l_orderkey, #3 int32 lineitem.l_suppkey, #4 float revenue) order by: {<2, ASC> , <1, ASC> }\n"
    "                        #3: Project<bool> ((<0, $1 int32>, <1, $2 int32>)) : (#0 int32 customer.c_nationkey, #1 int32 orders.o_orderkey) order by: {<1, ASC> }\n"
    "                            #2: KeyedJoin<bool> ($0 == $3) : (#0 int32 customer.c_custkey, #1 int32 customer.c_nationkey, #2 int32 orders.o_orderkey, #3 int32 orders.o_custkey) order by: {<2, ASC> , <0, ASC> }\n"
    "                                #0: SqlInput<bool> (\"SELECT t2.c_custkey, t2.c_nationkey FROM (SELECT r_regionkey, r_name FROM region WHERE r_name = 'EUROPE') AS t0 INNER JOIN (SELECT n_nationkey, n_name, n_regionkey FROM nation) AS t1 ON t0.r_regionkey = t1.n_regionkey INNER JOIN (SELECT c_custkey, c_nationkey FROM customer) AS t2 ON t1.n_nationkey = t2.c_nationkey ORDER BY t2.c_custkey ASC\", tuple_count=10) : (#0 int32 customer.c_custkey, #1 int32 customer.c_nationkey) order by: {<0, ASC> }\n"
    "                                #1: SqlInput<bool> (\"SELECT o_orderkey, o_custkey, NOT (o_orderdate >= DATE '1993-01-01' AND o_orderdate < DATE '1994-01-01') AS dummy_tag FROM orders ORDER BY o_orderkey, o_custkey, o_orderdate\", tuple_count=10) : (#0 int32 orders.o_orderkey, #1 int32 orders.o_custkey) order by: {<0, ASC> }\n"
    "                        #4: SqlInput<bool> (\"SELECT l_orderkey, l_suppkey, l_extendedprice * (1 - l_discount) AS revenue FROM lineitem ORDER BY l_orderkey, l_suppkey, revenue\", tuple_count=10) : (#0 int32 lineitem.l_orderkey, #1 int32 lineitem.l_suppkey, #2 float revenue) order by: {<0, ASC> }\n"
    "                #7: SqlInput<bool> (\"SELECT s_suppkey, s_nationkey, n_name FROM supplier JOIN nation ON s_nationkey = n_nationkey JOIN region ON n_regionkey = r_regionkey WHERE r_name = 'EUROPE' ORDER BY s_suppkey\", tuple_count=1) : (#0 int32 supplier.s_suppkey, #1 int32 supplier.s_nationkey, #2 varchar(25) nation.n_name) order by: {<0, ASC> }\n";
    runTest(5, expected_plan);
}

TEST_F(PlanParserTest, tpch_q08) {

    std::string expected_plan = "#12: Sort<bool> ({<0, ASC> }) : (#0 int32 orders.o_year, #1 float mkt_share) order by: {<0, ASC> }\n"
                                "    #11: Project<bool> ((<0, $0 int32>, <1, $2 / $1 float>)) : (#0 int32 orders.o_year, #1 float mkt_share) order by: {}\n"
                                "        #10: NestedLoopAggregate<bool> (group-by: (0) aggs: (SUM($1) null, SUM($2) null)) : (#0 int32 orders.o_year, #1 float null, #2 float null) order by: {}\n"
                                "            #9: Project<bool> ((<0, $0 int32>, <1, $2 float>, <2, CASE($4, $2, 0.000000) float>)) : (#0 int32 orders.o_year, #1 float volume, #2 float selected_volume) order by: {}\n"
                                "                #8: KeyedJoin<bool> ($3 == $1) : (#0 int32 orders.o_year, #1 int32 lineitem.l_suppkey, #2 float volume, #3 int32 supplier.s_suppkey, #4 bool nation_check) order by: {}\n"
                                "                    #6: Project<bool> ((<0, $1 int32>, <1, $3 int32>, <2, $4 float>)) : (#0 int32 orders.o_year, #1 int32 lineitem.l_suppkey, #2 float volume) order by: {}\n"
                                "                        #5: KeyedJoin<bool> ($0 == $2) : (#0 int32 orders.o_orderkey, #1 int32 orders.o_year, #2 int32 lineitem.l_orderkey, #3 int32 lineitem.l_suppkey, #4 float volume) order by: {<2, ASC> , <1, ASC> , <0, ASC> }\n"
                                "                            #3: Project<bool> ((<0, $1 int32>, <1, $3 int32>)) : (#0 int32 orders.o_orderkey, #1 int32 orders.o_year) order by: {<1, ASC> , <0, ASC> }\n"
                                "                                #2: KeyedJoin<bool> ($0 == $2) : (#0 int32 customer.c_custkey, #1 int32 orders.o_orderkey, #2 int32 orders.o_custkey, #3 int32 orders.o_year) order by: {<3, ASC> , <1, ASC> , <0, ASC> }\n"
                                "                                    #0: SqlInput<bool> (\"SELECT c_custkey FROM customer c JOIN nation n1 ON c_nationkey = n_nationkey JOIN region ON r_regionkey = n_regionkey WHERE r_name = 'AFRICA' ORDER BY c_custkey\", tuple_count=10) : (#0 int32 customer.c_custkey) order by: {<0, ASC> }\n"
                                "                                    #1: SqlInput<bool> (\"SELECT o_orderkey, o_custkey, o_orderyear::INT AS o_year, NOT (o_orderdate >= date '1995-01-01' and o_orderdate < date '1996-12-31') dummy_tag FROM orders ORDER BY o_year, o_orderkey\", tuple_count=10) : (#0 int32 orders.o_orderkey, #1 int32 orders.o_custkey, #2 int32 orders.o_year) order by: {<2, ASC> , <0, ASC> }\n"
                                "                            #4: SqlInput<bool> (\"SELECT l_orderkey, l_suppkey, l_extendedprice * (1.0 - l_discount) AS volume, CASE WHEN p_type = 'LARGE ANODIZED STEEL' AND p_partkey = l_partkey THEN FALSE ELSE TRUE END AS dummy_tag FROM lineitem LEFT JOIN part ON p_partkey = l_partkey ORDER BY l_orderkey\", tuple_count=10) : (#0 int32 lineitem.l_orderkey, #1 int32 lineitem.l_suppkey, #2 float volume) order by: {<0, ASC> }\n"
                                "                    #7: SqlInput<bool> (\"SELECT s_suppkey, CASE WHEN n_name = 'KENYA' THEN true ELSE false END AS nation_check FROM supplier JOIN nation ON n_nationkey = s_nationkey ORDER BY s_suppkey\", tuple_count=10) : (#0 int32 supplier.s_suppkey, #1 bool nation_check) order by: {<0, ASC> }\n";
    runTest(8, expected_plan);
}

// q9 expresssion:   l.l_extendedprice * (1 - l.l_discount) - ps.ps_supplycost * l.l_quantity
TEST_F(PlanParserTest, tpch_q09) {

    std::string expected_plan =  "#11: Sort<bool> ({<0, ASC> , <1, DESC> }) : (#0 varchar(25) nation.nation, #1 int32 amount, #2 float sum_profit) order by: {<0, ASC> , <1, DESC> }\n"
    "    #10: SortMergeAggregate<bool> (group-by: (0, 1) aggs: (SUM($2) sum_profit)) : (#0 varchar(25) nation.nation, #1 int32 amount, #2 float sum_profit) order by: {<0, ASC> , <1, DESC> }\n"
    "        #-1: Sort<bool> ({<0, ASC> , <1, ASC> }) : (#0 varchar(25) nation.nation, #1 int32 amount, #2 float o_year) order by: {<0, ASC> , <1, ASC> }\n"
    "            #9: Project<bool> ((<0, $0 varchar>, <1, $3 int32>, <2, $2 float>)) : (#0 varchar(25) nation.nation, #1 int32 amount, #2 float o_year) order by: {}\n"
    "                #8: KeyedJoin<bool> ($1 == $4) : (#0 varchar(25) nation.nation, #1 int32 lineitem.l_orderkey, #2 float amount, #3 int32 o_year, #4 int32 orders.o_orderkey) order by: {<1, ASC> , <4, ASC> }\n"
    "                    #6: Project<bool> ((<0, $0 varchar>, <1, $4 int32>, <2, ($8 * (1.000000 - $9)) - ($3 * $7) float>)) : (#0 varchar(25) nation.nation, #1 int32 lineitem.l_orderkey, #2 float amount) order by: {<1, ASC> }\n"
    "                        #5: KeyedJoin<bool> (($2 == $6) AND ($1 == $5)) : (#0 varchar(25) nation.nation, #1 int32 partsupp.ps_partkey, #2 int32 partsupp.ps_suppkey, #3 float partsupp.ps_supplycost, #4 int32 lineitem.l_orderkey, #5 int32 lineitem.l_partkey, #6 int32 lineitem.l_suppkey, #7 float l_quantity, #8 float lineitem.l_extendedprice, #9 float lineitem.l_discount) order by: {<4, ASC> , <5, ASC> , <6, ASC> , <1, ASC> , <2, ASC> , <0, ASC> }\n"
    "                            #3: Project<bool> ((<0, $1 varchar>, <1, $2 int32>, <2, $3 int32>, <3, $4 float>)) : (#0 varchar(25) nation.nation, #1 int32 partsupp.ps_partkey, #2 int32 partsupp.ps_suppkey, #3 float partsupp.ps_supplycost) order by: {<1, ASC> , <2, ASC> , <0, ASC> }\n"
    "                                #2: KeyedJoin<bool> ($0 == $3) : (#0 int32 supplier.s_suppkey, #1 varchar(25) nation.n_name, #2 int32 partsupp.ps_partkey, #3 int32 partsupp.ps_suppkey, #4 float partsupp.ps_supplycost) order by: {<2, ASC> , <3, ASC> , <1, ASC> , <0, ASC> }\n"
    "                                    #0: SqlInput<bool> (\"SELECT s_suppkey, n_name FROM supplier JOIN nation ON s_nationkey = n_nationkey ORDER BY n_name, s_suppkey\", tuple_count=10) : (#0 int32 supplier.s_suppkey, #1 varchar(25) nation.n_name) order by: {<1, ASC> , <0, ASC> }\n"
    "                                    #1: SqlInput<bool> (\"SELECT ps_partkey, ps_suppkey, ps_supplycost FROM partsupp JOIN part ON ps_partkey = p_partkey WHERE p_name LIKE '%yellow%' ORDER BY ps_suppkey, ps_partkey\", tuple_count=10) : (#0 int32 partsupp.ps_partkey, #1 int32 partsupp.ps_suppkey, #2 float partsupp.ps_supplycost) order by: {<0, ASC> , <1, ASC> }\n"
    "                            #4: SqlInput<bool> (\"SELECT l_orderkey, l_partkey, l_suppkey, l_quantity::float4, l_extendedprice, l_discount FROM lineitem ORDER BY l_orderkey, l_partkey, l_suppkey\", tuple_count=10) : (#0 int32 lineitem.l_orderkey, #1 int32 lineitem.l_partkey, #2 int32 lineitem.l_suppkey, #3 float l_quantity, #4 float lineitem.l_extendedprice, #5 float lineitem.l_discount) order by: {<0, ASC> , <1, ASC> , <2, ASC> }\n"
    "                    #7: SqlInput<bool> (\"SELECT EXTRACT('year' FROM o_orderdate)::INT AS o_year, o_orderkey FROM orders ORDER BY o_orderkey\", tuple_count=10) : (#0 int32 o_year, #1 int32 orders.o_orderkey) order by: {<1, ASC> }\n";

    runTest(9, expected_plan);

}



TEST_F(PlanParserTest, tpch_q18) {

    std::string expected_plan =  "#11: Sort<bool> ({<-1, ASC> , <4, DESC> , <3, ASC> }) : (#0 varchar(25) customer.c_name, #1 int32 customer.c_custkey, #2 int32 order_keys.o_orderkey, #3 int64 orders.o_orderdate, #4 float orders.o_totalprice, #5 float sum_qty) order by: {<-1, ASC> , <4, DESC> , <3, ASC> }\n"
    "    #10: Project<bool> ((<0, $6 varchar>, <1, $5 int32>, <2, $0 int32>, <3, $3 int64>, <4, $2 float>, <5, $4 float>)) : (#0 varchar(25) customer.c_name, #1 int32 customer.c_custkey, #2 int32 order_keys.o_orderkey, #3 int64 orders.o_orderdate, #4 float orders.o_totalprice, #5 float sum_qty) order by: {<2, ASC> }\n"
    "        #9: KeyedJoin<bool> ($1 == $5) : (#0 int32 order_keys.o_orderkey, #1 int32 orders.o_custkey, #2 float orders.o_totalprice, #3 int64 orders.o_orderdate, #4 float sum_qty, #5 int32 customer.c_custkey, #6 varchar(25) customer.c_name) order by: {<0, ASC> }\n"
    "            #7: Project<bool> ((<0, $0 int32>, <1, $3 int32>, <2, $4 float>, <3, $5 int64>, <4, $1 float>)) : (#0 int32 order_keys.o_orderkey, #1 int32 orders.o_custkey, #2 float orders.o_totalprice, #3 int64 orders.o_orderdate, #4 float sum_qty) order by: {<0, ASC> }\n"
    "                #6: MergeJoin<bool> (no-op) : (#0 int32 order_keys.o_orderkey, #1 float sum_qty, #2 int32 orders.o_orderkey, #3 int32 orders.o_custkey, #4 float orders.o_totalprice, #5 int64 orders.o_orderdate) order by: {<0, ASC> }\n"
    "                    #4: Filter<bool> (P$1(32) > 7.000000) : (#0 int32 order_keys.o_orderkey, #1 float sum_qty) order by: {<0, ASC> }\n"
    "                        #3: Project<bool> ((<0, $0 int32>, <1, $1 + $3 float>)) : (#0 int32 order_keys.o_orderkey, #1 float sum_qty) order by: {<0, ASC> }\n"
    "                            #2: MergeJoin<bool> (no-op) : (#0 int32 order_keys.o_orderkey, #1 float sum_qty, #2 int32 order_keys.o_orderkey, #3 float sum_qty) order by: {<0, ASC> }\n"
    "                                #0: SqlInput<bool> (\"WITH all_keys AS (SELECT o_orderkey  FROM order_keys), sum_qtys AS (SELECT l_orderkey, sum(l_quantity) as sum_qty, false AS dummy_tag FROM lineitem GROUP BY l_orderkey ORDER BY l_orderkey) SELECT o_orderkey, COALESCE(sum_qty, 0.0) sum_qty, COALESCE(dummy_tag, true) dummy_tag FROM all_keys LEFT JOIN sum_qtys ON l_orderkey = o_orderkey ORDER BY o_orderkey \", tuple_count=10) : (#0 int32 order_keys.o_orderkey, #1 float sum_qty) order by: {<0, ASC> }\n"
    "                                #1: SqlInput<bool> (\"WITH all_keys AS (SELECT o_orderkey  FROM order_keys), sum_qtys AS (SELECT l_orderkey, sum(l_quantity) as sum_qty, false AS dummy_tag FROM lineitem GROUP BY l_orderkey ORDER BY l_orderkey) SELECT o_orderkey, COALESCE(sum_qty, 0.0) sum_qty, COALESCE(dummy_tag, true) dummy_tag FROM all_keys LEFT JOIN sum_qtys ON l_orderkey = o_orderkey ORDER BY o_orderkey \", tuple_count=10) : (#0 int32 order_keys.o_orderkey, #1 float sum_qty) order by: {<0, ASC> }\n"
    "                    #5: SqlInput<bool> (\"SELECT o_orderkey, o_custkey, o_totalprice, o_orderdate, false dummy_tag FROM orders ORDER BY o_orderkey\", tuple_count=10) : (#0 int32 orders.o_orderkey, #1 int32 orders.o_custkey, #2 float orders.o_totalprice, #3 int64 orders.o_orderdate) order by: {<0, ASC> }\n"
    "            #8: SqlInput<bool> (\"SELECT c_custkey, c_name FROM customer ORDER BY c_custkey\", tuple_count=10) : (#0 int32 customer.c_custkey, #1 varchar(25) customer.c_name) order by: {}\n";
    runTest(18, expected_plan);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    ::testing::GTEST_FLAG(filter)=FLAGS_filter;
    int i = RUN_ALL_TESTS();
    google::ShutDownCommandLineFlags();
    return i;

}
