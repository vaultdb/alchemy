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
                                 "(85090, 6, O)\n"
                                 "(373158, 5, F)\n"
                                 "(1028, 7, F)\n"
                                 "(435171, 1, O)\n"
                                 "(338759, 2, F)\n"
                                 "(486753, 2, O)\n"
                                 "(39460, 1, F)\n"
                                 "(345252, 1, O)\n"
                                 "(455171, 1, F)\n"
                                 "(523878, 4, O)\n";

    std::shared_ptr<Operator> input = std::make_shared<SqlInput>("tpch_alice", sql, false);
    std::shared_ptr<QueryTable> output = input->run();

    std::cout << *output << std::endl;

    ASSERT_EQ(expectedOutput, output->toString());



}



// unencrypted case
class FilterPredicate : public PredicateClass {
    Value cmp;
public:
    FilterPredicate() {
         cmp = Value((int32_t) 1);
    }

    ~FilterPredicate() {}
    types::Value predicateCall(const QueryTuple & aTuple) const override {

        Value field = aTuple.getField(1).getValue();
        Value res = (field == cmp);
        return  !res;  // (!) because dummy is false if our selection criteria is satisfied
    }


};



TEST_F(FilterTest, test_filter) {
    std::string sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem ORDER BY l_comment LIMIT 10";
    std::string expectedOutput = "(#0 int32 lineitem.l_orderkey, #1 int32 lineitem.l_linenumber, #2 varchar(1) lineitem.l_linestatus) isEncrypted? 0\n"
                                 "(435171, 1, O)\n"
                                 "(39460, 1, F)\n"
                                 "(345252, 1, O)\n"
                                 "(455171, 1, F)\n";

    std::shared_ptr<Operator> input(new SqlInput("tpch_alice", sql, false));

    // TODO: fix the warnings associated with this
    std::shared_ptr<PredicateClass> predicateClass(new FilterPredicate());
    Filter *filterOp = new Filter(predicateClass, input); // heap allocate it
    std::shared_ptr<Operator> filter = filterOp->getPtr();

    std::shared_ptr<QueryTable> result = filter->run();
    std::cout << "Result: " << *result << std::endl;


    ASSERT_EQ(expectedOutput,  result->toString());

}

