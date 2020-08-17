//
// Created by Jennie Rogers on 8/16/20.
//

#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/filter.h>


using namespace emp;
using namespace vaultdb::types;



DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54321, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");

class FilterTest : public ::testing::Test {


protected:
    void SetUp() override {};
    void TearDown() override{};
};





TEST_F(FilterTest, test_table_scan) {

    std::string sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem ORDER BY l_comment LIMIT 10";
    std::string expectedOutput = "(#0 int32 lineitem.l_orderkey, #1 int32 lineitem.l_linenumber, #2 varchar(1) lineitem.l_linestatus) isEncrypted? 0\n"
                                 "(85090, 6, O) (dummy=false)\n"
                                 "(373158, 5, F) (dummy=false)\n"
                                 "(1028, 7, F) (dummy=false)\n"
                                 "(435171, 1, O) (dummy=false)\n"
                                 "(338759, 2, F) (dummy=false)\n"
                                 "(486753, 2, O) (dummy=false)\n"
                                 "(39460, 1, F) (dummy=false)\n"
                                 "(345252, 1, O) (dummy=false)\n"
                                 "(455171, 1, F) (dummy=false)\n"
                                 "(523878, 4, O) (dummy=false)";

    SqlInput input("tpch_alice", sql, false);
    std::shared_ptr<QueryTable> output = input.run();

    std::cout << *output << std::endl;

    ASSERT_EQ(expectedOutput, output->toString());



}


// filtering for l_linenumber = 1
// TODO: extend this to be part of a class instance so we can encrypt the literal just once
Value predicateCall(const QueryTuple & aTuple) {
    Value cmp = Value((int32_t) 1);
    Value field = *(aTuple.GetField(1)->GetValue());
    return  field == cmp;
}

TEST_F(FilterTest, test_filter) {
    std::string sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem ORDER BY l_comment LIMIT 10";
    std::string expectedOutput = "(#0 int32 lineitem.l_orderkey, #1 int32 lineitem.l_linenumber, #2 varchar(1) lineitem.l_linestatus) isEncrypted? 0\n"
                                 "(85090, 6, O) (dummy=false)\n"
                                 "(373158, 5, F) (dummy=false)\n"
                                 "(1028, 7, F) (dummy=false)\n"
                                 "(338759, 2, F) (dummy=false)\n"
                                 "(486753, 2, O) (dummy=false)\n"
                                 "(523878, 4, O) (dummy=false)\n";


    std::shared_ptr<Operator> input = std::make_shared<SqlInput>("tpch_alice", sql, false);
    //     Filter(types::Value(*predicateFunction)(const QueryTuple & tuple), std::shared_ptr<Operator> child);
    std::shared_ptr<Operator> filter(new Filter(&predicateCall, input));

    std::shared_ptr<QueryTable> result = filter->run();
    std::cout << "Result: " << *result << std::endl;

    std::cout << "Filter addr: " << &filter << std::endl;

    ASSERT_EQ(expectedOutput,  result->toString());

}

