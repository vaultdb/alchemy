#include <util/data_utilities.h>
#include "emp_base_test.h"

#include <iostream>
#include "emp-tool/emp-tool.h"
#include "query_table/query_table.h"
#include "operators/support/normalize_fields.h"
#include "operators/sort.h"
#include "query_table/column_table.h"


DEFINE_int32(party, 1, "party for EMP execution");
DEFINE_int32(port, 54345, "port for EMP execution");
DEFINE_string(alice_host, "127.0.0.1", "alice hostname for execution");
DEFINE_string(unioned_db, "tpch_unioned_150", "unioned db name");
DEFINE_string(alice_db, "tpch_alice_150", "alice db name");
DEFINE_string(bob_db, "tpch_bob_150", "bob db name");
DEFINE_int32(ctrl_port, 65455, "port for managing EMP control flow by passing public values");
DEFINE_bool(validation, true, "run reveal for validation, turn this off for benchmarking experiments (default true)");
DEFINE_string(filter, "*", "run only the tests passing this filter");
DEFINE_string(storage, "column", "storage model for columns (column, wire_packed or compressed)");


using namespace vaultdb;

class EmpTest : public EmpBaseTest {};


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


    int send_party = manager_->sendingParty();
    bool sender = (FLAGS_party == send_party);
    if(emp_mode_ == EmpMode::SH2PC) {
        send_party = emp::ALICE;
        sender = (FLAGS_party == emp::ALICE);
    }


    Integer secret_shared = Field<Bit>::secretShareString(sender ? initial_string : "", sender, send_party, len);

    if(FLAGS_validation) {
        string decoded_str = Field<Bit>::revealString(secret_shared);
        ASSERT_EQ(initial_string, decoded_str);
    }

    if(storage_model_ == StorageModel::PACKED_COLUMN_STORE) {
      OMPCPackedWire packed[bit_cnt]; // TODO: get packed wire cnt for this
      emp::Integer unpacked(bit_cnt, 0L, emp::PUBLIC);

    manager_->pack((Bit *) secret_shared.bits.data(), (Bit *) packed, bit_cnt);
    manager_->unpack((Bit *) packed, (Bit *) unpacked.bits.data(), bit_cnt);

    // the standard reveal method converts this to decimal.  Need to reveal it bitwise
    if(FLAGS_validation) {
        string revealed = Field<Bit>::revealString(unpacked);
        ASSERT_EQ(initial_string, revealed);
    }
   }
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

    Float f(f_input, PUBLIC);
    Float g(g_input, PUBLIC);

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

    QuerySchema schema;
    schema.putField(QueryFieldDesc(0, "test", "test_table", FieldType::INT));
    schema.initializeFieldOffsets();


    if(emp_mode_ != EmpMode::SH2PC) {
        in_tuple_cnt = (FLAGS_party == manager_->sendingParty())  ? all_input.size() : 0;
        plain = PlainTable::getTable(in_tuple_cnt, schema);
        if(manager_->sendingParty() == FLAGS_party) {
            input = all_input.data();
            memcpy(plain->column_data_[0].data(), input, in_tuple_cnt * sizeof(int32_t));
            memset(plain->column_data_[-1].data(), 0, in_tuple_cnt * sizeof(bool));
        }
    }
    else {
        input = (FLAGS_party == emp::ALICE) ? alice_input.data() : bob_input.data();
        in_tuple_cnt = (FLAGS_party == emp::ALICE) ? alice_input.size() : bob_input.size();
        plain = PlainTable::getTable(in_tuple_cnt, schema);

        memcpy(plain->column_data_[0].data(), input, in_tuple_cnt * sizeof(int32_t));
        memset(plain->column_data_[-1].data(), 0, in_tuple_cnt * sizeof(bool));
    }

    SecureTable *secret_shared = plain->secretShare();
    PlainTable *revealed = secret_shared->revealInsecure(emp::PUBLIC);

    // set up expected result by concatenating input tables
    PlainTable *expected = PlainTable::getTable(all_input.size(), schema);
    memcpy(expected->column_data_[0].data(), all_input.data(), all_input.size() * sizeof(int32_t));
    memset(expected->column_data_[-1].data(), 0, all_input.size() * sizeof(bool));

    ASSERT_EQ(*expected, *revealed) << "Query table was not processed correctly.";

    delete plain;
    delete secret_shared;
    delete revealed;
    delete expected;

}



TEST_F(EmpTest, sort_and_share_table_one_column) {
    uint32_t tuple_cnt = 10;
    vector<int32_t> alice_input{1, 1, 1, 1, 1, 1, 2, 3, 3, 3};
    vector<int32_t> bob_input{4, 33, 33, 33, 33, 35, 35, 35, 35, 35};
    SortDefinition sort_def = DataUtilities::getDefaultSortDefinition(1);

//    size_t tuple_cnt = 4;
//    vector<int32_t> alice_input{1, 3, 4, 7};
//    vector<int32_t> bob_input{2, 5, 6, 8};

    vector<int32_t> all_input(alice_input);
    all_input.insert(all_input.end(), bob_input.begin(), bob_input.end());

    int32_t *input = nullptr;

    QuerySchema schema;
    schema.putField(QueryFieldDesc(0, "test", "test_table", FieldType::INT));
    schema.initializeFieldOffsets();
    PlainTable  *input_table;


    if(emp_mode_ != EmpMode::SH2PC) {
        tuple_cnt = (FLAGS_party == manager_->sendingParty()) ? all_input.size() : 0;
        input_table = PlainTable::getTable(tuple_cnt, schema, sort_def);
        if(FLAGS_party == manager_->sendingParty()) {
            input = all_input.data();
            memcpy(input_table->column_data_[0].data(), input, tuple_cnt * sizeof(int32_t));
            memset(input_table->column_data_[-1].data(), 0, tuple_cnt * sizeof(bool));
        }


    }
    else {
        input = (FLAGS_party == emp::ALICE) ? alice_input.data() : bob_input.data();
        input_table = PlainTable::getTable(tuple_cnt, schema, sort_def);
        memcpy(input_table->column_data_[0].data(), input, tuple_cnt * sizeof(int32_t));
        memset(input_table->column_data_[-1].data(), 0, tuple_cnt * sizeof(bool));
    }


    // tests bitonic merge in 2PC case
    SecureTable *secret_shared = input_table->secretShare();
    PlainTable *revealed = secret_shared->reveal(emp::PUBLIC);

    // set up expected result
     std::sort(all_input.begin(), all_input.end());

    // use ColumnTable to avoid "double testing" experimental storage layouts
    PlainTable *expected = new ColumnTable<bool>(all_input.size(), schema, sort_def);
    memcpy(expected->column_data_[0].data(), all_input.data(), all_input.size() * sizeof(int32_t));
    memset(expected->column_data_[-1].data(), 0, all_input.size());

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
    int i = RUN_ALL_TESTS();
    google::ShutDownCommandLineFlags();
    return i;

}
