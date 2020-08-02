//
// Created by Jennie Rogers on 8/2/20.
//
#include <data/PsqlDataProvider.h>
#include <gflags/gflags.h>
#include <gtest/gtest.h>
#include <emp-sh2pc/emp-sh2pc.h>
#include <util/emp_manager.h>
#include <util/type_utilities.h>


using namespace emp;


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54321, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");


// TODO: refactor this into a common utility with QueryTableTest
// TODO: figure out how to codify that this test depends on QueryTableTest
class EmpManagerTestEnvironment : public ::testing::Environment {
public:
    // Assume there's only going to be a single instance of this class, so we can just
    // hold the timestamp as a const static local variable and expose it through a
    // static member function
    static std::string getInputQuery() {
        // selecting one of each type:
        // int
        // varchar
        // fixed char
        // numeric
        // date

        static const std::string inputQuery = "SELECT l_orderkey, l_comment, l_returnflag, l_discount, "
                                              "EXTRACT(EPOCH FROM l_commitdate) AS l_commitdate "  // handle timestamps by converting them to longs using SQL - "CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) AS l_commitdate,
                                              "FROM lineitem "
                                              "ORDER BY l_orderkey "
                                              "LIMIT 10";
        return inputQuery;
    }

    static const std::string getExpectedOutput() {

        /*
         * Expected output:
         * tpch_alice=# SELECT l_orderkey, l_comment, l_returnflag, l_discount, EXTRACT(EPOCH FROM l_commitdate) AS l_commitdate FROM lineitem ORDER BY l_orderkey LIMIT 10;
          1 |  pending foxes. slyly re                   | N            |       0.10 |    826761600
          1 | arefully slyly ex                          | N            |       0.07 |    823651200
          3 | nal foxes wake.                            | A            |       0.06 |    753926400
          5 | ts wake furiously                          | R            |       0.02 |    778291200
          5 | sts use slyly quickly special instruc      | R            |       0.07 |    780451200
          7 | es. instructions                           | N            |       0.08 |    825724800
          7 |  unusual reques                            | N            |       0.10 |    827884800
          7 | . slyly special requests haggl             | N            |       0.03 |    828921600
          7 | jole. excuses wake carefully alongside of  | N            |       0.06 |    825033600
         32 | lithely regular deposits. fluffily         | N            |       0.02 |    813024000
(10 rows)
         */

        static const std::string queryOutput = "(#0 int32 lineitem.l_orderkey, #1 varchar(44) lineitem.l_comment, #2 varchar(1) lineitem.l_returnflag, #3 float lineitem.l_discount, #4 double lineitem.l_commitdate) isEncrypted? 0\n"
                                               "(1,  pending foxes. slyly re, N, 0.100000, 826761600.000000) (dummy=false)\n"
                                               "(1, arefully slyly ex, N, 0.070000, 823651200.000000) (dummy=false)\n"
                                               "(3, nal foxes wake. , A, 0.060000, 753926400.000000) (dummy=false)\n"
                                               "(5, ts wake furiously , R, 0.020000, 778291200.000000) (dummy=false)\n"
                                               "(5, sts use slyly quickly special instruc, R, 0.070000, 780451200.000000) (dummy=false)\n"
                                               "(7, es. instructions, N, 0.080000, 825724800.000000) (dummy=false)\n"
                                               "(7,  unusual reques, N, 0.100000, 827884800.000000) (dummy=false)\n"
                                               "(7, . slyly special requests haggl, N, 0.030000, 828921600.000000) (dummy=false)\n"
                                               "(7, jole. excuses wake carefully alongside of , N, 0.060000, 825033600.000000) (dummy=false)\n"
                                               "(32, lithely regular deposits. fluffily , N, 0.020000, 813024000.000000) (dummy=false)\n";

        return queryOutput;

    }

    static std::string getInputQueryDummyTag() {
        // selecting one of each type:
        // int
        // varchar
        // fixed char
        // numeric
        // date
        // dummy tag

        static const std::string inputQueryDummyTag = "SELECT l_orderkey, l_comment, l_returnflag, l_discount, "
                                                      "EXTRACT(EPOCH FROM l_commitdate) AS l_commitdate, "  // handle timestamps by converting them to longs using SQL - "CAST(EXTRACT(EPOCH FROM l_commitdate) AS BIGINT) AS l_commitdate,
                                                      "l_returnflag <> 'N' AS dummy "  // simulate a filter for l_returnflag = 'N' -- all of the ones that dont match are dummies
                                                      "FROM lineitem "
                                                      "ORDER BY l_orderkey "
                                                      "LIMIT 10";
        return inputQueryDummyTag;
    }


