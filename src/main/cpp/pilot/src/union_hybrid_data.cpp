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
    inputTable = std::shared_ptr<QueryTable>(new QueryTable(1, inputSchema, emptySortDefinition));

}

// based on EmpManager::encryptedVarchar
Integer UnionHybridData::readEncrypted(int8_t *secretSharedBits, const size_t &sizeBytes, const int & dstParty) {


        bool *bools = DataUtilities::bytesToBool(secretSharedBits, sizeBytes);
        std::cout << "Allocating " << sizeBytes * 8 << " bits for new integer." << std::endl;

        Integer result(sizeBytes * 8, 0L, dstParty);

// rather than proto-exec, what if we just copy it into the vector?  Nope, already encrypted.
/* Reproducing
 *
 *
    for(int i = 0; i < 4; ++i) {
        aliceShares[i] = (party == ALICE) ? Integer(32, shares[i], ALICE) : Integer(32, 0, ALICE);
    }

    for(int i = 0; i < 4; ++i) {
        bobShares[i] = (party == BOB) ? Integer(32, shares[i], BOB) : Integer(32, 0, BOB);
    }

    for(int i = 0; i < 4; ++i) {
        recoveredShares[i] = aliceShares[i] ^ bobShares[i];
    }
    But we can't just copy it in.
    Semantically, we are saying the plaintext integer is the one in the file.
    We set the Integer equal to it by following the procedure in Integer::Integer
    Then we XOR it later to cancel out the blinding.
    Since we are replicating the logic in encryted_varchar, what does this mean?   Can we use the basic case of public values here?
 */

        if(party == dstParty) {
            ProtocolExecution::prot_exec->feed((block *)result.bits.data(), dstParty, bools, sizeBytes * 8);
        }
        else {
            ProtocolExecution::prot_exec->feed((block *)result.bits.data(), dstParty, nullptr, sizeBytes * 8);
        }


        delete [] bools;

        std::cout << "Result has " << result.bits.size() << " bits." << std::endl;
        return result;

    }

void UnionHybridData::readLocalInput(const string &localInputFile) {
    std::unique_ptr<QueryTable> localInput = CsvReader::readCsv(localInputFile, inputTable->getSchema());
    std::shared_ptr<QueryTable> encryptedTable = localInput->secretShare(netio, party);
    if(!inputTableInit) {
        inputTable = encryptedTable;
    }
    else {
       resizeAndAppend(encryptedTable);
    }

    inputTableInit = true;
}

 string UnionHybridData::printFirstBytes(vector<int8_t> & bytes, const int &  byteCount) {
    stringstream ss;
    assert(byteCount > 0 && byteCount <= bytes.size());
    vector<int8_t>::iterator  readPos = bytes.begin();
    ss << (int) *readPos;
    while((readPos - bytes.begin()) < byteCount) {
        ++readPos;
        ss << "," << (int) *readPos;
    }
    return ss.str();
}


/*std::string revealAndPrintBytes(Integer & i, int & byteCount) {

}*/

void UnionHybridData::readSecretSharedInput(const string &secretSharesFile) {

    // read in binary and then xor it with other side to secret share it.

    std::vector<int8_t> srcData = DataUtilities::readFile(secretSharesFile);

    size_t srcBytes = srcData.size();
    size_t srcBits = srcBytes * 8;
    bool *bools = DataUtilities::bytesToBool(srcData.data(), srcBytes);
    std::cout << "Allocating " << srcBits << " bits for new integer." << std::endl;

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

     std::shared_ptr<QueryTable> additionalInputs = QueryTable::deserialize(inputTable->getSchema(),
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

std::shared_ptr<QueryTable> UnionHybridData::getInputTable() {
    return inputTable;
}

void UnionHybridData::resizeAndAppend(std::shared_ptr<QueryTable> toAdd) {
    size_t oldTupleCount = inputTable->getTupleCount();
    size_t newTupleCount = oldTupleCount + toAdd->getTupleCount();
    inputTable->resize(newTupleCount);

    int writeIdx = oldTupleCount;

    for(int i = 0; i < toAdd->getTupleCount(); ++i) {
        inputTable->putTuple(writeIdx, toAdd->getTuple(i));
        ++writeIdx;
    }

}

shared_ptr<QueryTable> UnionHybridData::unionHybridData(const QuerySchema &schema, const string &localInputFile,
                                                        const string &secretSharesFile, NetIO *aNetIO,
                                                        const int &party) {
    UnionHybridData unioned(schema, aNetIO, party);
    unioned.readLocalInput(localInputFile);
    std::cout << "Local input has " << unioned.getInputTable()->getTupleCount() << " tuples" << std::endl;
    unioned.readSecretSharedInput(secretSharesFile);
    return unioned.getInputTable();
}






/*
 * Scratch:
 *     std::string bitString = additionalData.reveal<std::string>(emp::PUBLIC);
    std::string::iterator strPos = bitString.begin();

    for(int i =  0; i < srcBits; ++i) {
        bools[i] = (*strPos == '1');
        ++strPos;
    }

    vector<int8_t> decodedBytesVector = DataUtilities::boolsToBytes(bools, srcBits);

    // expected:  First plaintext bytes: 2,0,0,0,48,50,56,-60,49,0,0
    // observed:  First plaintext bytes: 2,0,0,0,48,50,56,-60,49,0,0

    /* Bob:
     * First plaintext bytes: 2,0,0,0,48,50,56,-60,49,0,0
       Last bytes: -103, -72, -77

       Alice:
       First plaintext bytes: 2,0,0,0,48,50,56,-60,49,0,0



std::cout << "First plaintext bytes: " << printFirstBytes(decodedBytesVector, 10) << std::endl;
std::cout << "Last bytes: " << (int) decodedBytesVector[decodedBytesVector.size() - 1] << ", " <<  (int) decodedBytesVector[decodedBytesVector.size() - 2] << ", " <<  (int) decodedBytesVector[decodedBytesVector.size() - 3]  << std::endl;
/* Reproducing
*
*
for(int i = 0; i < 4; ++i) {
    aliceShares[i] = (party == ALICE) ? Integer(32, shares[i], ALICE) : Integer(32, 0, ALICE);
}

for(int i = 0; i < 4; ++i) {
    bobShares[i] = (party == BOB) ? Integer(32, shares[i], BOB) : Integer(32, 0, BOB);
}

for(int i = 0; i < 4; ++i) {
    recoveredShares[i] = aliceShares[i] ^ bobShares[i];
}
But we can't just copy it in.
Semantically, we are saying the plaintext integer is the one in the file.
We set the Integer equal to it by following the procedure in Integer::Integer
Then we XOR it later to cancel out the blinding.
Since we are replicating the logic in encryted_varchar, what does this mean?   Can we use the basic case of public values here?

//    Integer aliceBytes = readEncrypted(shares, srcData.size(), ALICE);
//    Integer bobBytes = readEncrypted(shares, srcData.size(), BOB);
// still one long bucket of bits.  Need to break it down into tuples



std::cout << "Schema: " << inputTable->getSchema() << std::endl;
std::cout << "Schema size: " << inputTable->getSchema().size() << " bits." << std::endl;
std::cout << "Read in " << additionalData.size() << " bits, tupleCount = " << additionalData.size() / inputTable->getSchema().size()  << std::endl;

*/