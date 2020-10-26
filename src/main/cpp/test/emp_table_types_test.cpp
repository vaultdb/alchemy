//
// Created by Jennie Rogers on 10/24/20.
//

#include <data/PsqlDataProvider.h>
#include <util/type_utilities.h>
#include <util/data_utilities.h>
#include "support/QueryTableTestQueries.h"
#include "support/EmpBaseTest.h"

DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54326, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");




// all setup  handled in parent class
class EmpTableTypesTest : public EmpBaseTest {};


// test more column types
TEST_F(EmpTableTypesTest, encrypt_table) {

    PsqlDataProvider dataProvider;
    string db_name =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";

    std::string inputQuery = QueryTableTestQueries::getInputQuery();

    std::cout << "Querying " << db_name << " at " << FLAGS_alice_host <<  ":" << FLAGS_port <<  " with: " << inputQuery << std::endl;




    std::unique_ptr<QueryTable>  inputTable = dataProvider.getQueryTable(db_name,
                                                                         inputQuery, false);

    std::cout << "Initial table: " << *inputTable << std::endl;

    std::shared_ptr<QueryTable> encryptedTable = inputTable->secretShare(netio, FLAGS_party);

    std::cout << "Finished encrypting table with " << encryptedTable->getTupleCount() << " tuples." << std::endl;

    netio->flush();

    std::unique_ptr<QueryTable> expectedTable = DataUtilities::getUnionedResults("tpch_alice", "tpch_bob",  inputQuery, false);

    std::cout << "Expected:\n" << *expectedTable << std::endl;


    std::unique_ptr<QueryTable> decryptedTable = encryptedTable->reveal(emp::PUBLIC);
    std::cout << "Observed: \n" << *decryptedTable << endl;

    ASSERT_EQ(*expectedTable, *decryptedTable) << "Query table was not processed correctly.";



}

TEST_F(EmpTableTypesTest, encrypt_table_dummy_tag) {

    PsqlDataProvider dataProvider;
    string db_name =  FLAGS_party == emp::ALICE ? "tpch_alice" : "tpch_bob";

    std::string inputQuery = QueryTableTestQueries::getInputQueryDummyTag();

    std::cout << "Querying " << db_name << " at " << FLAGS_alice_host <<  ":" << FLAGS_port <<  " with: " << inputQuery << std::endl;




    std::unique_ptr<QueryTable>  inputTable = dataProvider.getQueryTable(db_name,
                                                                         inputQuery, true);


    std::cout << "Initial table: " << *inputTable << std::endl;
    std::shared_ptr<QueryTable> encryptedTable = inputTable->secretShare(netio, FLAGS_party);
    std::cout << "Finished encrypting table with " << encryptedTable->getTupleCount() << " tuples." << std::endl;
    netio->flush();

    std::unique_ptr<QueryTable> expectedTable = DataUtilities::getUnionedResults("tpch_alice", "tpch_bob", inputQuery,
                                                                                true);

    std::cout << "Expected:\n" << *expectedTable << std::endl;


    std::unique_ptr<QueryTable> decryptedTable = encryptedTable->reveal(emp::PUBLIC);
    std::cout << "Observed: \n" << *decryptedTable << endl;

    ASSERT_EQ(*expectedTable, *decryptedTable) << "Query table was not processed correctly.";



}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}
