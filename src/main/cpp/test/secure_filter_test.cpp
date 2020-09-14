//
// Created by Jennie Rogers on 8/16/20.
//

//
// Created by Jennie Rogers on 8/16/20.
//



#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <util/type_utilities.h>
#include <stdexcept>
#include <operators/sql_input.h>
#include <operators/filter.h>
#include <util/emp_manager.h>
#include <operators/secure_sql_input.h>
#include <operators/support/predicate.h>


using namespace emp;
using namespace vaultdb::types;


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54321, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");


class SecureFilterPredicateClass : public Predicate {

    Value encryptedLineNumber;
public:
    SecureFilterPredicateClass(int32_t valueToEncrypt) {
        emp::Integer val(32, valueToEncrypt);
        // encrypting here so we don't have to secret share it for every comparison
        encryptedLineNumber = Value(TypeId::ENCRYPTED_INTEGER32, val);

    }

    // filtering for l_linenumber = 1
    Value predicateCall(const QueryTuple & aTuple) const override {
        Value field = aTuple.getField(1).getValue();

        Value res = field == encryptedLineNumber;

        return res;
    }

};



class SecureFilterTest : public ::testing::Test {



protected:
    void SetUp() override {};
    void TearDown() override{};

    const std::string aliceDb = "tpch_alice";
    const std::string bobDb = "tpch_bob";

};





TEST_F(SecureFilterTest, test_table_scan) {

    EmpManager *empManager = EmpManager::getInstance();
    empManager->configureEmpManager(FLAGS_alice_host.c_str(), FLAGS_port,  FLAGS_party);
    std::string dbName =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";

    std::string sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem ORDER BY l_comment LIMIT 5";
    std::unique_ptr<QueryTable> expected = DataUtilities::getUnionedResults(aliceDb, bobDb, sql, false);

    std::shared_ptr<SecureSqlInput> input(new SecureSqlInput(dbName, sql, false));
    std::shared_ptr<QueryTable> output = input->run();

    std::unique_ptr<QueryTable> revealed = output->reveal(emp::PUBLIC);
    std::cout << *revealed << std::endl;

    ASSERT_EQ(*expected, *revealed);

    empManager->close();


}





TEST_F(SecureFilterTest, test_filter) {
    EmpManager *empManager = EmpManager::getInstance();
    empManager->configureEmpManager(FLAGS_alice_host.c_str(), FLAGS_port, FLAGS_party);
    std::string dbName =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";

    std::string sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem ORDER BY l_comment LIMIT 5";
    std::string expectedResultSql = "WITH input AS (" + sql + ") SELECT *, l_linenumber<>1 dummy FROM input";

    std::unique_ptr<QueryTable> expected = DataUtilities::getUnionedResults(aliceDb, bobDb, expectedResultSql, true);


    std::shared_ptr<Operator> input = std::make_shared<SecureSqlInput>(dbName, sql, false);

    std::shared_ptr<Predicate> aPredicate(new SecureFilterPredicateClass(1));  // secret share the constant (1) just once
    Filter *filterOp = new Filter(aPredicate, input);  // deletion handled by shared_ptr
    std::shared_ptr<Operator> filter = filterOp->getPtr();

    std::shared_ptr<QueryTable> result = filter->run();
    std::unique_ptr<QueryTable> revealed = result->reveal(emp::PUBLIC);
    std::cout << "Result: " << *revealed << std::endl;


    ASSERT_EQ(*expected,  *revealed);

}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}


