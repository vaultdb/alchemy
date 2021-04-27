#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <data/CsvReader.h>
#include <parser/plan_parser.h>
#include "support/tpch_queries.h"
#include <boost/algorithm/string/replace.hpp>
#include <boost/tokenizer.hpp>



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
    const string db_name_ = "tpch_unioned"; // plaintext case first
    // limit input to first 100 tuples per SQL statement
    int limit_ = 100;
    string limit_str_ = std::to_string(limit_);

};

// examples (from TPC-H Q1, Q3):
// 0, collation: (0 ASC, 1 ASC)
// 1, collation: (0 ASC, 2 DESC, 3 ASC)
//   (above actually all ASC in tpc-h, DESC for testing)

pair<int, SortDefinition> parseSqlHeader(const string & header) {
    std::cout << "Parsing " << header << std::endl;
    int comma_idx = header.find( ',');
    int operator_id = std::atoi(header.substr(3, comma_idx-3).c_str()); // chop off "-- "

    pair<int, SortDefinition> result;
    result.first = operator_id;
    SortDefinition output_collation;


    if(header.find("collation") != string::npos) {
        int sort_start = header.find('(');
        int sort_end = header.find(')');
        string collation = header.substr(sort_start + 1, sort_end - sort_start - 1);

        boost::tokenizer<boost::escaped_list_separator<char> > tokenizer(collation);
        for(boost::tokenizer<boost::escaped_list_separator<char> >::iterator beg=tokenizer.begin(); beg!=tokenizer.end();++beg) {
            std::cout << "Token: " << *beg << std::endl;
            boost::tokenizer<> sp(*beg); // space delimited
            boost::tokenizer<>::iterator  entries = sp.begin();

            int ordinal = std::atoi(entries->c_str());
            std::string direction = *(++entries);
            assert(direction == "ASC" || direction == "DESC");
            ColumnSort sort(ordinal, (direction == "ASC") ? SortDirection::ASCENDING : SortDirection::DESCENDING);
            output_collation.emplace_back(sort);
        }
    }

    result.second = output_collation;

    return result;
}

TEST_F(PlanParserTest, read_collation_sql) {
    // header in comment on top of SQL statement
    string q1 = "-- 0, collation: (0 ASC, 1 ASC)";
    string q3_orders = "-- 1, collation: (0 ASC, 2 DESC, 3 ASC)"; // actually all ASC in tpc-h, for testing

    parseSqlHeader(q1);
    parseSqlHeader(q3_orders);




}

// sort not really needed in Q1 MPC pipeline, retaining it to demo the hand-off from one op to the next
TEST_F(PlanParserTest, tpch_q1) {
    string test_name = "q1";
    PlanParser<bool> parser(db_name_, test_name, limit_);
    shared_ptr<PlainOperator> root = parser.getRoot();

    string query = tpch_queries[1];
    string limit_query = "(SELECT * FROM lineitem ORDER BY l_returnflag, l_linestatus,  l_orderkey, l_linenumber LIMIT " + limit_str_ + ") lineitem";
    boost::replace_first(query, "lineitem", limit_query);
    // collect expected results
    shared_ptr<PlainTable> expected = DataUtilities::getExpectedResults(db_name_, query, false, 2);

    // failing to run query tree because additional operators are being stored as shared_ptrs.
    /// when we delete PlanParser, we delete the pointers
    shared_ptr<PlainTable> observed = root->run();
    observed = DataUtilities::removeDummies(observed);

    ASSERT_EQ(*expected, *observed);

}


TEST_F(PlanParserTest, tpch_q3) {
    string test_name = "q3";
    string query = tpch_queries[3];

    // set up expected output
    string customer_sql = "(SELECT * FROM customer ORDER BY c_custkey LIMIT " + limit_str_ + ")";

    string orders_sql = "(SELECT * "
                           "FROM orders "
                           "ORDER BY o_orderkey, o_orderdate, o_shippriority " // was o_orderkey, o_custkey, o_orderdate, o_shippriority
                           "LIMIT " + limit_str_ + ")";


    string lineitem_sql = "(SELECT * FROM lineitem ORDER BY l_orderkey, l_shipdate LIMIT " + limit_str_ + ")";

    boost::replace_first(query, "customer", customer_sql);
    boost::replace_first(query, "orders", orders_sql);
    boost::replace_first(query, "lineitem", lineitem_sql);


    shared_ptr<PlainTable> expected = DataUtilities::getExpectedResults(db_name_, query, false, 0);
    // 1 DESC, 2 ASC
    SortDefinition expected_sort{ColumnSort(1, SortDirection::DESCENDING), ColumnSort(2, SortDirection::ASCENDING)};
    expected->setSortOrder(expected_sort);

    // run test
    PlanParser<bool> parser(db_name_, test_name, limit_);
    shared_ptr<PlainOperator> root = parser.getRoot();
    shared_ptr<PlainTable> observed = root->run();
    observed = DataUtilities::removeDummies(observed);

    ASSERT_EQ(*expected, *observed);


}


TEST_F(PlanParserTest, tpch_q5) {
    string test_name = "q5";
    PlanParser<bool> plan_reader(db_name_, test_name, limit_);
}

TEST_F(PlanParserTest, tpch_q8) {
    string test_name = "q8";
    PlanParser<bool> plan_reader(db_name_, test_name, limit_);
}

// q9 expresssion:   l.l_extendedprice * (1 - l.l_discount) - ps.ps_supplycost * l.l_quantity
TEST_F(PlanParserTest, tpch_q9) {
    string test_name = "q9";
    PlanParser<bool> plan_reader(db_name_, test_name, limit_);
}



TEST_F(PlanParserTest, tpch_q18) {
    string test_name = "q18";
    PlanParser<bool> plan_reader(db_name_, test_name, limit_);
}

