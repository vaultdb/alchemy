#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/filter.h>
#include <query_table/field/field_factory.h>


using namespace emp;
using namespace vaultdb;


class FilterTest : public ::testing::Test {


protected:
    void SetUp() override { setup_plain_prot(false, ""); };
    void TearDown() override{  finalize_plain_prot(); };

    const std::string dbName = "tpch_unioned";
};





TEST_F(FilterTest, test_table_scan) {

    std::string sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem ORDER BY (1), (2) LIMIT 10";

    SqlInput input(dbName, sql, false);

    std::shared_ptr<QueryTable<BoolField> > output = input.run(); // a smoke test for the operator infrastructure
    std::shared_ptr<QueryTable<BoolField> > expected = DataUtilities::getQueryResults(dbName, sql, false);

    ASSERT_EQ(*expected, *output);



}



// unencrypted case
// l_linenumber == 1
class FilterPredicate : public Predicate<BoolField> {
    IntField cmp;
public:
    FilterPredicate() {
          cmp = IntField(1);
    }

    ~FilterPredicate() = default;
    BoolField predicateCall(const QueryTuple<BoolField> & aTuple) const override {

        const IntField *field = static_cast<const IntField *>(aTuple.getField(1));
        return  (*field == cmp);
    }


};



TEST_F(FilterTest, test_filter) {
    std::string sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem ORDER BY (1), (2) LIMIT 10";
    std::string expectedResultSql = "WITH input AS (" + sql + ") SELECT *, l_linenumber<>1 dummy FROM input";
   std::shared_ptr<QueryTable<BoolField> > expected = DataUtilities::getQueryResults(dbName, expectedResultSql, true);

    SqlInput input(dbName, sql, false);

    std::shared_ptr<Predicate<BoolField> > predicateClass(new FilterPredicate());
    Filter<BoolField> filter(&input, predicateClass); // heap allocate it
    //std::shared_ptr<Operator> filter = Operator::getOperatorTree(new Filter(predicateClass, input), input);


    std::shared_ptr<QueryTable<BoolField> > result = filter.run();


    ASSERT_EQ(*expected,  *result);

}

