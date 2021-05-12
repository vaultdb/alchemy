#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <data/CsvReader.h>
#include <parser/plan_parser.h>
#include "support/tpch_queries.h"
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/replace.hpp>




using namespace emp;
using namespace vaultdb;


// create a temp DB that's already truncated to the "correct" sizes for our test
// then undo it during teardown
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
    int limit_ = 10; // TODO: better manage relationships between tables so that all tests have an expected output that's not empty

    void runTest(const int & test_id, const SortDefinition & expected_sort);

};

void PlanParserTest::runTest(const int & test_id, const SortDefinition & expected_sort) {
    string test_name = "q" + std::to_string(test_id);
    string query = truncated_tpch_queries[test_id];
    boost::replace_all(query, "$LIMIT", std::to_string(limit_));
    std::cout << "Query: " << query << std::endl;

    shared_ptr<PlainTable> expected = DataUtilities::getExpectedResults(db_name_, query, false, 0);
    expected->setSortOrder(expected_sort);

    PlanParser<bool> plan_reader(db_name_, test_name, limit_);
    shared_ptr<PlainOperator> root = plan_reader.getRoot();
    std::cout << "Parsed plan: " << *root << std::endl;

    shared_ptr<PlainTable> observed = root->run();
    observed = DataUtilities::removeDummies(observed);

    ASSERT_EQ(*expected, *observed);

}

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
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(2);
    runTest(1, expected_sort);
}


TEST_F(PlanParserTest, tpch_q3) {
    // dummy_tag (-1), 1 DESC, 2 ASC
    // aka revenue desc,  o.o_orderdate
    SortDefinition expected_sort{ColumnSort(-1, SortDirection::ASCENDING),
                                 ColumnSort(1, SortDirection::DESCENDING),
                                 ColumnSort(2, SortDirection::ASCENDING)};

    runTest(3, expected_sort);
}


TEST_F(PlanParserTest, tpch_q5) {
    SortDefinition  expected_sort{ColumnSort(1, SortDirection::DESCENDING)};
    runTest(5, expected_sort);
}

TEST_F(PlanParserTest, tpch_q8) {
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(1);
    runTest(8, expected_sort);
}

// q9 expresssion:   l.l_extendedprice * (1 - l.l_discount) - ps.ps_supplycost * l.l_quantity
TEST_F(PlanParserTest, tpch_q9) {
    // $0 ASC, $1 DESC
    SortDefinition  expected_sort{ColumnSort(0, SortDirection::ASCENDING), ColumnSort(1, SortDirection::DESCENDING)};
    runTest(9, expected_sort);

}



TEST_F(PlanParserTest, tpch_q18) {
    // $4 DESC, $3 ASC
    SortDefinition expected_sort{ColumnSort(4, SortDirection::DESCENDING), ColumnSort(3, SortDirection::ASCENDING)};
    runTest(18, expected_sort);
}

