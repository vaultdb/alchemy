#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <data/csv_reader.h>
#include <parser/plan_parser.h>
#include "support/tpch_queries.h"
#include <boost/algorithm/string/replace.hpp>




using namespace emp;
using namespace vaultdb;


class PlanParserTest : public ::testing::Test {


protected:
    void SetUp() override{
        setup_plain_prot(false, "");

    };
    void TearDown() override{
        finalize_plain_prot();
    };
    // depends on truncate-tpch.sql
    const string db_name_ = "tpch_unioned"; // plaintext case first
    // limit input to first N tuples per SQL statement
    int limit_ = 10;

    void runTest(const int &test_id, const SortDefinition &expected_sort, const std::string &expected_plan);

};

// most of these runs are not meaningful for diffing the results because they produce no tuples - joins are too sparse.
// This isn't relevant to the parser so work on this elsewhere.
void
PlanParserTest::runTest(const int &test_id, const SortDefinition &expected_sort, const std::string &expected_plan) {
    string test_name = "q" + std::to_string(test_id);
    string query = truncated_tpch_queries[test_id];
    boost::replace_all(query, "$LIMIT", std::to_string(limit_));

    shared_ptr<PlainTable> expected = DataUtilities::getExpectedResults(db_name_, query, false, 0);
    expected->setSortOrder(expected_sort);

    PlanParser<bool> plan_reader(db_name_, test_name, limit_);
    shared_ptr<PlainOperator> root = plan_reader.getRoot();
    std::string observed_plan = root->printTree();

    ASSERT_EQ(expected_plan, observed_plan);



}


TEST_F(PlanParserTest, read_collation_sql) {
    // header in comment on top of SQL statement
    string q1 = "-- 0, collation: (0 ASC, 1 ASC)";
    string q3_orders = "-- 1, collation: (0 ASC, 2 DESC, 3 ASC)"; // actually all ASC in tpc-h, for testing

    pair<int, SortDefinition> q1_header = PlanParser<bool>::parseSqlHeader(q1);
    ASSERT_EQ(q1_header.first, 0);
    ASSERT_EQ(q1_header.second, DataUtilities::getDefaultSortDefinition(2));

    pair<int, SortDefinition> q3_header = PlanParser<bool>::parseSqlHeader(q3_orders);
    SortDefinition q3_sort{ColumnSort(0, SortDirection::ASCENDING),
                           ColumnSort(2, SortDirection::DESCENDING),
                           ColumnSort(3, SortDirection::ASCENDING)};

    ASSERT_EQ(q3_header.first, 1);
    ASSERT_EQ(q3_header.second, q3_sort);


}

// sort not really needed in Q1 MPC pipeline, retaining it to demo the hand-off from one op to the next
TEST_F(PlanParserTest, tpch_q1) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    std::string expected_plan = "Sort<bool> ({<0, ASC> , <1, ASC> )) : (#0 varchar(1) lineitem.l_returnflag, #1 varchar(1) lineitem.l_linestatus, #2 float .sum_qty, #3 float .sum_base_price, #4 float .sum_disc_price, #5 float .sum_charge, #6 float .avg_qty, #7 float .avg_price, #8 float .avg_disc, #9 int64 .count_order) order by: {<0, ASC> , <1, ASC> )\n"
                                "    GroupByAggregate<bool> (group-by: (0, 1) aggs: (SUM($2) sum_qty, SUM($3) sum_base_price, SUM($4) sum_disc_price, SUM($5) sum_charge, AVG($2) avg_qty, AVG($3) avg_price, AVG($6) avg_disc, COUNT(*) count_order)) : (#0 varchar(1) lineitem.l_returnflag, #1 varchar(1) lineitem.l_linestatus, #2 float .sum_qty, #3 float .sum_base_price, #4 float .sum_disc_price, #5 float .sum_charge, #6 float .avg_qty, #7 float .avg_price, #8 float .avg_disc, #9 int64 .count_order) order by: {<0, ASC> , <1, ASC> )\n"
                                "        SqlInput<bool> (\"SELECT * FROM (SELECT l_returnflag, l_linestatus, l_quantity, l_extendedprice, l_extendedprice * (1 - l_discount) AS discount, l_extendedprice * (1 - l_discount) * (1 + l_tax) AS charge, l_discount, NOT l_shipdate <= DATE '1998-08-03' AS dummy_tag FROM lineitem ORDER BY l_returnflag, l_linestatus ) input LIMIT 10\", tuple_count=10) : (#0 varchar(1) lineitem.l_returnflag, #1 varchar(1) lineitem.l_linestatus, #2 float lineitem.l_quantity, #3 float lineitem.l_extendedprice, #4 float .discount, #5 float .charge, #6 float lineitem.l_discount) order by: {<0, ASC> , <1, ASC> ) tuple count: 10\n";

    runTest(1, expected_sort, expected_plan);
}


