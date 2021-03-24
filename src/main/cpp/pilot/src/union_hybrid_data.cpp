#include <pilot/src/common/shared_schema.h>
#include <data/CsvReader.h>
#include "union_hybrid_data.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <util/data_utilities.h>


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
    std::unique_ptr<PlainTable> localInput = CsvReader::readCsv(localInputFile, inputTable->getSchema());
    std::shared_ptr<SecureTable> encryptedTable = localInput->secretShare(netio, party);

    Utilities::checkMemoryUtilization("before local read: ");
    if(!inputTableInit) {
        inputTable = encryptedTable;
    }
    else {
       resizeAndAppend(encryptedTable);
    }


    PlainTuple test = (*inputTable)[0].reveal();
    std::cout << "Sanity check: " << test << std::endl;

    Utilities::checkMemoryUtilization("after local read: ");

    inputTableInit = true;
}





void UnionHybridData::readSecretSharedInput(const string &secretSharesFile) {

    // read in binary and then xor it with other side to secret share it.
    std::cout << "Reading " << secretSharesFile << std::endl;
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

    std::cout << "****First bits: ";
    for(int i = 0; i < 56; ++i) {
        std::cout << additionalData.bits[i].reveal();
        if((i+1) % 8 == 0) std::cout << " ";
    }

    std::cout  << std::endl;

     std::shared_ptr<SecureTable> additionalInputs = SecureTable::deserialize(inputTable->getSchema(),
                                                                            additionalData.bits);

    PlainTuple test = (*additionalInputs)[0].reveal();
    std::cout << "Sanity check 2: " << test << std::endl;

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

    std::cout << "Appending at idx: " << writeIdx << std::endl;

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
    shared_ptr<PlainTable> revealed = unioned.getInputTable()->reveal();

    unioned.readSecretSharedInput(secretSharesFile);
    revealed = unioned.getInputTable()->reveal();

    PlainTuple test = (*unioned.getInputTable())[0].reveal();
    std::cout << "Sanity check 3: " << test << std::endl;


    return unioned.getInputTable();
}




