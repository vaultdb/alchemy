#include <util/data_utilities.h>
#include "emp_base_test.h"

#include <iostream>
#include "emp-tool/emp-tool.h"
#include "query_table/table_factory.h"
#include "operators/support/normalize_fields.h"
#include "util/field_utilities.h"


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54335, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(alice_db, "tpch_alice_150", "alice db name");
DEFINE_string(bob_db, "tpch_bob_150", "bob db name");
DEFINE_string(storage, "row", "storage model for tables (row or column)");
DEFINE_int32(ctrl_port, 65450, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");

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
        protocol->pack(input.bits.data(), packed, bit_cnt);
        protocol->unpack(unpacked.bits.data(), packed, unpacked.size());

        // the standard reveal method converts this to decimal.  Need to reveal it bitwise
        if(FLAGS_validation) {
            bool *bools = new bool[bit_cnt];
            protocol->reveal(bools, emp::PUBLIC, unpacked.bits.data(),  bit_cnt);
            delete [] packed;

            vector<int8_t> decoded_bytes =  Utilities::boolsToBytes(bools, bit_cnt);
            decoded_bytes.resize(bit_cnt + 1);
            decoded_bytes[bit_cnt+1] = '\0';

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

TEST_F(EmpTest, test_float_normalization) {
    float_t f_input = 6941.231934;
    float_t g_input = 17236.36719;

    PlainField f_input_prime = PlainField(FieldType::FLOAT, f_input);
    PlainField g_input_prime = PlainField(FieldType::FLOAT, g_input);

    PlainField f_input_norm = NormalizeFields::normalize(f_input_prime, SortDirection::DESCENDING);
    PlainField g_input_norm = NormalizeFields::normalize(g_input_prime, SortDirection::DESCENDING);

    bool input_lt = (f_input < g_input);
    bool norm_geq = (f_input_norm >= g_input_norm); // inverting for DESC

    ASSERT_EQ(input_lt, norm_geq);

    Float f(f_input);
    Float g(g_input);

    SecureField f_prime(FieldType::SECURE_FLOAT, f);
    SecureField g_prime(FieldType::SECURE_FLOAT, g);

    SecureField f_norm = NormalizeFields::normalize(f_prime, SortDirection::DESCENDING);
    SecureField g_norm = NormalizeFields::normalize(g_prime, SortDirection::DESCENDING);


    Bit orig = (f_prime < g_prime);
    Bit norm = !(f_norm < g_norm);
    ASSERT_EQ(orig.reveal(), norm.reveal());
}


// test secret sharing a query table with a single int in EMP
// Testing absent psql dependency
TEST_F(EmpTest, secret_share_table_one_column) {

    uint32_t in_tuple_cnt;
    PlainTable *plain;

    vector<int32_t> alice_input{1, 1, 1, 1, 1, 1, 2, 3, 3, 3};
    vector<int32_t> bob_input{4, 33, 33, 33, 33, 35, 35, 35, 35, 35};
    vector<int32_t> all_input(alice_input);
    all_input.insert(all_input.end(), bob_input.begin(), bob_input.end());
    int32_t *input;

    if(SystemConfiguration::getInstance().emp_mode_ == EmpMode::OUTSOURCED) {
        input = (FLAGS_party == TP) ? all_input.data() : nullptr;
        in_tuple_cnt = (FLAGS_party == TP) ? all_input.size() : 0;
    }
    else {
        input = (FLAGS_party == emp::ALICE) ? alice_input.data() : bob_input.data();
        in_tuple_cnt = (FLAGS_party == emp::ALICE) ? alice_input.size() : bob_input.size();
    }

    QuerySchema schema;
    schema.putField(QueryFieldDesc(0, "test", "test_table", FieldType::INT));
    schema.initializeFieldOffsets();



    plain = TableFactory<bool>::getTable(in_tuple_cnt, schema, storage_model_);

    for (uint32_t i = 0; i < in_tuple_cnt; ++i) {
            Field<bool> val(FieldType::INT, input[i]);
            plain->setField(i, 0, val);
            plain->setDummyTag(i, false);
    }

    SecureTable *secret_shared = plain->secretShare();

    PlainTable *revealed = secret_shared->revealInsecure(emp::PUBLIC);

    // set up expected result by concatenating input tables
    PlainTable *expected = TableFactory<bool>::getTable(all_input.size(), schema, storage_model_);

    for (uint32_t i = 0; i < all_input.size(); ++i) {
        expected->setField(i, 0, Field<bool>(FieldType::INT, all_input[i]));
        expected->setDummyTag(i, false);
    }

    ASSERT_EQ(*expected, *revealed) << "Query table was not processed correctly.";

    delete plain;
    delete secret_shared;
    delete revealed;
    delete expected;

}



TEST_F(EmpTest, sort_and_share_table_one_column) {
    uint32_t in_tuple_cnt = 10;
    vector<int32_t> alice_input{1, 1, 1, 1, 1, 1, 2, 3, 3, 3};
    vector<int32_t> bob_input{4, 33, 33, 33, 33, 35, 35, 35, 35, 35};
    vector<int32_t> all_input(alice_input);
    all_input.insert(all_input.end(), bob_input.begin(), bob_input.end());
    int32_t *input;


//    const size_t tuple_cnt = 4;
//    vector<int32_t> alice_input{1, 3, 4, 7};
//    vector<int32_t> bob_input{2, 5, 6, 8};

if(SystemConfiguration::getInstance().emp_mode_ == EmpMode::OUTSOURCED) {
input = all_input.data();
in_tuple_cnt = (FLAGS_party == TP) ? all_input.size() : 0;
}
else {
input = (FLAGS_party == emp::ALICE) ? alice_input.data() : bob_input.data();
}


QuerySchema schema;
schema.putField(QueryFieldDesc(0, "test", "test_table", FieldType::INT));
schema.initializeFieldOffsets();

SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(1);
PlainTable *input_table = TableFactory<bool>::getTable(in_tuple_cnt, schema, storage_model_, sort_def);

for(uint32_t i = 0; i < in_tuple_cnt; ++i) {
Field<bool> val(FieldType::INT, input[i]);
input_table->setField(i, 0, val);
input_table->setDummyTag(i, false);
}

// tests bitonic merge in 2PC case
SecureTable *secret_shared = input_table->secretShare();


PlainTable *revealed = secret_shared->reveal(emp::PUBLIC);

// set up expected result
std::sort(all_input.begin(), all_input.end());


PlainTable *expected = TableFactory<bool>::getTable(all_input.size(), schema, storage_model_, sort_def);

for(uint32_t i = 0; i < expected->getTupleCount(); ++i) {
Field<bool> val(FieldType::INT, all_input[i]);
expected->setField(i, 0, val);
expected->setDummyTag(i, false);

}

//verify output
ASSERT_EQ(*expected, *revealed) << "Query table was not processed correctly.";

delete secret_shared;
delete revealed;
delete expected;
delete input_table;


}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, false);

	::testing::GTEST_FLAG(filter)=FLAGS_filter;
    return RUN_ALL_TESTS();
}