TEST_F(PlanParserTest, tpch_q3) {
    // dummy_tag (-1), 1 DESC, 2 ASC
    // aka revenue desc,  o.o_orderdate
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING)};

    std::string expected_plan = "Project<bool> ((<0, $0 int32>, <1, $3 float>, <2, $1 int64>, <3, $2 int32>)) : (#0 int32 lineitem.l_orderkey, #1 float .revenue, #2 int64 .o_orderdate, #3 int32 orders.o_shippriority) order by: {<-1, ASC> , <1, DESC> , <2, ASC> )\n"
                                "    Sort<bool> ({<-1, ASC> , <3, DESC> , <1, ASC> )) : (#0 int32 lineitem.l_orderkey, #1 int64 .o_orderdate, #2 int32 orders.o_shippriority, #3 float .revenue) order by: {<-1, ASC> , <3, DESC> , <1, ASC> )\n"
                                "        GroupByAggregate<bool> (group-by: (0, 1, 2) aggs: (SUM($3) revenue)) : (#0 int32 lineitem.l_orderkey, #1 int64 .o_orderdate, #2 int32 orders.o_shippriority, #3 float .revenue) order by: {<0, ASC> , <1, ASC> , <2, ASC> )\n"
                                "            Sort<bool> ({<0, ASC> , <1, ASC> , <2, ASC> )) : (#0 int32 lineitem.l_orderkey, #1 int64 .o_orderdate, #2 int32 orders.o_shippriority, #3 float .revenue) order by: {<0, ASC> , <1, ASC> , <2, ASC> )\n"
                                "                Project<bool> ((<0, $6 int32>, <1, $4 int64>, <2, $5 int32>, <3, $8 float>)) : (#0 int32 lineitem.l_orderkey, #1 int64 .o_orderdate, #2 int32 orders.o_shippriority, #3 float .revenue) order by: {)\n"
                                "                    KeyedJoin<bool> ($6 == $2) : (#0 int32 customer.c_custkey, #1 varchar(10) customer.c_mktsegment, #2 int32 orders.o_orderkey, #3 int32 orders.o_custkey, #4 int64 .o_orderdate, #5 int32 orders.o_shippriority, #6 int32 lineitem.l_orderkey, #7 int64 .l_shipdate, #8 float .revenue) order by: {)\n"
                                "                        KeyedJoin<bool> ($0 == $3) : (#0 int32 customer.c_custkey, #1 varchar(10) customer.c_mktsegment, #2 int32 orders.o_orderkey, #3 int32 orders.o_custkey, #4 int64 .o_orderdate, #5 int32 orders.o_shippriority) order by: {)\n"
                                "                            SqlInput<bool> (\"SELECT * FROM (SELECT c_custkey, c_mktsegment, NOT c_mktsegment = 'HOUSEHOLD ' AS dummy_tag FROM customer ORDER BY c_custkey ) input LIMIT 10\", tuple_count=10) : (#0 int32 customer.c_custkey, #1 varchar(10) customer.c_mktsegment) order by: {<0, ASC> ) tuple count: 10\n"
                                "                            SqlInput<bool> (\"SELECT * FROM (SELECT o_orderkey, o_custkey, o_orderdate, o_shippriority, NOT o_orderdate < DATE '1995-03-25' AS dummy_tag FROM orders ORDER BY o_orderkey, o_orderdate, o_shippriority ) input LIMIT 10\", tuple_count=10) : (#0 int32 orders.o_orderkey, #1 int32 orders.o_custkey, #2 int64 .o_orderdate, #3 int32 orders.o_shippriority) order by: {<0, ASC> , <2, ASC> , <3, ASC> ) tuple count: 10\n"
                                "                        SqlInput<bool> (\"SELECT * FROM (SELECT l_orderkey, l_shipdate, l_extendedprice * (1 - l_discount) AS revenue, NOT l_shipdate > DATE '1995-03-25' AS dummy_tag FROM lineitem ORDER BY l_orderkey, l_shipdate ) input LIMIT 10\", tuple_count=10) : (#0 int32 lineitem.l_orderkey, #1 int64 .l_shipdate, #2 float .revenue) order by: {<0, ASC> , <1, ASC> ) tuple count: 10\n";

    runTest(3, expected_sort, expected_plan);
}


