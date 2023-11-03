#include <pilot/src/common/shared_schema.h>
#include <data/csv_reader.h>
#include "union_hybrid_data.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <util/data_utilities.h>
#include <query_table/plain_tuple.h>
#include <query_table/secure_tuple.h>
#include <operators/union.h>
#include <operators/sort.h>
#include <query_table/query_table.h>

UnionHybridData::UnionHybridData(const QuerySchema & src_schema)  {
    // placeholder, retains schema
    input_table_ = new SecureTable(0, src_schema);

}

// based on EmpManager::encryptedVarchar
Integer UnionHybridData::readEncrypted(int8_t *secret_shared_bits, const size_t &size_bytes, const int & dst_party) {


        bool *bools = Utilities::bytesToBool(secret_shared_bits, size_bytes);

        Integer result(size_bytes * 8, 0L, dst_party);

        int party = SystemConfiguration::getInstance().party_;
        EmpManager *manager = SystemConfiguration::getInstance().emp_manager_;
        if(party == dst_party) {
            //         virtual void feed(Bit *labels, int party, const bool *b, int bit_cnt) = 0;
            manager->feed(result.bits.data(), dst_party, bools, size_bytes * 8);
        }
        else {
            manager->feed(result.bits.data(), dst_party, nullptr, size_bytes * 8);
        }


        delete [] bools;

        return result;

    }

SecureTable *
UnionHybridData::readLocalInput(const string &localInputFile, const QuerySchema &src_schema) {
    PlainTable *local = CsvReader::readCsv(localInputFile, src_schema);
    SecureTable *secret_shared = local->secretShare();
    delete local;

    return secret_shared;

}





// plaintext input schema, setup for XOR-shared data
SecureTable *UnionHybridData::readSecretSharedInput(const string &secret_shares_input, const QuerySchema &plain_schema) {
    cout << "RSS: Plain schema: " << plain_schema << endl;
    QuerySchema secure_schema = QuerySchema::toSecure(plain_schema);
    cout << "Secure schema: " << secure_schema << endl;

    // read in binary and then xor it with other side to secret share it.
    std::vector<int8_t> src_data = DataUtilities::readFile(secret_shares_input);
    size_t src_byte_cnt = src_data.size();
    size_t src_bit_cnt = src_byte_cnt * 8;
    size_t tuple_cnt = src_bit_cnt / plain_schema.size();
    cout << "reading secret shares for " << tuple_cnt << " rows." << endl;

    // convert serialized representation from byte-aligned to bit-by-bit
    size_t dst_bit_cnt = tuple_cnt * secure_schema.bitCnt();
//    size_t dst_bit_alloc = dst_bit_cnt + (dst_bit_cnt % 128);  // pad it to 128-bit increments for emp
    bool *dst_bools = new bool[dst_bit_cnt]; // dst_bit_alloc
    bool *dst_cursor = dst_bools;
    int8_t *src_cursor = src_data.data();

    assert(src_bit_cnt % plain_schema.size() == 0);
    for(int i = 0; i < plain_schema.getFieldCount(); ++i) {
        auto plain_field = plain_schema.getField(i);
        auto secure_field = secure_schema.getField(i);
        cout << "Reading field " << i << " " << plain_field << " " << secure_field << ", plain offset: " << (src_cursor - src_data.data()) << ", sec offset: " << (dst_cursor - dst_bools) << endl;
        if(plain_field.size() == secure_field.size()) { // 1:1, just serialize it
            int write_size = secure_schema.getField(i).size() * tuple_cnt;
            emp::to_bool<int8_t>(dst_cursor, src_cursor, write_size, false);
            dst_cursor += write_size;
            src_cursor += write_size/8;
        }
        else {
            assert(plain_field.size() == (secure_field.size() + 7)); // only for bools
            for(int j = 0; j < tuple_cnt; ++j) {
                *dst_cursor =   ((*src_cursor & 1) != 0); // (bool) *src_cursor;
                ++dst_cursor;
                ++src_cursor;
            }
        } // end for each tuple
    } // end for all fields

    // copy dummy tag
    for(int i = 0; i < tuple_cnt; ++i) {
        *dst_cursor = ((*src_cursor & 1) != 0);
        ++dst_cursor;
        ++src_cursor;
    }


    Integer alice(dst_bit_cnt, 0L, emp::PUBLIC);
    Integer bob(dst_bit_cnt, 0L, emp::PUBLIC);
    int party = SystemConfiguration::getInstance().party_;
    EmpManager *manager = SystemConfiguration::getInstance().emp_manager_;

    if(party == ALICE) {
        // feed through Alice's data, then wait for Bob's
        manager->feed(alice.bits.data(), ALICE, dst_bools, dst_bit_cnt);
        manager->feed(bob.bits.data(), BOB, nullptr, dst_bit_cnt);
    }
    else {
        manager->feed(alice.bits.data(), ALICE, nullptr, dst_bit_cnt);
        manager->feed(bob.bits.data(), BOB, dst_bools, dst_bit_cnt);

    }

    Integer shared_data = alice ^ bob;
    // remove padding
   //    shared_data.bits.resize(dst_bit_cnt);

    SecureTable *shared_table = QueryTable<Bit>::deserialize(secure_schema, shared_data.bits);
    cout << "First rows: " << DataUtilities::printTable(shared_table,  10, true) << endl;

     delete [] dst_bools;
     return shared_table;

}

