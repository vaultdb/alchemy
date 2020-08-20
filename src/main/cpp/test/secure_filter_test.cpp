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
#include <operators/support/predicate_class.h>


using namespace emp;
using namespace vaultdb::types;


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54321, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");


class SecureFilterPredicateClass : public PredicateClass {

    Value encryptedLineNumber;
public:
    SecureFilterPredicateClass(int32_t valueToEncrypt) {
        emp::Integer val(32, valueToEncrypt);
        // encrypting here so we don't have to secret share it for every comparison
        encryptedLineNumber = Value(TypeId::ENCRYPTED_INTEGER32, val);

    }

    // filtering for l_linenumber = 1
    Value predicateCall(const QueryTuple & aTuple) const override {
        Value field = *(aTuple.getField(1)->GetValue());

        Value res = field == encryptedLineNumber;
        std::cout << "Comparing " << field.reveal() << " to " << encryptedLineNumber.reveal() << " result: " <<  res.reveal() <<  " at " << &res << std::endl;

        res = !res;
        return res;  // (!) because dummy is false if our selection criteria is satisfied
    }

};



class SecureFilterTest : public ::testing::Test {


protected:
    void SetUp() override {};
    void TearDown() override{};
};





TEST_F(SecureFilterTest, test_table_scan) {

    EmpManager *empManager = EmpManager::getInstance();
    empManager->configureEmpManager(FLAGS_alice_host.c_str(), FLAGS_port, (EmpParty) FLAGS_party);
    std::string dbName =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";

    std::string sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem ORDER BY l_comment LIMIT 5";
    std::string expectedOutput = "(#0 int32 lineitem.l_orderkey, #1 int32 lineitem.l_linenumber, #2 varchar(1) lineitem.l_linestatus) isEncrypted? 0\n"
                                 "(338759, 2, F) (dummy=false)\n"
                                 "(435171, 1, O) (dummy=false)\n"
                                 "(1028, 7, F) (dummy=false)\n"
                                 "(373158, 5, F) (dummy=false)\n"
                                 "(85090, 6, O) (dummy=false)\n"
                                 "(7299, 1, F) (dummy=false)\n"
                                 "(16452, 4, F) (dummy=false)\n"
                                 "(232579, 1, O) (dummy=false)\n"
                                 "(474533, 3, O) (dummy=false)\n"
                                 "(173698, 1, F) (dummy=false)\n";

    std::shared_ptr<SecureSqlInput> input(new SecureSqlInput(dbName, sql, false));
    std::shared_ptr<QueryTable> output = input->run();

    std::unique_ptr<QueryTable> revealed = output->reveal((EmpParty) emp::PUBLIC);
    std::cout << *revealed << std::endl;

    ASSERT_EQ(expectedOutput, revealed->toString());

    empManager->close();


}





TEST_F(SecureFilterTest, test_filter) {
    EmpManager *empManager = EmpManager::getInstance();
    empManager->configureEmpManager(FLAGS_alice_host.c_str(), FLAGS_port, (EmpParty) FLAGS_party);
    std::string dbName =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";

    std::string sql = "SELECT l_orderkey, l_linenumber, l_linestatus  FROM lineitem ORDER BY l_comment LIMIT 5";

    std::string expectedOutput = "(#0 int32 lineitem.l_orderkey, #1 int32 lineitem.l_linenumber, #2 varchar(1) lineitem.l_linestatus) isEncrypted? 0\n"
                                 "(435171, 1, O) (dummy=false)\n"
                                 "(7299, 1, F) (dummy=false)\n"
                                 "(232579, 1, O) (dummy=false)\n"
                                 "(173698, 1, F) (dummy=false)\n";


    std::shared_ptr<Operator> input = std::make_shared<SecureSqlInput>(dbName, sql, false);

    std::shared_ptr<PredicateClass> aPredicate(new SecureFilterPredicateClass(1));  // secret share the constant (1) just once
    Filter *filterOp = new Filter(aPredicate, input);
    std::shared_ptr<Operator> filter = filterOp->getPtr();

    std::shared_ptr<QueryTable> result = filter->run();
    std::unique_ptr<QueryTable> revealed = result->reveal(EmpParty::PUBLIC);
    std::cout << "Result: " << *revealed << std::endl;


    ASSERT_EQ(expectedOutput,  revealed->toString());

}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}