TEST_F(PlanParserTest, tpch_q5) {
    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    std::string expected_plan = "Sort<bool> ({<1, DESC> )) : (#0 varchar(25) nation.n_name, #1 float .revenue) order by: {<1, DESC> )\n"
                                "    GroupByAggregate<bool> (group-by: (0) aggs: (SUM($1) revenue)) : (#0 varchar(25) nation.n_name, #1 float .revenue) order by: {<0, ASC> )\n"
                                "        Sort<bool> ({<0, ASC> )) : (#0 varchar(25) nation.n_name, #1 float .anonymous) order by: {<0, ASC> )\n"
                                "            Project<bool> ((<0, $0 varchar>, <1, $8 * 1.000000 - $9 float>)) : (#0 varchar(25) nation.n_name, #1 float .anonymous) order by: {)\n"
                                "                KeyedJoin<bool> ($7 == $10 AND $2 == $11) : (#0 varchar(25) nation.n_name, #1 int32 customer.c_custkey, #2 int32 customer.c_nationkey, #3 int32 orders.o_orderkey, #4 int32 orders.o_custkey, #5 int64 .o_orderdate, #6 int32 lineitem.l_orderkey, #7 int32 lineitem.l_suppkey, #8 float lineitem.l_extendedprice, #9 float lineitem.l_discount, #10 int32 supplier.s_suppkey, #11 int32 supplier.s_nationkey) order by: {)\n"
                                "                    KeyedJoin<bool> ($6 == $3) : (#0 varchar(25) nation.n_name, #1 int32 customer.c_custkey, #2 int32 customer.c_nationkey, #3 int32 orders.o_orderkey, #4 int32 orders.o_custkey, #5 int64 .o_orderdate, #6 int32 lineitem.l_orderkey, #7 int32 lineitem.l_suppkey, #8 float lineitem.l_extendedprice, #9 float lineitem.l_discount) order by: {)\n"
                                "                        KeyedJoin<bool> ($1 == $4) : (#0 varchar(25) nation.n_name, #1 int32 customer.c_custkey, #2 int32 customer.c_nationkey, #3 int32 orders.o_orderkey, #4 int32 orders.o_custkey, #5 int64 .o_orderdate) order by: {)\n"
                                "                            SqlInput<bool> (\"SELECT * FROM (SELECT t1.n_name, t2.c_custkey, t2.c_nationkey FROM (SELECT r_regionkey, r_name FROM region WHERE r_name = 'EUROPE') AS t0 INNER JOIN (SELECT n_nationkey, n_name, n_regionkey FROM nation) AS t1 ON t0.r_regionkey = t1.n_regionkey INNER JOIN (SELECT c_custkey, c_nationkey FROM customer) AS t2 ON t1.n_nationkey = t2.c_nationkey ORDER BY t2.c_custkey ASC ) input LIMIT 10\", tuple_count=10) : (#0 varchar(25) nation.n_name, #1 int32 customer.c_custkey, #2 int32 customer.c_nationkey) order by: {<1, ASC> ) tuple count: 10\n"
                                "                            SqlInput<bool> (\"SELECT * FROM (SELECT o_orderkey, o_custkey, o_orderdate, NOT (o_orderdate >= DATE '1997-01-01' AND o_orderdate < DATE '1998-01-01') AS dummy_tag FROM orders ORDER BY o_orderkey, o_custkey, o_orderdate ) input LIMIT 10\", tuple_count=10) : (#0 int32 orders.o_orderkey, #1 int32 orders.o_custkey, #2 int64 .o_orderdate) order by: {<0, ASC> , <1, ASC> , <2, ASC> ) tuple count: 10\n"
                                "                        SqlInput<bool> (\"SELECT * FROM (SELECT l_orderkey, l_suppkey, l_extendedprice, l_discount FROM lineitem ORDER BY l_orderkey, l_suppkey, l_extendedprice, l_discount ) input LIMIT 10\", tuple_count=10) : (#0 int32 lineitem.l_orderkey, #1 int32 lineitem.l_suppkey, #2 float lineitem.l_extendedprice, #3 float lineitem.l_discount) order by: {<0, ASC> , <1, ASC> , <2, ASC> , <3, ASC> ) tuple count: 10\n"
                                "                    SqlInput<bool> (\"SELECT * FROM (SELECT t2.s_suppkey, t2.s_nationkey FROM (SELECT r_regionkey, r_name FROM region WHERE r_name = 'EUROPE') AS t0 INNER JOIN (SELECT n_nationkey, n_regionkey FROM nation) AS t1 ON t0.r_regionkey = t1.n_regionkey INNER JOIN (SELECT s_suppkey, s_nationkey FROM supplier) AS t2 ON t1.n_nationkey = t2.s_nationkey ORDER BY s_suppkey ) input LIMIT 10\", tuple_count=10) : (#0 int32 supplier.s_suppkey, #1 int32 supplier.s_nationkey) order by: {<0, ASC> ) tuple count: 10\n";
    runTest(5, expected_sort, expected_plan);
}