SecureTable *UnionHybridData::getInputTable() {
    return input_table_;
}

void UnionHybridData::resizeAndAppend(SecureTable *to_add) {
    size_t old_tuple_cnt = input_table_->getTupleCount();
    size_t new_tuple_cnt = old_tuple_cnt + to_add->getTupleCount();
    input_table_->resize(new_tuple_cnt);

    int write_idx = old_tuple_cnt;

    for(size_t i = 0; i < to_add->getTupleCount(); ++i) {
        input_table_->cloneRow(write_idx, 0, to_add, i);
        ++write_idx;
    }

}

SecureTable *UnionHybridData::unionHybridData(const QuerySchema &schema, const string &localInputFile,
                                                        const string &secretSharesFile) {

    SecureTable *local = UnionHybridData::readLocalInput(localInputFile, schema);


    if(!secretSharesFile.empty()) {
     SecureTable *remote = UnionHybridData::readSecretSharedInput(secretSharesFile, schema);
      Union<emp::Bit> union_op(local, remote);
      return union_op.run();
    }

    return local;

}


SecureTable *
UnionHybridData::unionHybridData(const string &db_name, const string &input_query, const string &secret_shares_file) {


    PlainTable *local_plain = DataUtilities::getQueryResults(db_name, input_query, false);
    size_t total_tuples = local_plain->getTupleCount();

    cout << "Reading in " << local_plain->getTupleCount() << " tuples from local db." << endl;

    SecureTable *local = local_plain->secretShare();
    int party = SystemConfiguration::getInstance().party_;

    string other_party = (party == ALICE) ? "Bob" : "Alice";
    cout <<  other_party << " read in " << local->getTupleCount() - local_plain->getTupleCount() << " tuples." << endl;

    total_tuples += local->getTupleCount() - local_plain->getTupleCount();

    QuerySchema local_plain_schema = local_plain->getSchema();
    delete local_plain;

    if(!secret_shares_file.empty()) {
        SecureTable *remote = UnionHybridData::readSecretSharedInput(secret_shares_file, local_plain_schema);
        cout << "Reading in " << remote->getTupleCount() << " secret-shared records from " << secret_shares_file << endl;
        total_tuples += remote->getTupleCount();
        Union<emp::Bit> union_op(local, remote);
        cout << "Total tuples read: " << total_tuples << endl;
        // deep copy so it isn't deleted with union_op
        SecureTable *res = union_op.run()->clone();
        return res;
    }
    
    cout << "Total tuples read: " << total_tuples << endl;
    return local;


}

