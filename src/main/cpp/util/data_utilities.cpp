#include <cstddef>
#include <assert.h>
#include <data/PsqlDataProvider.h>
#include <sys/stat.h>
#include "data_utilities.h"


#ifndef PATH_MAX
#define PATH_MAX (4096)
#endif

using namespace vaultdb;

unsigned char DataUtilities::reverse(unsigned char b) {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}


signed char DataUtilities::boolsToByte(const bool *src) {
    signed char dst = 0;

    for(int i = 0; i < 8; ++i) {
        dst |= (src[i] << i);
    }

    return dst;
}

bool *DataUtilities::bytesToBool(int8_t *bytes, int byteCount) {
    bool *ret = new bool[byteCount * 8];


    bool *writePos = ret;

    for(int i = 0; i < byteCount; ++i) {
        uint8_t b = bytes[i];
        for(int j = 0; j < 8; ++j) {
            *writePos = ((b & (1<<j)) != 0);
            ++writePos;
        }
    }
    return ret;
}

vector<int8_t> DataUtilities::boolsToBytes(const bool *const src, const uint32_t &bitCount) {
    int byteCount = bitCount / 8;
    assert(bitCount % 8 == 0); // no partial bytes supported

    std::vector<int8_t> result;
    result.resize(byteCount);

    bool *cursor = const_cast<bool*>(src);

    for(int i = 0; i < byteCount; ++i) {
        result[i] = boolsToByte(cursor);
        cursor += 8;
    }

    return result;

}



// in some cases, like with LIMIT, we can't just run over tpch_unioned
std::unique_ptr<QueryTable>
DataUtilities::getUnionedResults(const std::string &aliceDb, const std::string &bobDb, const std::string &sql,
                                 const bool &hasDummyTag) {

    PsqlDataProvider dataProvider;

    std::unique_ptr<QueryTable> alice = dataProvider.getQueryTable(aliceDb, sql, hasDummyTag); // dummyTag true not yet implemented
    std::unique_ptr<QueryTable> bob = dataProvider.getQueryTable(bobDb, sql, hasDummyTag);


    uint32_t tupleCount = alice->getTupleCount() + bob->getTupleCount();

    std::unique_ptr<QueryTable> unioned(new QueryTable(tupleCount, alice->getSchema().getFieldCount()));
    unioned->setSchema(alice->getSchema());

    for(int i = 0; i < alice->getTupleCount(); ++i) {
        unioned->putTuple(i, alice->getTuple(i));
    }

    int offset = alice->getTupleCount();

    // add bob's tuples from last to first
    int readIdx = bob->getTupleCount();
    for(int i = 0; i < bob->getTupleCount(); ++i) {
        --readIdx;
        unioned->putTuple(i + offset, bob->getTuple(readIdx));
    }

    QuerySchema schema = alice->getSchema();
    unioned->setSchema(schema);


    return unioned;
}


 std::shared_ptr<QueryTable> DataUtilities::getQueryResults(const std::string & dbName, const std::string & sql, const bool & hasDummyTag) {
    PsqlDataProvider dataProvider;
    return dataProvider.getQueryTable(dbName, sql, hasDummyTag);
}

std::string DataUtilities::queryDatetime(const string &colName) {
    return "CAST(EXTRACT(epoch FROM " + colName + ") AS BIGINT) " + colName; // last colName for aliasing
}


void DataUtilities::locallySecretShareTable(const std::unique_ptr<QueryTable> &table, const string &aliceFile,
                                           const string &bobFile) {
    SecretShares shares = table->generateSecretShares();
    writeFile(aliceFile, shares.first);
    writeFile(bobFile, shares.second);


}


void DataUtilities::writeFile(std::string fileName, vector<int8_t> contents) {
    std::ofstream outFile(fileName.c_str(), std::ios::out | std::ios::binary);
    if(!outFile.is_open()) {
        throw std::invalid_argument("Could not write output file " + fileName);
    }
    outFile.write((char *) contents.data(), contents.size());
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

std::shared_ptr<QueryTable> DataUtilities::removeDummies(const std::shared_ptr<QueryTable> &input) {
    // only works for plaintext tables
    assert(!input->isEncrypted());
    int outputTupleCount = input->getTrueTupleCount();

    int writeCursor = 0;
    std::shared_ptr<QueryTable> output(new QueryTable(outputTupleCount, input->getSchema(), input->getSortOrder()));
    output->setSchema(input->getSchema());
    output->setSortOrder(input->getSortOrder());

    for(int i = 0; i < input->getTupleCount(); ++i) {
        QueryTuple *tuple = input->getTuplePtr(i);
        if(!tuple->getDummyTag().getBool()) {
            output->putTuple(writeCursor, *tuple);
            ++writeCursor;
        }
    }

    return output;
}

std::shared_ptr<QueryTable>
DataUtilities::getExpectedResults(const string &dbName, const string &sql, const bool &hasDummyTag,
                                  const int &sortColCount) {

    std::shared_ptr<QueryTable> expected = DataUtilities::getQueryResults(dbName, sql, false);
    SortDefinition expectedSortOrder = DataUtilities::getDefaultSortDefinition(sortColCount);
    expected->setSortOrder(expectedSortOrder);
    return expected;
}

std::string DataUtilities::getCurrentWorkingDirectory() {
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    std::string  currentWorkingDirectory = std::string(cwd);
    std::string suffix = currentWorkingDirectory.substr(currentWorkingDirectory.length() - 4, 4);
    if(suffix == std::string("/bin")) {
        currentWorkingDirectory = currentWorkingDirectory.substr(0, currentWorkingDirectory.length() - 4);
    }

    return currentWorkingDirectory;
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

    assert(byteCount > 0 && byteCount <= bytes.size());

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

    vector<int8_t> decodedBytes = boolsToBytes(bitString);
    return printFirstBytes(decodedBytes, byteCount);

}

vector<int8_t> DataUtilities::boolsToBytes( string &bitString) {
    int srcBits = bitString.length();
    string::iterator strPos = bitString.begin();
    bool *bools = new bool[srcBits];

    for(int i =  0; i < srcBits; ++i) {
        bools[i] = (*strPos == '1');
        ++strPos;
    }

    vector<int8_t> decodedBytesVector = DataUtilities::boolsToBytes(bools, srcBits);
    delete[] bools;
    return decodedBytesVector;
}

// From Chenkai Li's EMP memory instrumentation

void DataUtilities::checkMemoryUtilization() {
#if defined(__linux__)
    struct rusage rusage;
	if (!getrusage(RUSAGE_SELF, &rusage))
		std::cout << "[Linux]Peak resident set size: " << (size_t)rusage.ru_maxrss <<  " bytes." << std::endl;
	else std::cout << "[Linux]Query RSS failed" << std::endl;
#elif defined(__APPLE__)
    struct mach_task_basic_info info;
    mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &count) == KERN_SUCCESS)
        std::cout << "[Mac]Peak resident set size: " << (size_t)info.resident_size_max << " bytes, current memory size: " << (size_t)info.resident_size  <<  std::endl;
    else std::cout << "[Mac]Query RSS failed" << std::endl;
#endif

}