TEST_F(PlanParserTest, tpch_q8) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    std::string expected_plan = "Project<bool> ((<0, $0 int32>, <1, $1 / $2 float>)) : (#0 int32 orders.o_year, #1 float .anonymous) order by: {<0, ASC> )\n"
                                "    Sort<bool> ({<0, ASC> )) : (#0 int32 orders.o_year, #1 float .null, #2 float .null) order by: {<0, ASC> )\n"
                                "        GroupByAggregate<bool> (group-by: (0) aggs: (SUM($1) null, SUM($2) null)) : (#0 int32 orders.o_year, #1 float .null, #2 float .null) order by: {<0, ASC> )\n"
                                "            Sort<bool> ({<0, ASC> )) : (#0 int32 orders.o_year, #1 float .anonymous, #2 float .volume) order by: {<0, ASC> )\n"
                                "                Project<bool> ((<0, $4 int32>, <1, $8 * $10 float>, <2, $8 float>)) : (#0 int32 orders.o_year, #1 float .anonymous, #2 float .volume) order by: {)\n"
                                "                    KeyedJoin<bool> ($2 == $6) : (#0 int32 customer.c_nationkey, #1 int32 customer.c_custkey, #2 int32 orders.o_orderkey, #3 int32 orders.o_custkey, #4 int32 orders.o_year, #5 int32 lineitem.l_partkey, #6 int32 lineitem.l_orderkey, #7 int32 lineitem.l_suppkey, #8 float .volume, #9 int32 supplier.s_suppkey, #10 float .nation_check) order by: {)\n"
                                "                        KeyedJoin<bool> ($3 == $1) : (#0 int32 customer.c_nationkey, #1 int32 customer.c_custkey, #2 int32 orders.o_orderkey, #3 int32 orders.o_custkey, #4 int32 orders.o_year) order by: {)\n"
                                "                            SqlInput<bool> (\"SELECT * FROM (SELECT c_nationkey, c_custkey FROM customer c JOIN nation n1 ON c_nationkey = n_nationkey                 JOIN region ON r_regionkey = n_regionkey WHERE r_name = 'MIDDLE EAST' ORDER BY c_custkey ) input LIMIT 10\", tuple_count=10) : (#0 int32 customer.c_nationkey, #1 int32 customer.c_custkey) order by: {<1, ASC> ) tuple count: 10\n"
                                "                            SqlInput<bool> (\"SELECT * FROM (SELECT o_orderkey, o_custkey, o_orderyear::INT AS o_year, NOT (o_orderdate >= DATE '1995-01-01' AND o_orderdate <= DATE '1996-12-31') AS dummy_tag FROM orders ORDER BY o_custkey, o_orderkey ) input LIMIT 10\", tuple_count=10) : (#0 int32 orders.o_orderkey, #1 int32 orders.o_custkey, #2 int32 orders.o_year) order by: {<1, ASC> , <0, ASC> ) tuple count: 10\n"
                                "                        KeyedJoin<bool> ($4 == $2) : (#0 int32 lineitem.l_partkey, #1 int32 lineitem.l_orderkey, #2 int32 lineitem.l_suppkey, #3 float .volume, #4 int32 supplier.s_suppkey, #5 float .nation_check) order by: {)\n"
                                "                            SqlInput<bool> (\"SELECT * FROM (SELECT l_partkey, l_orderkey, l_suppkey, l_extendedprice * (1.0 - l_discount) volume FROM part JOIN lineitem ON p_partkey = l_partkey WHERE p_type = 'PROMO BRUSHED COPPER' ORDER BY l_orderkey ) input LIMIT 10\", tuple_count=10) : (#0 int32 lineitem.l_partkey, #1 int32 lineitem.l_orderkey, #2 int32 lineitem.l_suppkey, #3 float .volume) order by: {<1, ASC> ) tuple count: 10\n"
                                "                            SqlInput<bool> (\"SELECT * FROM (SELECT s_suppkey, CASE WHEN n_name = 'EGYPT' THEN 1.0 ELSE 0.0 END AS nation_check FROM supplier JOIN nation ON n_nationkey = s_nationkey ORDER BY s_suppkey ) input LIMIT 10\", tuple_count=10) : (#0 int32 supplier.s_suppkey, #1 float .nation_check) order by: {<0, ASC> ) tuple count: 10\n";
    runTest(8, expected_sort, expected_plan);
}

