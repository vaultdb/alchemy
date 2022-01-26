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

UnionHybridData::UnionHybridData(const QuerySchema & srcSchema, NetIO *aNetio, const int & aParty)  :  party(aParty), netio(aNetio){
    QuerySchema inputSchema = srcSchema;
    SortDefinition emptySortDefinition;
    // placeholder, retains schema
    inputTable = std::shared_ptr<SecureTable>(new SecureTable(1, inputSchema, emptySortDefinition));

}

// based on EmpManager::encryptedVarchar
Integer UnionHybridData::readEncrypted(int8_t *secretSharedBits, const size_t &sizeBytes, const int & dstParty) {


        bool *bools = Utilities::bytesToBool(secretSharedBits, sizeBytes);

        Integer result(sizeBytes * 8, 0L, dstParty);


        if(party == dstParty) {
            ProtocolExecution::prot_exec->feed((block *)result.bits.data(), dstParty, bools, sizeBytes * 8);
        }
        else {
            ProtocolExecution::prot_exec->feed((block *)result.bits.data(), dstParty, nullptr, sizeBytes * 8);
        }


        delete [] bools;

        return result;

    }

shared_ptr<SecureTable>
UnionHybridData::readLocalInput(const string &localInputFile, const QuerySchema &src_schema, NetIO *netio,
                                const int &party) {
    std::unique_ptr<PlainTable> localInput = CsvReader::readCsv(localInputFile, src_schema);
    std::shared_ptr<SecureTable> encryptedTable = PlainTable::secretShare(*localInput, netio, party);
    return encryptedTable;

}





// plaintext input schema, setup for XOR-shared data
shared_ptr<SecureTable>
UnionHybridData::readSecretSharedInput(const string &secretSharesFile, const QuerySchema &plain_schema,
                                       const int &party) {

    // read in binary and then xor it with other side to secret share it.
    std::vector<int8_t> src_data = DataUtilities::readFile(secretSharesFile);
    size_t src_byte_cnt = src_data.size();
    size_t src_bit_cnt = src_byte_cnt * 8;
    size_t tuple_cnt = src_bit_cnt / plain_schema.size(); // need byte-size for tuples owing to bit/byte disconnect


    auto logger = vaultdb_logger::get();
    assert(src_bit_cnt % plain_schema.size() == 0);

    bool *src_bools = new bool[src_bit_cnt];
    emp::to_bool<int8_t>(src_bools, src_data.data(), src_bit_cnt, false);

    // convert serialized representation from byte-aligned to bit-by-bit
    // expect 486 tuples
    QuerySchema secure_schema = QuerySchema::toSecure(plain_schema);
    size_t dst_bit_cnt = tuple_cnt * secure_schema.size();
    size_t remainder = dst_bit_cnt % 128; // pad it to 128-bit increments
    size_t dst_bit_alloc = dst_bit_cnt + remainder;
    bool *dst_bools = new bool[dst_bit_alloc];
    assert(dst_bools != nullptr);

    
    plain_to_secure_bits(src_bools, dst_bools, plain_schema, secure_schema, tuple_cnt);

    delete [] src_bools;

    Integer alice(dst_bit_cnt, 0L, emp::PUBLIC);
    Integer bob(dst_bit_cnt, 0L, emp::PUBLIC);

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
    std::shared_ptr<SecureTable> shared_table = SecureTable::deserialize(secure_schema,
                                                                              shared_data.bits);


     delete [] dst_bools;
     return shared_table;

}

std::shared_ptr<SecureTable> UnionHybridData::getInputTable() {
    return inputTable;
}

void UnionHybridData::resizeAndAppend(std::shared_ptr<SecureTable> toAdd) {
    size_t oldTupleCount = inputTable->getTupleCount();
    size_t newTupleCount = oldTupleCount + toAdd->getTupleCount();
    inputTable->resize(newTupleCount);

    int writeIdx = oldTupleCount;

    for(size_t i = 0; i < toAdd->getTupleCount(); ++i) {
        inputTable->putTuple(writeIdx, toAdd->getTuple(i));
        ++writeIdx;
    }

}

shared_ptr<SecureTable> UnionHybridData::unionHybridData(const QuerySchema &schema, const string &localInputFile,
                                                        const string &secretSharesFile, NetIO *aNetIO,
                                                        const int &party) {

    std::shared_ptr<SecureTable> local = UnionHybridData::readLocalInput(localInputFile, schema, aNetIO,
                                                                party);


    if(!secretSharesFile.empty()) {
      std::shared_ptr<SecureTable> remote = UnionHybridData::readSecretSharedInput(secretSharesFile, schema, party);
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

shared_ptr<SecureTable>
UnionHybridData::unionHybridData(const string &dbName, const string &inputQuery, const string &secretSharesFile,
                                 NetIO *aNetIO, const int &party) {


    std::shared_ptr<PlainTable> local_plain = DataUtilities::getQueryResults(dbName, inputQuery, false);
    auto logger = vaultdb_logger::get();
    BOOST_LOG(logger) << "Reading in " << local_plain->getTupleCount() << " tuples from local db." << endl;

    std::shared_ptr<SecureTable> local = SecureTable::secretShare(*local_plain, aNetIO, party);


    if(!secretSharesFile.empty()) {
        std::shared_ptr<SecureTable> remote = UnionHybridData::readSecretSharedInput(secretSharesFile, *local_plain->getSchema(), party);
        BOOST_LOG(logger) << "Reading in " << remote->getTupleCount() << " secret-shared records." << endl;

        // TODO: reverse the tuple order for bitonic merge, save one more sort - see bottom of file for more
        Union<emp::Bit> union_op(local, remote);
        return union_op.run();
    }

    return local;


}

shared_ptr<SecureTable>
UnionHybridData::unionHybridDataBatch(const string &dbName, const string &inputQuery, const string &secretSharesFile,
                                      NetIO *aNetIO, const int &party, const int &batch_cnt, const int &batch_id) {
    return shared_ptr<SecureTable>();
}
