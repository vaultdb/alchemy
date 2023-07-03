#include <util/data_utilities.h>
#include "emp_base_test.h"

#include <iostream>
#include "emp-tool/emp-tool.h"
#include "query_table/table_factory.h"


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54325, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");
DEFINE_string(storage, "row", "storage model for tables (row or column)");
DEFINE_int32(ctrl_port, 65446, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");

using namespace vaultdb;

class EmpTest : public EmpBaseTest {
protected:
    void test_packing(const emp::Integer & input);
};






#if __has_include("emp-rescu/emp-rescu.h")
void EmpTest::test_packing(const emp::Integer & input) {
    int bit_cnt = input.size();
            auto protocol =  (OMPCBackend<N> *) backend;
        emp::Integer unpacked(bit_cnt, 0L, emp::PUBLIC); // empty for setup

        // ceil(bitCount / 128)
        OMPCPackedWire *packed = new OMPCPackedWire[TypeUtilities::packedWireCount(bit_cnt)];
        protocol->pack(input.bits.data(), packed, secret_shared.size());
        protocol->unpack(unpacked.bits.data(), packed, unpacked.size());

        // the standard reveal method converts this to decimal.  Need to reveal it bitwise
        if(FLAGS_validation) {
            bool *bools = new bool[bit_cnt];
            protocol->reveal(bools, emp::PUBLIC, unpacked.bits.data(),  bit_cnt);
            delete [] packed;

            vector<int8_t> decoded_bytes =  Utilities::boolsToBytes(bools, bit_cnt);
            decoded_bytes.resize(len + 1);
            decoded_bytes[len+1] = '\0';

            std::string decoded_string((char *) decoded_bytes.data());
            delete [] bools;

            ASSERT_EQ("lithely regular deposits. fluffily          ", decoded_string);
        }
}
#else
void EmpTest::test_packing(const emp::Integer & input) {
    return; // N/A here
}

#endif


//  verify emp configuration for int32s
 TEST_F(EmpTest, emp_test_int) {


    // test encrypting an int from ALICE
    int32_t input = FLAGS_party == emp::ALICE ? 5 : 0;
    emp::Integer alice_secret_shared = emp::Integer(32, input, emp::ALICE);
    int32_t revealed;

    if(FLAGS_validation){
        revealed = alice_secret_shared.reveal<int32_t>(emp::PUBLIC);
        ASSERT_EQ(5, revealed);

    }


    // test encrypting int from BOB
    input = FLAGS_party == emp::ALICE ? 0 : 4;
    emp::Integer bob_secret_shared = emp::Integer(32, input, emp::BOB);

    if(FLAGS_validation) {
        revealed = bob_secret_shared.reveal<int32_t>(emp::PUBLIC);
        ASSERT_EQ(4, revealed);

    }

}



// basic test to verify emp configuration for strings
TEST_F(EmpTest, emp_test_varchar) {

    std::string initial_string = "lithely regular deposits. fluffily";

    size_t len = 44;
    int bit_cnt = len * 8;

    while(initial_string.length() != len) {
        initial_string += " ";
    }

    bool *bools = Utilities::bytesToBool((int8_t *) initial_string.c_str(), len);

    int send_party = (this->emp_mode_ == EmpMode::SH2PC) ? emp::ALICE : emp::TP;

    // encrypting as _sender
    emp::Integer secret_shared(bit_cnt, 0L, send_party);

    if(FLAGS_party == send_party)
        manager_->feed(secret_shared.bits.data(), send_party, bools, bit_cnt);
    else
        manager_->feed(secret_shared.bits.data(), send_party, nullptr, bit_cnt);

    manager_->flush();
    delete [] bools;

    if(FLAGS_validation) {

        // the standard reveal method converts this to decimal.  Need to reveal it bitwise
        bools = new bool[bit_cnt];
        manager_->reveal(bools, PUBLIC, secret_shared.bits.data(), bit_cnt);

        vector<int8_t> decoded_bytes = Utilities::boolsToBytes(bools, bit_cnt);
        decoded_bytes.resize(len + 1);
        decoded_bytes[len + 1] = '\0';
        std::string decoded_str((char *) decoded_bytes.data());
        delete[] bools;
        ASSERT_EQ(initial_string, decoded_str);
    }

    test_packing(secret_shared);

}



// test encrypting a query table with a single int in EMP
// Testing absent psql dependency
TEST_F(EmpTest, secret_share_table_one_column) {

    uint32_t in_tuple_cnt = 10;
    uint32_t out_tuple_cnt = 20;
    PlainTable *plain;

    vector<int32_t> alice_input{1, 1, 1, 1, 1, 1, 2, 3, 3, 3};
    vector<int32_t> bob_input{4, 33, 33, 33, 33, 35, 35, 35, 35, 35};
    vector<int32_t> all_input(alice_input);
    all_input.insert(all_input.end(), bob_input.begin(), bob_input.end());
    int32_t *input;

    if(SystemConfiguration::getInstance().emp_mode_ == EmpMode::OUTSOURCED) {
       input = all_input.data();
       in_tuple_cnt = all_input.size();
       in_tuple_cnt = (FLAGS_party == TP) ? in_tuple_cnt : 0;
    }
    else {
       input = (FLAGS_party == emp::ALICE) ? alice_input.data() : bob_input.data();
    }

    QuerySchema schema;
    schema.putField(QueryFieldDesc(0, "test", "test_table", FieldType::INT));
    schema.initializeFieldOffsets();
    auto collation = DataUtilities::getDefaultSortDefinition(1);

    // set up expected result by concatenating input tables
    PlainTable *expected = TableFactory<bool>::getTable(out_tuple_cnt, schema, storage_model_, collation);


    for(uint32_t i = 0; i < all_input.size(); ++i) {
        Field<bool> val(FieldType::INT, all_input[i]);
        PlainTuple tuple = expected->getPlainTuple(i);
        tuple.setField(0, val);
        tuple.setDummyTag(false);
        expected->putTuple(i, tuple);
    }


    plain = TableFactory<bool>::getTable(in_tuple_cnt, schema, storage_model_, collation);

    for(uint32_t i = 0; i < in_tuple_cnt; ++i) {
        Field<bool> val(FieldType::INT, input[i]);
        plain->setField(i, 0, val);
        plain->setDummyTag(i, false);
    }

    SecureTable *secret_shared = plain->secretShare();

    if(FLAGS_validation) {
        PlainTable *revealed = secret_shared->reveal(emp::PUBLIC);
        //verify output
        ASSERT_EQ(*expected, *revealed) << "Query table was not processed correctly.";
        delete revealed;

    }

    delete plain;
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


    SecureTable *secret_shared = input_table->secretShare();

    if(FLAGS_validation) {
        PlainTable *revealed = secret_shared->reveal(emp::PUBLIC);

        // set up expected result
        std::vector<int32_t> input_tuples = alice_input;
        input_tuples.insert(input_tuples.end(), bob_input.begin(), bob_input.end());
        std::sort(input_tuples.begin(), input_tuples.end());


        PlainTable *expected_table = TableFactory<bool>::getTable(input_tuples.size(), schema, storage_model_,
                                                                  sort_def);

        for (uint32_t i = 0; i < input_tuples.size(); ++i) {
            Field<bool> val(FieldType::INT, input_tuples[i]);
            expected_table->setField(i, 0, val);
            expected_table->setDummyTag(i, false);

        }

        //verify output
        ASSERT_EQ(*expected_table, *revealed) << "Query table was not processed correctly.";
        delete revealed;
        delete expected_table;
    }

    delete secret_shared;
    delete input_table;


}





int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

    return RUN_ALL_TESTS();
}