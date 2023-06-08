#include <util/data_utilities.h>
#include "emp_base_test.h"
#include "query_table/secure_tuple.h"
#include "query_table/plain_tuple.h"

#include <emp-zk/emp-zk.h>
#include <iostream>
#include "emp-tool/emp-tool.h"
#include "query_table/table_factory.h"


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54321, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");

using namespace vaultdb;

class EmpTest : public EmpBaseTest {
};





//  verify emp configuration for int32s from both ALICE and BOB

 TEST_F(EmpTest, emp_test_int) {


    // test encrypting an int from ALICE
    int32_t input = FLAGS_party == emp::ALICE ? 5 : 0;
    emp::Integer alice_secret_shared = emp::Integer(32, input, emp::ALICE);
    int32_t revealed = alice_secret_shared.reveal<int32_t>(emp::PUBLIC);

    ASSERT_EQ(5, revealed);


    // test encrypting int from BOB
    input = FLAGS_party == emp::ALICE ? 0 : 4;
    emp::Integer bob_secret_shared = emp::Integer(32, input, emp::BOB);
    revealed = bob_secret_shared.reveal<int32_t>(emp::PUBLIC);

    ASSERT_EQ(4, revealed);

}



// basic test to verify emp configuration for strings
TEST_F(EmpTest, emp_test_varchar) {

    std::string initial_string = "lithely regular deposits. fluffily";

    size_t len = 44;
    int string_bits = len * 8;

    while(initial_string.length() != len) {
        initial_string += " ";
    }



    bool *bools = Utilities::bytesToBool((int8_t *) initial_string.c_str(), len);

    // encrypting as ALICE
    emp::Integer aliceSecretShared(string_bits, 0L, emp::ALICE);

    if(FLAGS_party == emp::ALICE) {
        ProtocolExecution::prot_exec->feed((block *)aliceSecretShared.bits.data(), emp::ALICE, bools, string_bits);
    }
    else {
        ProtocolExecution::prot_exec->feed((block *)aliceSecretShared.bits.data(), emp::ALICE, nullptr, string_bits);
    }

    netio_->flush();
    delete [] bools;



    // the standard reveal method converts this to decimal.  Need to reveal it bitwise

    bools = new bool[string_bits];
    ProtocolExecution::prot_exec->reveal(bools, emp::PUBLIC, (block *)aliceSecretShared.bits.data(), string_bits);



    vector<int8_t> decodedBytes =  Utilities::boolsToBytes(bools, string_bits);
    decodedBytes.resize(len + 1);
    decodedBytes[len + 1] = '\0';


    std::string decodedString((char *) decodedBytes.data());
    delete [] bools;


    ASSERT_EQ(initial_string, decodedString);


}



// test encrypting a query table with a single int in EMP
// Testing absent psql dependency
TEST_F(EmpTest, secret_share_table_one_column) {

    const uint32_t tuple_cnt = 10;
    vector<int32_t> alice_input{1, 1, 1, 1, 1, 1, 2, 3, 3, 3};
    vector<int32_t> bob_input{4, 33, 33, 33, 33, 35, 35, 35, 35, 35};


    int32_t *input = (FLAGS_party == emp::ALICE) ? alice_input.data() : bob_input.data();

    QuerySchema schema;
    schema.putField(QueryFieldDesc(0, "test", "test_table", FieldType::INT));
    schema.initializeFieldOffsets();

    // set up expected result by concatenating input tables
    PlainTable *expected = TableFactory<bool>::getTable(2 * tuple_cnt, schema, storage_model_);
    std::vector<int32_t> concat = alice_input;
    concat.insert(concat.end(), bob_input.begin(), bob_input.end());


    for(uint32_t i = 0; i < concat.size(); ++i) {
        Field<bool> val(FieldType::INT, concat[i]);
        PlainTuple tuple = expected->getPlainTuple(i);
        tuple.setField(0, val);
        tuple.setDummyTag(false);
    }


    PlainTable *plain = TableFactory<bool>::getTable(tuple_cnt, schema, storage_model_);

    for(uint32_t i = 0; i < tuple_cnt; ++i) {
        Field<bool> val(FieldType::INT, input[i]);
        plain->setField(i, 0, val);
        plain->setDummyTag(i, false);
    }

    SecureTable *secret_shared = plain->secretShare(netio_, FLAGS_party);

    PlainTable *revealed = secret_shared->reveal(emp::PUBLIC);


    //verify output
    ASSERT_EQ(*expected, *revealed) << "Query table was not processed correctly.";

    delete plain;
    delete revealed;
    delete expected;
    delete secret_shared;

}



TEST_F(EmpTest, sort_and_share_table_one_column) {
    const uint32_t tuple_cnt = 10;
    vector<int32_t> alice_input{1, 1, 1, 1, 1, 1, 2, 3, 3, 3};
    vector<int32_t> bob_input{4, 33, 33, 33, 33, 35, 35, 35, 35, 35};

    /*const size_t tuple_cnt = 4;
    vector<int32_t> alice_input{1, 3, 4, 7};
    vector<int32_t> bob_input{2, 5, 6, 8}; */
    int32_t *input = (FLAGS_party == emp::ALICE) ? alice_input.data() : bob_input.data();

    QuerySchema schema;
    schema.putField(QueryFieldDesc(0, "test", "test_table", FieldType::INT));
    schema.initializeFieldOffsets();

    SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(1);
    PlainTable *input_table = TableFactory<bool>::getTable(tuple_cnt, schema, storage_model_, sort_def);

    for(uint32_t i = 0; i < tuple_cnt; ++i) {
        Field<bool> val(FieldType::INT, input[i]);
        input_table->setField(i, 0, val);
        input_table->setDummyTag(i, false);
    }


    SecureTable *secret_shared = input_table->secretShare(netio_, FLAGS_party);


    PlainTable *revealed = secret_shared->reveal(emp::PUBLIC);

    // set up expected result
    std::vector<int32_t> input_tuples = alice_input;
    input_tuples.insert(input_tuples.end(), bob_input.begin(), bob_input.end());
    std::sort(input_tuples.begin(), input_tuples.end());


    PlainTable *expected_table = TableFactory<bool>::getTable(input_tuples.size(), schema, storage_model_, sort_def);

    for(uint32_t i = 0; i < input_tuples.size(); ++i) {
        Field<bool> val(FieldType::INT, input_tuples[i]);
        expected_table->setField(i, 0, val);
        expected_table->setDummyTag(i, false);

    }

    //verify output
    ASSERT_EQ(*expected_table, *revealed) << "Query table was not processed correctly.";

    delete secret_shared;
    delete revealed;
    delete expected_table;
    delete input_table;


}





int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}