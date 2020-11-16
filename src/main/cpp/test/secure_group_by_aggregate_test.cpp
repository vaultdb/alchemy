#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/support/aggregate_id.h>
#include <operators/group_by_aggregate.h>
#include <test/support/EmpBaseTest.h>
#include <operators/secure_sql_input.h>
#include <operators/sort.h>

using namespace emp;
using namespace vaultdb::types;


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 43439, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "hostname for execution");
DEFINE_bool(input, false, "input value");


class SecureGroupByAggregateTest : public EmpBaseTest {};


TEST_F(SecureGroupByAggregateTest, test_count) {
    // produces 25 rows
    std::string query = "SELECT l_orderkey, l_linenumber FROM lineitem WHERE l_orderkey <=10 ORDER BY (1), (2)";

    // set up expected output
    std::string expectedOutputQuery = "SELECT l_orderkey, COUNT(*) cnt FROM lineitem WHERE l_orderkey <= 10 GROUP BY l_orderkey ORDER BY (1)";
    std::shared_ptr<QueryTable> expected = DataUtilities::getQueryResults(unionedDb, expectedOutputQuery, false);
    SortDefinition expectedSortOrder;
    expectedSortOrder.push_back(ColumnSort (0, SortDirection::ASCENDING));
    expected->setSortOrder(expectedSortOrder);

    // run secure query
    SortDefinition sortDefinition = DataUtilities::getDefaultSortDefinition(2);
    std::shared_ptr<Operator> input(new SecureSqlInput(dbName, query, false, sortDefinition, netio, FLAGS_party));

    // sort alice + bob inputs after union
    Sort *sortOp = new Sort(sortDefinition, input);
    std::shared_ptr<Operator> sort = sortOp->getPtr();

    std::vector<ScalarAggregateDefinition> aggregators;
    aggregators.push_back(ScalarAggregateDefinition(-1, AggregateId::COUNT, "cnt"));

    std::vector<int32_t> groupByCols;
    groupByCols.push_back(0);

    GroupByAggregate *aggregateOp = new GroupByAggregate(sort, groupByCols, aggregators);
    std::shared_ptr<Operator> aggregate = aggregateOp->getPtr();

    std::shared_ptr<QueryTable> aggregated = aggregate->run();
    std::shared_ptr<QueryTable> aggregatedReveal = aggregated->reveal();

    std::cout << "Initial schema: " << input->getOutput()->getSchema() << std::endl;
    std::cout << "Sort schema: " << sort->getOutput()->getSchema() << std::endl;
    std::cout << "Aggregate schema: " << aggregate->getOutput()->getSchema() << std::endl;


    std::cout << "Observed output: " << aggregatedReveal->toString(true) << std::endl;

    // need to delete dummies from observed output to compare it to expected
    std::shared_ptr<QueryTable> observed = DataUtilities::removeDummies(aggregatedReveal);

    ASSERT_EQ(*expected, *observed);


}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}





