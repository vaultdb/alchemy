#include <pilot/src/common/shared_schema.h>
#include <data/CsvReader.h>
#include "union_hybrid_data.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <util/data_utilities.h>
#include <query_table/plain_tuple.h>
#include <query_table/secure_tuple.h>


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

void UnionHybridData::readLocalInput(const string &localInputFile) {
    std::unique_ptr<PlainTable> localInput = CsvReader::readCsv(localInputFile, *inputTable->getSchema());
    Utilities::checkMemoryUtilization(" read csv: ");
    std::cout << "Read " << localInput->getTupleCount() << " tuples of local input." << std::endl;

    std::shared_ptr<SecureTable> encryptedTable = localInput->secret_share(netio, party);

    if(!inputTableInit) {
        inputTable = encryptedTable;
    }
    else {
       resizeAndAppend(encryptedTable);
    }


    Utilities::checkMemoryUtilization(" local read: ");

    inputTableInit = true;
}





void UnionHybridData::readSecretSharedInput(const string &secretSharesFile) {

    // read in binary and then xor it with other side to secret share it.
    std::vector<int8_t> srcData = DataUtilities::readFile(secretSharesFile);

    size_t srcBytes = srcData.size();
    size_t srcBits = srcBytes * 8;
    bool *bools = Utilities::bytesToBool(srcData.data(), srcBytes);

    Integer aliceBytes(srcBits, 0L, ALICE);
    Integer bobBytes(srcBits, 0L, BOB);

    if(party == ALICE) {
        // feed through my data, then wait for Bob's
        ProtocolExecution::prot_exec->feed((block *)aliceBytes.bits.data(), ALICE, bools, srcBits);
        ProtocolExecution::prot_exec->feed((block *)bobBytes.bits.data(), BOB, nullptr, srcBits);
    }
    else {
        ProtocolExecution::prot_exec->feed((block *)aliceBytes.bits.data(), ALICE, nullptr, srcBits);
        ProtocolExecution::prot_exec->feed((block *)bobBytes.bits.data(), BOB, bools, srcBits);

    }

    Integer additionalData = aliceBytes ^ bobBytes;


     std::shared_ptr<SecureTable> additionalInputs = SecureTable::deserialize(*inputTable->getSchema(),
                                                                            additionalData.bits);


    if(!inputTableInit) {
        inputTable = additionalInputs;
    }
    else {
        resizeAndAppend(additionalInputs);
    }


    inputTableInit = true;
    delete [] bools;


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
    UnionHybridData unioned(schema, aNetIO, party);

    unioned.readLocalInput(localInputFile);

    unioned.readSecretSharedInput(secretSharesFile);



    return unioned.getInputTable();
}