// q9 expresssion:   l.l_extendedprice * (1 - l.l_discount) - ps.ps_supplycost * l.l_quantity
TEST_F(PlanParserTest, tpch_q9) {
    // $0 ASC, $1 DESC
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};

    std::string expected_plan = "Sort<bool> ({<0, ASC> , <1, DESC> )) : (#0 varchar(25) nation.n_name, #1 int32 orders.o_year, #2 float .sum_profit) order by: {<0, ASC> , <1, DESC> )\n"
                                "    GroupByAggregate<bool> (group-by: (0, 1) aggs: (SUM($2) sum_profit)) : (#0 varchar(25) nation.n_name, #1 int32 orders.o_year, #2 float .sum_profit) order by: {<0, ASC> , <1, ASC> )\n"
                                "        Sort<bool> ({<0, ASC> , <1, ASC> )) : (#0 varchar(25) nation.n_name, #1 int32 orders.o_year, #2 float .anonymous) order by: {<0, ASC> , <1, ASC> )\n"
                                "            Project<bool> ((<0, $0 varchar>, <1, $10 int32>, <2, $8 * 1.000000 - $9 - $3 * $7 float>)) : (#0 varchar(25) nation.n_name, #1 int32 orders.o_year, #2 float .anonymous) order by: {)\n"
                                "                KeyedJoin<bool> ($11 == $4) : (#0 varchar(25) nation.n_name, #1 int32 partsupp.ps_suppkey, #2 int32 partsupp.ps_partkey, #3 float partsupp.ps_supplycost, #4 int32 lineitem.l_orderkey, #5 int32 lineitem.l_partkey, #6 int32 lineitem.l_suppkey, #7 float .l_quantity, #8 float lineitem.l_extendedprice, #9 float lineitem.l_discount, #10 int32 orders.o_year, #11 int32 orders.o_orderkey) order by: {)\n"
                                "                    KeyedJoin<bool> ($1 == $6 AND $2 == $5) : (#0 varchar(25) nation.n_name, #1 int32 partsupp.ps_suppkey, #2 int32 partsupp.ps_partkey, #3 float partsupp.ps_supplycost, #4 int32 lineitem.l_orderkey, #5 int32 lineitem.l_partkey, #6 int32 lineitem.l_suppkey, #7 float .l_quantity, #8 float lineitem.l_extendedprice, #9 float lineitem.l_discount) order by: {)\n"
                                "                        SqlInput<bool> (\"SELECT * FROM (SELECT n_name,  ps_suppkey, ps_partkey, ps_supplycost FROM nation JOIN supplier ON s_nationkey = n_nationkey     JOIN partsupp ON s_suppkey = ps_suppkey     JOIN part ON ps_partkey = p_partkey WHERE p_name LIKE '%yellow%' ORDER BY ps_suppkey, ps_partkey ) input LIMIT 10\", tuple_count=10) : (#0 varchar(25) nation.n_name, #1 int32 partsupp.ps_suppkey, #2 int32 partsupp.ps_partkey, #3 float partsupp.ps_supplycost) order by: {<1, ASC> , <2, ASC> ) tuple count: 10\n"
                                "                        SqlInput<bool> (\"SELECT * FROM (SELECT l_orderkey, l_partkey, l_suppkey, l_quantity::float4, l_extendedprice, l_discount FROM lineitem ORDER BY l_orderkey, l_partkey, l_suppkey ) input LIMIT 10\", tuple_count=10) : (#0 int32 lineitem.l_orderkey, #1 int32 lineitem.l_partkey, #2 int32 lineitem.l_suppkey, #3 float .l_quantity, #4 float lineitem.l_extendedprice, #5 float lineitem.l_discount) order by: {<1, ASC> , <2, ASC> , <3, ASC> ) tuple count: 10\n"
                                "                    SqlInput<bool> (\"SELECT * FROM (SELECT o_orderyear AS o_year, o_orderkey FROM orders ORDER BY o_orderkey ) input LIMIT 10\", tuple_count=10) : (#0 int32 orders.o_year, #1 int32 orders.o_orderkey) order by: {<1, ASC> ) tuple count: 10\n";
    runTest(9, expected_sort, expected_plan);

}



