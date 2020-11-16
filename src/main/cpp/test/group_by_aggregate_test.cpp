#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/filter.h>
#include <operators/support/aggregate_id.h>
#include <operators/group_by_aggregate.h>


using namespace emp;
using namespace vaultdb::types;



class GroupByAggregateTest : public ::testing::Test {


protected:
    void SetUp() override {};
    void TearDown() override{};
    const std::string dbName = "tpch_unioned";
};


TEST_F(GroupByAggregateTest, test_count) {
    // produces 25 rows
    std::string query = "SELECT l_orderkey, l_linenumber FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";
    std::string expectedOutputQuery = "SELECT l_orderkey, COUNT(*) FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";

    SortDefinition sortDefinition = DataUtilities::getDefaultSortDefinition(2);
    std::shared_ptr<Operator> input(new SqlInput(dbName, query, false, sortDefinition));

    std::vector<ScalarAggregateDefinition> aggregators;
    aggregators.push_back(ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt"));

    std::vector<int32_t> groupByCols;
    groupByCols.push_back(0);

    GroupByAggregate *aggregateOp = new GroupByAggregate(input, groupByCols, aggregators);
    std::shared_ptr<Operator> aggregate = aggregateOp->getPtr();

    std::shared_ptr<QueryTable> aggregated = aggregate->run();
    std::shared_ptr<QueryTable> expected = DataUtilities::getQueryResults(dbName, expectedOutputQuery, false);
    SortDefinition expectedSortOrder;
    expectedSortOrder.push_back(ColumnSort (0, SortDirection::ASCENDING));
    expected->setSortOrder(expectedSortOrder);

    // need to delete dummies from observed output to compare it to expected
    std::shared_ptr<QueryTable> observed = DataUtilities::removeDummies(aggregated);

    ASSERT_EQ(*expected, *observed);


}