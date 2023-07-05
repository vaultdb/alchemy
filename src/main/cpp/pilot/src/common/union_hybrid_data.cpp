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
#include "query_table/row_table.h"

UnionHybridData::UnionHybridData(const QuerySchema & srcSchema)  {
    QuerySchema inputSchema = srcSchema;
    SortDefinition emptySortDefinition;
    // placeholder, retains schema
    input_table_ = new RowTable<Bit>(0, inputSchema, emptySortDefinition);

}

// based on EmpManager::encryptedVarchar
Integer UnionHybridData::readEncrypted(int8_t *secret_shared_bits, const size_t &size_bytes, const int & dst_party) {


        bool *bools = Utilities::bytesToBool(secret_shared_bits, size_bytes);

        Integer result(size_bytes * 8, 0L, dst_party);

        int party = SystemConfiguration::getInstance().party_;

        if(party == dst_party) {
            ProtocolExecution::prot_exec->feed((block *)result.bits.data(), dst_party, bools, size_bytes * 8);
        }
        else {
            ProtocolExecution::prot_exec->feed((block *)result.bits.data(), dst_party, nullptr, size_bytes * 8);
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
SecureTable *UnionHybridData::readSecretSharedInput(const string &secretSharesFile, const QuerySchema &plain_schema) {

    // read in binary and then xor it with other side to secret share it.
    std::vector<int8_t> src_data = DataUtilities::readFile(secretSharesFile);
    size_t src_byte_cnt = src_data.size();
    size_t src_bit_cnt = src_byte_cnt * 8;
    size_t tuple_cnt = src_bit_cnt / plain_schema.size();

    assert(src_bit_cnt % plain_schema.size() == 8); // 8 left over for storage model header

    bool *src_bools = new bool[src_bit_cnt];
    emp::to_bool<int8_t>(src_bools, src_data.data(), src_bit_cnt, false);

    // convert serialized representation from byte-aligned to bit-by-bit
    QuerySchema secure_schema = QuerySchema::toSecure(plain_schema);

    size_t dst_bit_cnt = tuple_cnt * secure_schema.size() + 8;
    size_t remainder = dst_bit_cnt % 128; // pad it to 128-bit increments
    size_t dst_bit_alloc = dst_bit_cnt + remainder;
    bool *dst_bools = new bool[dst_bit_alloc];
    assert(dst_bools != nullptr);

    memcpy(dst_bools, src_bools, 8); // copy over storage model header
    plain_to_secure_bits(src_bools+8, dst_bools+8, plain_schema, secure_schema, tuple_cnt);

    delete [] src_bools;

    Integer alice(dst_bit_cnt, 0L, emp::PUBLIC);
    Integer bob(dst_bit_cnt, 0L, emp::PUBLIC);
    int party = SystemConfiguration::getInstance().party_;

    if(party == ALICE) {
        // feed through Alice's data, then wait for Bob's
        ProtocolExecution::prot_exec->feed((block *)alice.bits.data(), ALICE, dst_bools, dst_bit_cnt);
        ProtocolExecution::prot_exec->feed((block *)bob.bits.data(), BOB, nullptr, dst_bit_cnt);
    }
    else {
        ProtocolExecution::prot_exec->feed((block *)alice.bits.data(), ALICE, nullptr, dst_bit_cnt);
        ProtocolExecution::prot_exec->feed((block *)bob.bits.data(), BOB, dst_bools, dst_bit_cnt);

    }

    Integer shared_data = alice ^ bob;
    // remove padding
    shared_data.bits.resize(dst_bit_cnt);
    SecureTable *shared_table = RowTable<Bit>::deserialize(secure_schema, shared_data.bits);


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

void UnionHybridData::plain_to_secure_bits(bool *src, bool *dst, const QuerySchema &plain_schema,
                                           const QuerySchema &secure_schema, const size_t &tuple_cnt) {

    // issue: serialize this to byte-aligned bitstring.   Hence bools and dummy tags have 8 bits instead of 1.
    // solution: only retain the bits we need
    size_t src_pos = 0;
    size_t dst_pos = 0;


    for(size_t i = 0; i < tuple_cnt; ++i) {
        for(size_t j = 0; j < plain_schema.getFieldCount(); ++j) {
            QueryFieldDesc plain_field = plain_schema.getField(j);
            QueryFieldDesc secure_field = secure_schema.getField(j);
            memcpy(dst + dst_pos, src + src_pos, secure_field.size());
            dst_pos += secure_field.size();
            src_pos += plain_field.size();
        }
        // handle dummy tag
        dst[dst_pos] = src[src_pos + 7];
        memcpy(dst + dst_pos, src + src_pos, 1);
        src_pos += 8; // original, byte-aligned bool
        dst_pos += 1; // emp::Bit, semantically a bool
    }



}

SecureTable *
UnionHybridData::unionHybridData(const string &dbName, const string &inputQuery, const string &secretSharesFile) {


    PlainTable *local_plain = DataUtilities::getQueryResults(dbName, inputQuery, false);
    size_t total_tuples = local_plain->getTupleCount();

    cout << "Reading in " << local_plain->getTupleCount() << " tuples from local db." << endl;

    SecureTable *local = local_plain->secretShare();
    int party = SystemConfiguration::getInstance().party_;

    string other_party = (party == ALICE) ? "Bob" : "Alice";
    cout <<  other_party << " read in " << local->getTupleCount() - local_plain->getTupleCount() << " tuples." << endl;

    total_tuples += local->getTupleCount() - local_plain->getTupleCount();

    QuerySchema local_plain_schema = local_plain->getSchema();
    delete local_plain;

    if(!secretSharesFile.empty()) {
        SecureTable *remote = UnionHybridData::readSecretSharedInput(secretSharesFile, local_plain_schema);
        cout << "Reading in " << remote->getTupleCount() << " secret-shared records from " <<  secretSharesFile << endl;
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

