#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/filter.h>


using namespace emp;
using namespace vaultdb::types;



class FilterTest : public ::testing::Test {


protected:
    void SetUp() override {};
    void TearDown() override{};
    const std::string dbName = "tpch_alice";
};





TEST_F(FilterTest, test_table_scan) {

    std::string sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem ORDER BY l_comment LIMIT 10";


    std::shared_ptr<Operator> input = std::make_shared<SqlInput>(dbName, sql, false);
    std::shared_ptr<QueryTable> output = input->run(); // a smoke test for the operator infrastructure
    std::shared_ptr<QueryTable> expected = DataUtilities::getQueryResults(dbName, sql, false);

    std::cout << *output << std::endl;

    ASSERT_EQ(*expected, *output);



}



// unencrypted case
// l_linenumber == 1
class FilterPredicate : public Predicate {
    Value cmp;
public:
    FilterPredicate() {
         cmp = Value((int32_t) 1);
    }

    ~FilterPredicate() {}
    types::Value predicateCall(const QueryTuple & aTuple) const override {

        Value field = aTuple.getField(1).getValue();
        Value res = (field == cmp);
        return  res;
    }


};



TEST_F(FilterTest, test_filter) {
    std::string sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem ORDER BY l_comment LIMIT 10";
    std::string expectedResultSql = "WITH input AS (" + sql + ") SELECT *, l_linenumber<>1 dummy FROM input";
   std::shared_ptr<QueryTable> expected = DataUtilities::getQueryResults(dbName, expectedResultSql, true);

    std::shared_ptr<Operator> input(new SqlInput(dbName, sql, false));

    // TODO: fix the warnings associated with this
    std::shared_ptr<Predicate> predicateClass(new FilterPredicate());
    Filter *filterOp = new Filter(predicateClass, input); // heap allocate it
    std::shared_ptr<Operator> filter = filterOp->getPtr();

    std::shared_ptr<QueryTable> result = filter->run();

    ASSERT_EQ(*expected,  *result);

}

