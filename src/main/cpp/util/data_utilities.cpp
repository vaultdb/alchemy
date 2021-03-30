#include <cstddef>
#include <assert.h>
#include <data/PsqlDataProvider.h>
#include <sys/stat.h>
#include "data_utilities.h"


using namespace vaultdb;

unsigned char DataUtilities::reverse(unsigned char b) {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}







// in some cases, like with LIMIT, we can't just run over tpch_unioned
std::unique_ptr<PlainTable >
DataUtilities::getUnionedResults(const std::string &aliceDb, const std::string &bobDb, const std::string &sql,
                                 const bool &hasDummyTag) {

    PsqlDataProvider dataProvider;

    std::unique_ptr<PlainTable> alice = dataProvider.getQueryTable(aliceDb, sql, hasDummyTag); // dummyTag true not yet implemented
    std::unique_ptr<PlainTable> bob = dataProvider.getQueryTable(bobDb, sql, hasDummyTag);


    uint32_t tupleCount = alice->getTupleCount() + bob->getTupleCount();

    std::unique_ptr<PlainTable > unioned(new PlainTable(tupleCount, *alice->getSchema()));
    unioned->setSchema(*alice->getSchema());

    for(size_t i = 0; i < alice->getTupleCount(); ++i) {
        unioned->putTuple(i, alice->getTuple(i));
    }

    size_t offset = alice->getTupleCount();

    // add bob's tuples from last to first
    size_t readIdx = bob->getTupleCount();
    for(size_t i = 0; i < bob->getTupleCount(); ++i) {
        --readIdx;
        unioned->putTuple(i + offset, bob->getTuple(readIdx));
    }

    QuerySchema schema = *alice->getSchema();
    unioned->setSchema(schema);


    return unioned;
}


 std::shared_ptr<PlainTable > DataUtilities::getQueryResults(const std::string & dbName, const std::string & sql, const bool & hasDummyTag) {
    PsqlDataProvider dataProvider;
    return dataProvider.getQueryTable(dbName, sql, hasDummyTag);
}

std::string DataUtilities::queryDatetime(const string &colName) {
    return "CAST(EXTRACT(epoch FROM " + colName + ") AS BIGINT) " + colName; // last colName for aliasing
}


/*
void DataUtilities::locallySecretShareTable(const std::unique_ptr<QueryTable> &table, const string &aliceFile,
                                           const string &bobFile) {
    SecretShares shares = table->generateSecretShares();
    writeFile(aliceFile, shares.first);
    writeFile(bobFile, shares.second);


}*/



void DataUtilities::writeFile(const string &fileName, vector<int8_t> contents) {
    std::ofstream outFile(fileName.c_str(), std::ios::out | std::ios::binary);
    if(!outFile.is_open()) {
        throw std::invalid_argument("Could not write output file " + fileName);
    }
    outFile.write((char *) contents.data(), contents.size());
    outFile.close();
}

void DataUtilities::writeFile(const string &fileName, const string &contents) {
    std::ofstream outFile(fileName.c_str(), std::ios::out);
    if(!outFile.is_open()) {
        throw std::invalid_argument("Could not write output file " + fileName);
    }
    outFile.write(contents.c_str(), contents.size());
    outFile.close();
}


// reads binary file
vector<int8_t> DataUtilities::readFile(const std::string & fileName) {
    // read in binary and then xor it with other side to secret share it.
    // get file size
    struct stat fileStats;
    size_t fileSize; // bytes
    if (stat(fileName.c_str(), &fileStats) == 0) {
        fileSize = fileStats.st_size;
    }
    else {
        throw std::invalid_argument("Can't open input file " + fileName + "!");
    }


    std::vector<int8_t> fileBytes;
    fileBytes.resize(fileSize);
    int8_t  *shares = fileBytes.data();
    std::ifstream inputFile(fileName, std::ios::in | std::ios::binary);
    inputFile.read((char *) shares, fileSize);
    inputFile.close();

    return fileBytes;

}

SortDefinition DataUtilities::getDefaultSortDefinition(const uint32_t &colCount) {
        SortDefinition  sortDefinition;

        for(uint32_t i = 0; i < colCount; ++i) {
            sortDefinition.push_back(ColumnSort(i, SortDirection::ASCENDING));
        }

        return sortDefinition;

    }

std::shared_ptr<PlainTable > DataUtilities::removeDummies(const std::shared_ptr<PlainTable> &input) {
    // only works for plaintext tables
    assert(!input->isEncrypted());
    int outputTupleCount = input->getTrueTupleCount();

    int writeCursor = 0;
    std::shared_ptr<PlainTable > output(new PlainTable(outputTupleCount, *input->getSchema(), input->getSortOrder()));

    for(size_t i = 0; i < input->getTupleCount(); ++i) {
        PlainTuple tuple = input->getTuple(i);
        if(!tuple.getDummyTag()) {
            output->putTuple(writeCursor, tuple);
            ++writeCursor;
        }
    }

    return output;
}

std::shared_ptr<PlainTable >
DataUtilities::getExpectedResults(const string &dbName, const string &sql, const bool &hasDummyTag,
                                  const int &sortColCount) {

    std::shared_ptr<PlainTable > expected = DataUtilities::getQueryResults(dbName, sql, false);
    SortDefinition expectedSortOrder = DataUtilities::getDefaultSortDefinition(sortColCount);
    expected->setSortOrder(expectedSortOrder);
    return expected;
}


std::string DataUtilities::printSortDefinition(const SortDefinition &sortDefinition) {
    std::stringstream  result;
    result << "(";
    for(ColumnSort c : sortDefinition) {
        string direction = (c.second == SortDirection::ASCENDING) ? "ASC" : "DESC";
        result << "<" << c.first << ", "
                  << direction << "> " <<  std::endl;

    }
    result << ")";
    return result.str();
}


std::string DataUtilities::printFirstBytes(vector<int8_t> &bytes, const int &byteCount) {
    std::stringstream ss;

    assert(byteCount > 0 && (size_t) byteCount <= bytes.size());

    vector<int8_t>::iterator  readPos = bytes.begin();
    ss << (int) *readPos;
    while((readPos - bytes.begin()) < byteCount) {
        ++readPos;
        ss << "," << (int) *readPos;
    }
    return ss.str();

}


std::string DataUtilities::revealAndPrintFirstBytes(vector<Bit> &bits, const int & byteCount) {
    Integer anInt(bits.size(), 0L);
    anInt.bits = bits;
    std::string bitString = anInt.reveal<std::string>(emp::PUBLIC);

    vector<int8_t> decodedBytes = Utilities::boolsToBytes(bitString);
    return printFirstBytes(decodedBytes, byteCount);

}

// actually an array of emp::Bits.  Each bit is sizeof(emp::block) length
/*emp::Integer toEmpInteger(const vector<int8_t> & src_bytes) {

    emp::Integer dst(src_bytes.size() / sizeof(emp::block), 0);

} */