TEST_F(PlanParserTest, tpch_q18) {
    // -1 ASC, $4 DESC, $3 ASC
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
        ColumnSort(4, SortDirection::DESCENDING),
        ColumnSort(3, SortDirection::ASCENDING)};

    std::string expected_plan = "Sort<bool> ({<-1, ASC> , <4, DESC> , <3, ASC> )) : (#0 varchar(25) customer.c_name, #1 int32 customer.c_custkey, #2 int32 orders.o_orderkey, #3 int64 .o_orderdate, #4 float orders.o_totalprice, #5 float .sum_qty) order by: {<-1, ASC> , <4, DESC> , <3, ASC> )\n"
                                "    GroupByAggregate<bool> (group-by: (0, 1, 2, 3, 4) aggs: (SUM($5) sum_qty)) : (#0 varchar(25) customer.c_name, #1 int32 customer.c_custkey, #2 int32 orders.o_orderkey, #3 int64 .o_orderdate, #4 float orders.o_totalprice, #5 float .sum_qty) order by: {<0, ASC> , <1, ASC> , <2, ASC> , <3, ASC> , <4, ASC> )\n"
                                "        Sort<bool> ({<0, ASC> , <1, ASC> , <2, ASC> , <3, ASC> , <4, ASC> )) : (#0 varchar(25) customer.c_name, #1 int32 customer.c_custkey, #2 int32 orders.o_orderkey, #3 int64 .o_orderdate, #4 float orders.o_totalprice, #5 float lineitem.l_quantity) order by: {<0, ASC> , <1, ASC> , <2, ASC> , <3, ASC> , <4, ASC> )\n"
                                "            Project<bool> ((<0, $7 varchar>, <1, $6 int32>, <2, $2 int32>, <3, $4 int64>, <4, $5 float>, <5, $1 float>)) : (#0 varchar(25) customer.c_name, #1 int32 customer.c_custkey, #2 int32 orders.o_orderkey, #3 int64 .o_orderdate, #4 float orders.o_totalprice, #5 float lineitem.l_quantity) order by: {)\n"
                                "                KeyedJoin<bool> ($6 == $3) : (#0 int32 lineitem.l_orderkey, #1 float lineitem.l_quantity, #2 int32 orders.o_orderkey, #3 int32 orders.o_custkey, #4 int64 .o_orderdate, #5 float orders.o_totalprice, #6 int32 customer.c_custkey, #7 varchar(25) customer.c_name) order by: {)\n"
                                "                    KeyedJoin<bool> ($0 == $2) : (#0 int32 lineitem.l_orderkey, #1 float lineitem.l_quantity, #2 int32 orders.o_orderkey, #3 int32 orders.o_custkey, #4 int64 .o_orderdate, #5 float orders.o_totalprice) order by: {)\n"
                                "                        SqlInput<bool> (\"SELECT * FROM (SELECT l_orderkey, l_quantity FROM lineitem ORDER BY l_orderkey, l_quantity ) input LIMIT 10\", tuple_count=10) : (#0 int32 lineitem.l_orderkey, #1 float lineitem.l_quantity) order by: {<0, ASC> , <1, ASC> ) tuple count: 10\n"
                                "                        Project<bool> ((<0, $0 int32>, <1, $1 int32>, <2, $3 int64>, <3, $2 float>)) : (#0 int32 orders.o_orderkey, #1 int32 orders.o_custkey, #2 int64 .o_orderdate, #3 float orders.o_totalprice) order by: {)\n"
                                "                            KeyedJoin<bool> ($0 == $4) : (#0 int32 orders.o_orderkey, #1 int32 orders.o_custkey, #2 float orders.o_totalprice, #3 int64 .o_orderdate, #4 int32 orders.o_orderkey) order by: {)\n"
                                "                                SqlInput<bool> (\"SELECT * FROM (SELECT o_orderkey, o_custkey, o_totalprice, o_orderdate FROM orders ORDER BY o_orderkey, o_custkey ) input LIMIT 10\", tuple_count=10) : (#0 int32 orders.o_orderkey, #1 int32 orders.o_custkey, #2 float orders.o_totalprice, #3 int64 .o_orderdate) order by: {<0, ASC> , <1, ASC> ) tuple count: 10\n"
                                "                                Project<bool> ((<0, $0 int32>)) : (#0 int32 orders.o_orderkey) order by: {<0, ASC> )\n"
                                "                                    Filter<bool> ($1 > 313) : (#0 int32 orders.o_orderkey, #1 int64 .null) order by: {<0, ASC> , <1, ASC> )\n"
                                "                                        GroupByAggregate<bool> (group-by: (0) aggs: (SUM($1) null)) : (#0 int32 orders.o_orderkey, #1 int64 .null) order by: {<0, ASC> , <1, ASC> )\n"
                                "                                            SqlInput<bool> (\"SELECT * FROM (SELECT o_orderkey, o_custkey, o_totalprice, o_orderdate FROM orders ORDER BY o_orderkey, o_custkey ) input LIMIT 10\", tuple_count=10) : (#0 int32 orders.o_orderkey, #1 int32 orders.o_custkey, #2 float orders.o_totalprice, #3 int64 .o_orderdate) order by: {<0, ASC> , <1, ASC> ) tuple count: 10\n"
                                "                    SqlInput<bool> (\"SELECT * FROM (SELECT c_custkey, c_name FROM customer ORDER BY c_custkey ) input LIMIT 10\", tuple_count=10) : (#0 int32 customer.c_custkey, #1 varchar(25) customer.c_name) order by: {<0, ASC> ) tuple count: 10\n";

    runTest(18, expected_sort, expected_plan);
}

