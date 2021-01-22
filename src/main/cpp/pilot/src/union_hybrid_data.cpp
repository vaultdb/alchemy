#include <pilot/src/common/shared_schema.h>
#include <data/CsvReader.h>
#include "union_hybrid_data.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <util/data_utilities.h>

UnionHybridData::UnionHybridData(const string &localInputFile, const string &secretSharesFile,
                                 NetIO *aNetio, const int & aParty)  : netio(aNetio), party(aParty) {
    QuerySchema inputSchema = SharedSchema::getInputSchema();
    std::unique_ptr<QueryTable> localInput = CsvReader::readCsv(localInputFile, inputSchema);
    std::shared_ptr<QueryTable> inputData = localInput->secretShare(netio, party);

    // read in binary and then xor it with other side to secret share it.

    // get file size
    struct stat fileStats;

    size_t fileSize; // bytes
    if (stat(secretSharesFile.c_str(), &fileStats) == 0) {
        fileSize = fileStats.st_size;
    }
    else {
        throw std::invalid_argument("Can't open input file " + secretSharesFile + "!");
    }

    int8_t  *shares = new int8_t[fileSize];
    ifstream inputFile(secretSharesFile, ios::in | ios::binary);
    inputFile.read((char *) shares, 16);
    inputFile.close();

    Integer aliceBytes = readEncrypted(shares, fileSize, ALICE);
    Integer bobBytes = readEncrypted(shares, fileSize, BOB);
    // still one long bucket of bits.  Need to break it down into tuples
    Integer additionalData = aliceBytes ^ bobBytes;
    delete[] shares;

    // size in bits
    size_t rowSize = inputSchema.size();
    size_t encryptedBitCount = additionalData.size();
    size_t additionalTupleCount = encryptedBitCount / rowSize;

    // TODO: redo this for encrypted case - passing around a pile of Bits instead of int8_ts.
    QueryTable::deserialize(inputData->getSchema(), additionalData.bits);
    inputData->resize(additionalTupleCount);



}

// based on EmpManager::encrypteVarchar
Integer UnionHybridData::readEncrypted(int8_t *secretSharedBits, const size_t &size, const int & dstParty) {


        bool *bools = DataUtilities::bytesToBool(secretSharedBits, size);

        Integer result(size * 8, 0L, dstParty);
        if(party == dstParty) {
            ProtocolExecution::prot_exec->feed((block *)result.bits.data(), dstParty, bools, size * 8);
        }
        else {
            ProtocolExecution::prot_exec->feed((block *)result.bits.data(), dstParty, nullptr, size * 8);
        }


        delete [] bools;


        return result;

    }