    static const std::string getExpectedOutputDummyTag() {

        /*
         * Expected output:
         * tpch_alice=# SELECT l_orderkey, l_comment, l_returnflag, l_discount, EXTRACT(EPOCH FROM l_commitdate) AS l_commitdate FROM lineitem ORDER BY l_orderkey LIMIT 10;
          1 |  pending foxes. slyly re                   | N            |       0.10 |    826761600
          1 | arefully slyly ex                          | N            |       0.07 |    823651200
          3 | nal foxes wake.                            | A            |       0.06 |    753926400
          5 | ts wake furiously                          | R            |       0.02 |    778291200
          5 | sts use slyly quickly special instruc      | R            |       0.07 |    780451200
          7 | es. instructions                           | N            |       0.08 |    825724800
          7 |  unusual reques                            | N            |       0.10 |    827884800
          7 | . slyly special requests haggl             | N            |       0.03 |    828921600
          7 | jole. excuses wake carefully alongside of  | N            |       0.06 |    825033600
         32 | lithely regular deposits. fluffily         | N            |       0.02 |    813024000
(10 rows)
         */

        static const std::string queryOutput = "(#0 int32 lineitem.l_orderkey, #1 varchar(44) lineitem.l_comment, #2 varchar(1) lineitem.l_returnflag, #3 float lineitem.l_discount, #4 double lineitem.l_commitdate) isEncrypted? 0\n"
                                               "(1,  pending foxes. slyly re, N, 0.100000, 826761600.000000) (dummy=false)\n"
                                               "(1, arefully slyly ex, N, 0.070000, 823651200.000000) (dummy=false)\n"
                                               "(3, nal foxes wake. , A, 0.060000, 753926400.000000) (dummy=true)\n"
                                               "(5, ts wake furiously , R, 0.020000, 778291200.000000) (dummy=true)\n"
                                               "(5, sts use slyly quickly special instruc, R, 0.070000, 780451200.000000) (dummy=true)\n"
                                               "(7, es. instructions, N, 0.080000, 825724800.000000) (dummy=false)\n"
                                               "(7,  unusual reques, N, 0.100000, 827884800.000000) (dummy=false)\n"
                                               "(7, . slyly special requests haggl, N, 0.030000, 828921600.000000) (dummy=false)\n"
                                               "(7, jole. excuses wake carefully alongside of , N, 0.060000, 825033600.000000) (dummy=false)\n"
                                               "(32, lithely regular deposits. fluffily , N, 0.020000, 813024000.000000) (dummy=false)\n";

        return queryOutput;

    }
    virtual void SetUp() {

    }

};




class EmpManagerTest : public ::testing::Test {


protected:
    void SetUp() override {};
    void TearDown() override{};
};





TEST_F(EmpManagerTest, emp_manager_test) {

    EmpManager *empManager = EmpManager::getInstance();
    empManager->configureEmpManager(FLAGS_alice_host.c_str(), FLAGS_port, (EmpParty) FLAGS_party);

    int32_t inputValue =  FLAGS_party == emp::ALICE ? 1 : 0;

    emp::Integer aliceSecretShared = emp::Integer(32, inputValue, (int) EmpParty::ALICE);
    empManager->flush();

    int32_t decrypted = aliceSecretShared.reveal<int32_t>(emp::PUBLIC);
    empManager->flush();

    ASSERT_EQ(1, decrypted);

    inputValue =  FLAGS_party == emp::ALICE ? 0 : 4;

    emp::Integer bobSecretShared = emp::Integer(32, inputValue, (int) EmpParty::BOB);

    empManager->flush();
    decrypted = bobSecretShared.reveal<int32_t>(emp::PUBLIC);
    ASSERT_EQ(4, decrypted);



    empManager->close();






}

// test encrypting the query table with EMP
TEST_F(EmpManagerTest, encrypt_table) {

    PsqlDataProvider dataProvider;
    string db_name =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";
    EmpManager *empManager = EmpManager::getInstance();
    empManager->configureEmpManager(FLAGS_alice_host.c_str(), FLAGS_port, (EmpParty) FLAGS_party);

    std::string inputQuery = EmpManagerTestEnvironment::getInputQuery();

    inputQuery =  "SELECT l_orderkey FROM lineitem ORDER BY l_orderkey LIMIT 1";
    std::cout << "Querying " << db_name << " at " << FLAGS_alice_host <<  ":" << FLAGS_port <<  " with: " << inputQuery << std::endl;




    std::unique_ptr<QueryTable>  inputTable = dataProvider.GetQueryTable(db_name,
                                                                         inputQuery, "lineitem", false);


    std::cout << "Initial table: " << *inputTable << std::endl;
    std::unique_ptr<QueryTable> encryptedTable = empManager->secretShareTable(inputTable.get());

    std::cout << "Finished encrypting table with " << encryptedTable->getTupleCount() << " tuples." << std::endl;

    empManager->flush();

    string expectedTable = "(#0 int32 lineitem.l_orderkey) isEncrypted? 0\n"
                           "(1) (dummy=false)\n"
                           "(4) (dummy=false)\n";

    std::cout << "Expected:\n" << expectedTable << std::endl;

    const QueryTuple *encryptedTuple = encryptedTable->GetTuple(0);
    const QueryField *encryptedField = encryptedTuple->GetField(0);
    types::Value value = encryptedField->GetValue();
    types::Value revealedValue = value.reveal(EmpParty::PUBLIC);


    std::unique_ptr<QueryTable> decryptedTable = encryptedTable->reveal(EmpParty::PUBLIC);


    std::cout << "Observed: \n" << *decryptedTable << endl;

    ASSERT_EQ(expectedTable, decryptedTable->toString()) << "Query table was not processed correctly.";

    empManager->close();


}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}

