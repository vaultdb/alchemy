//
// Created by Jennie Rogers on 8/20/20.
//

#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/filter.h>
#include <operators/sort.h>

using namespace emp;
using namespace vaultdb::types;



DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54321, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");

class SortTest : public ::testing::Test {


protected:
    void SetUp() override {};
    void TearDown() override{};
};





TEST_F(SortTest, testSingleIntColumn) {

    std::string sql = "SELECT c_custkey FROM customer ORDER BY c_address LIMIT 10";  // c_address "randomizes" the order
    std::string expectedResult = "(#0 int32 customer.c_custkey) isEncrypted? 0\n"
                                 "(1523)\n"
                                 "(2310)\n"
                                 "(3932)\n"
                                 "(5345)\n"
                                 "(6512)\n"
                                 "(6578)\n"
                                 "(7934)\n"
                                 "(8702)\n"
                                 "(11702)\n"
                                 "(12431)\n";

    std::string dbName =  "tpch_alice";
    SortDefinition sortDefinition;
    ColumnSort aColumnSort(0, SortDirection::ASCENDING);
    sortDefinition.columnOrders.push_back(aColumnSort);


    std::shared_ptr<Operator> input(new SqlInput(dbName, sql, false));
    Sort *sortOp = new Sort(sortDefinition, input); // heap allocate it
    std::shared_ptr<Operator> sort = sortOp->getPtr();

    std::shared_ptr<QueryTable> result = sort->run();

    std::cout << *result << std::endl;

    ASSERT_EQ(result->toString(), expectedResult);


}
