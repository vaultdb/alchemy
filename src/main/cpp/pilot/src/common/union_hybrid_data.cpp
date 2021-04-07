#include <pilot/src/common/shared_schema.h>
#include <data/CsvReader.h>
#include "union_hybrid_data.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <util/data_utilities.h>
#include <query_table/plain_tuple.h>
#include <query_table/secure_tuple.h>
#include <operators/union.h>


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
    Utilities::checkMemoryUtilization(" read csv: ");
    std::cout << "Read " << localInput->getTupleCount() << " tuples of local input." << std::endl;

    std::shared_ptr<SecureTable> encryptedTable = localInput->secret_share(netio, party);
    Utilities::checkMemoryUtilization(" local read: ");
    return encryptedTable;
    /*if(!inputTableInit) {
        inputTable = encryptedTable;
    }
    else {
       resizeAndAppend(encryptedTable);
    }



    inputTableInit = true; */
}





// plaintext input schema, setup for XOR-shared data
shared_ptr<SecureTable>
UnionHybridData::readSecretSharedInput(const string &secretSharesFile, const QuerySchema &plain_schema,
                                       const int &party) {

    // read in binary and then xor it with other side to secret share it.
    std::vector<int8_t> src_data = DataUtilities::readFile(secretSharesFile);
    size_t src_byte_cnt = src_data.size();
    size_t tuple_size = plain_schema.size();
    size_t src_bit_cnt = src_byte_cnt * 8;
    assert(src_bit_cnt % tuple_size == 0);

    bool *bools = new bool[src_bit_cnt];
    std::cout << " allocated " << src_bit_cnt << " bools for reading in secret shared data " << std::endl;
    emp::to_bool<int8_t>(bools, src_data.data(), src_bit_cnt, false);

    Integer alice_bytes(src_bit_cnt, 0L, emp::PUBLIC);
    Integer bob_bytes(src_bit_cnt, 0L, emp::PUBLIC);

    if(party == ALICE) {
        // feed through Alice's data, then wait for Bob's
        ProtocolExecution::prot_exec->feed((block *)alice_bytes.bits.data(), ALICE, bools, src_bit_cnt);
        ProtocolExecution::prot_exec->feed((block *)bob_bytes.bits.data(), BOB, nullptr, src_bit_cnt);
    }
    else {
        ProtocolExecution::prot_exec->feed((block *)alice_bytes.bits.data(), ALICE, nullptr, src_bit_cnt);
        ProtocolExecution::prot_exec->feed((block *)bob_bytes.bits.data(), BOB, bools, src_bit_cnt);

    }

    Integer additionalData = alice_bytes ^ bob_bytes;
    // issue: serialize this to byte-aligned bitstring.   Hence bools and dummy tags have 8 bits instead of 1.
    // solution: only retain the bits we need
    size_t src_pos = 0;
    size_t dst_pos = 0;

    QuerySchema secure_schema = QuerySchema::toSecure(plain_schema);
    size_t tuple_cnt = src_bit_cnt / plain_schema.size();
    assert(src_bit_cnt % plain_schema.size() == 0);

    Integer downsized_bits(tuple_cnt * secure_schema.size(), 0, emp::PUBLIC);
    emp::Bit *src = (emp::Bit *) additionalData.bits.data();
    emp::Bit *dst = (emp::Bit *) downsized_bits.bits.data();


    for(size_t i = 0; i < tuple_cnt; ++i) {
        for(size_t j = 0; j < plain_schema.getFieldCount(); ++j) {
            QueryFieldDesc plain_field = plain_schema.getField(j);
            QueryFieldDesc secure_field = secure_schema.getField(j);
            memcpy(dst + dst_pos, src + src_pos, secure_field.size() * sizeof(emp::block));
            dst_pos += secure_field.size();
            src_pos += plain_field.size();
        }
        // handle dummy tag
        memcpy(dst + dst_pos, src + src_pos,  sizeof(emp::block));
        src_pos += 8; // original, byte-aligned bool
        dst_pos += 1; // emp::Bit, semantically a bool
    }



     std::shared_ptr<SecureTable> additionalInputs = SecureTable::deserialize(secure_schema,
                                                                            downsized_bits.bits);


     delete [] bools;
     return additionalInputs;
  /*  if(!inputTableInit) {
        inputTable = additionalInputs;
    }
    else {
        resizeAndAppend(additionalInputs);
    }


    inputTableInit = true;
    */

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
    
    std::shared_ptr<SecureTable> remote = UnionHybridData::readSecretSharedInput(secretSharesFile, schema, party);

    Union<emp::Bit> union_op(local, remote);
    return union_op.run();

}




