#include <cstddef>
#include <assert.h>
#include <data/psql_data_provider.h>
#include <sys/stat.h>
#include <operators/union.h>
#include "data_utilities.h"
#include <algorithm>


using namespace vaultdb;

unsigned char DataUtilities::reverse(unsigned char b) {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}







// in some cases, like with LIMIT, we can't just run over tpch_unioned
std::shared_ptr<PlainTable>
DataUtilities::getUnionedResults(const std::string &aliceDb, const std::string &bobDb, const std::string &sql,
                                 const bool &hasDummyTag, const size_t & limit) {

    PsqlDataProvider dataProvider;

    std::shared_ptr<PlainTable> alice = dataProvider.getQueryTable(aliceDb, sql,
                                                                   hasDummyTag); // dummyTag true not yet implemented
    std::shared_ptr<PlainTable> bob = dataProvider.getQueryTable(bobDb, sql, hasDummyTag);

    if(limit > 0) {
        alice->resize(limit);
        bob->resize(limit);
    }

    Union<bool> result(alice, bob);
    return result.run();
}



 std::shared_ptr<PlainTable > DataUtilities::getQueryResults(const std::string & dbName, const std::string & sql, const bool & hasDummyTag) {
    PsqlDataProvider dataProvider;
    return dataProvider.getQueryTable(dbName, sql, hasDummyTag);
}

std::string DataUtilities::queryDatetime(const string &colName) {
    return "CAST(EXTRACT(epoch FROM " + colName + ") AS BIGINT) " + colName; // last colName for aliasing
}




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
    auto output_pos = output->begin();
    for(auto input_pos = input->begin(); input_pos != input->end(); ++input_pos) {
        if(!input_pos->getDummyTag()) {
            *output_pos = *input_pos;
            ++output_pos;
        }
    }

    return output;
}

std::shared_ptr<PlainTable >
DataUtilities::getExpectedResults(const string &dbName, const string &sql, const bool &hasDummyTag,
                                  const int &sortColCount) {

    std::shared_ptr<PlainTable > expected = DataUtilities::getQueryResults(dbName, sql, hasDummyTag);
    SortDefinition expectedSortOrder = DataUtilities::getDefaultSortDefinition(sortColCount);
    expected->setSortOrder(expectedSortOrder);
    return expected;
}


std::string DataUtilities::printSortDefinition(const SortDefinition &sortDefinition) {
    std::stringstream  result;
    result << "{";
    bool init = false;
    for(ColumnSort c : sortDefinition) {
        if(init)
            result << ", ";
        string direction = (c.second == SortDirection::ASCENDING) ? "ASC" : "DESC";
        result << "<" << c.first << ", "
                  << direction << "> ";

        init = true;
    }

    result << "}";
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

size_t DataUtilities::get_tuple_cnt(const string &db_name, const string &sql, bool has_dummy_tag) {
    if(has_dummy_tag)  { // run it and count
        shared_ptr<PlainTable> res = DataUtilities::getQueryResults(db_name, sql, true);
        return res->getTrueTupleCount();
    }

    string query = "SELECT COUNT(*) FROM (" + sql + ") q";
    shared_ptr<PlainTable> res = DataUtilities::getQueryResults(db_name, query, false);
    return res->getTuple(0).getField(0).getValue<int64_t>();

}

vector<string> DataUtilities::readTextFile(const string &filename) {
    std::vector<std::string> lines;
    std::ifstream inFile(filename);
    std::string line;


    if(!inFile)
    {
        string cwd = Utilities::getCurrentWorkingDirectory();
        throw std::invalid_argument("Unable to open file: " + filename + " from " + cwd);
    }


    while (std::getline(inFile, line))
    {
        lines.push_back(line);
    }

    return lines;
}

string DataUtilities::readTextFileToString(const string & filename) {
    ifstream input_file(filename);

    if(!input_file.is_open())
    {
        string cwd = Utilities::getCurrentWorkingDirectory();
        throw std::invalid_argument("Unable to open file: " + filename + " from " + cwd);
    }

    return string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}

// batch_no is zero_-indexed
vector<string> DataUtilities::readTextFileBatch(const string &filename, const size_t & batch_tuple_cnt, const size_t & batch_no) {
    std::vector<std::string> lines;
    std::ifstream inFile(filename);
    std::string line;


    if(!inFile)
    {
        string cwd = Utilities::getCurrentWorkingDirectory();
        throw std::invalid_argument("Unable to open file: " + filename + " from " + cwd);
    }

    // count lines in file
    size_t line_cnt = std::count(std::istreambuf_iterator<char>(inFile),
               std::istreambuf_iterator<char>(), '\n');

    size_t max_batches = line_cnt / batch_tuple_cnt;
    if(max_batches > batch_no) {
        throw std::invalid_argument("batch no out of range, expected between [0, " + std::to_string(max_batches) + "], received " + std::to_string(batch_no));
    }

    inFile.seekg(std::ios::beg);
    size_t cursor = batch_no * batch_tuple_cnt;

    for(size_t i = 0; i < cursor; ++i) {
        std::getline(inFile, line);
    }

    for(size_t i = 0; i < batch_tuple_cnt; ++i) {
        if(!std::getline(inFile, line))
            break;
        lines.push_back(line);
    }

    return lines;
}


bool DataUtilities::isOrdinal(const string &s) {
    return !s.empty() && std::find_if(s.begin(),
                                      s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}




