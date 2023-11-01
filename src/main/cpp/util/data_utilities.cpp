#include <assert.h>
#include <data/psql_data_provider.h>
#include <sys/stat.h>
#include <operators/union.h>
#include "data_utilities.h"
#include <algorithm>
#include <query_table/query_table.h>


using namespace vaultdb;









// in some cases, like with LIMIT, we can't just run over tpch_unioned
PlainTable *
DataUtilities::getUnionedResults(const std::string &alice_db, const std::string &bob_db, const std::string &sql,
                                 const bool &has_dummy_tag, const size_t &limit) {

    PsqlDataProvider dataProvider;

    PlainTable *alice = dataProvider.getQueryTable(alice_db, sql,
                                                   has_dummy_tag); // dummyTag true not yet implemented
    PlainTable *bob = dataProvider.getQueryTable(bob_db, sql, has_dummy_tag);

    if(limit > 0) {
        alice->resize(limit);
        bob->resize(limit);
    }

    Union<bool> result(alice, bob);

    // result will delete its output in destructor.  Need to clone!
    PlainTable  *p = result.run();
    return p->clone();

}



PlainTable *DataUtilities::getQueryResults(const std::string &dbName, const std::string &sql,
                                           const bool &has_dummy_tag) {
    PsqlDataProvider dataProvider;
    return dataProvider.getQueryTable(dbName, sql, has_dummy_tag);
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
vector<int8_t> DataUtilities::readFile(const std::string & filename) {
    // read in binary and then xor it with other side to secret share it.
    // get file size
    struct stat fileStats;
    size_t file_size; // bytes
    if (stat(filename.c_str(), &fileStats) == 0) {
        file_size = fileStats.st_size;
    }
    else {
        throw std::invalid_argument("Can't open input file " + filename + "!");
    }


    std::vector<int8_t> file_bytes;
    file_bytes.resize(file_size);
    int8_t  *shares = file_bytes.data();
    std::ifstream input(filename, std::ios::in | std::ios::binary);
    input.read((char *) shares, file_size);
    input.close();
    return file_bytes;

}

SortDefinition DataUtilities::getDefaultSortDefinition(const uint32_t &colCount) {
        SortDefinition  sortDefinition;

        for(uint32_t i = 0; i < colCount; ++i) {
            sortDefinition.push_back(ColumnSort(i, SortDirection::ASCENDING));
        }

        return sortDefinition;

    }

void DataUtilities::removeDummies(PlainTable *table) {
    // only works for plaintext tables
    int out_tuple_cnt = table->getTrueTupleCount();
    if(out_tuple_cnt == table->getTupleCount()) return;

    int write_cursor = 0;

    for(int i = 0; i < table->getTupleCount(); ++i) {
        if(!table->getDummyTag(i)) {
            table->cloneRow(write_cursor, 0, table, i );
            table->setDummyTag(write_cursor, false);
            ++write_cursor;
        }
        if(write_cursor == out_tuple_cnt) break;
    }

    table->resize(out_tuple_cnt);

}

PlainTable *
DataUtilities::getExpectedResults(const string &db_name, const string &sql, const bool &has_dummy_tag,
                                  const int &sort_col_cnt) {

    PlainTable *expected = DataUtilities::getQueryResults(db_name, sql, has_dummy_tag);
    SortDefinition expected_sort = DataUtilities::getDefaultSortDefinition(sort_col_cnt);
    expected->setSortOrder(expected_sort);
    return expected;
}


std::string DataUtilities::printSortDefinition(const SortDefinition &sortDefinition) {
    std::stringstream  result;
    result << "{";
    bool init = false;
    for(ColumnSort c : sortDefinition) {
        if(init)
            result << ", ";
        string direction = (c.second == SortDirection::ASCENDING) ? "ASC" : (c.second == SortDirection::DESCENDING) ? "DESC" : "INVALID";
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
    std::string bitString = SystemConfiguration::getInstance().emp_manager_->revealToString(anInt);

    vector<int8_t> decodedBytes = Utilities::boolsToBytes(bitString);
    return printFirstBytes(decodedBytes, byteCount);

}

size_t DataUtilities::getTupleCount(const string &db_name, const string &sql, bool has_dummy_tag) {
    if(has_dummy_tag)  { // run it and count
        PlainTable *res = DataUtilities::getQueryResults(db_name, sql, true);
        int cnt =  res->getTrueTupleCount();
        delete res;
        return cnt;
    }

    string query = "SELECT COUNT(*) FROM (" + sql + ") q";
    PlainTable *res = DataUtilities::getQueryResults(db_name, query, false);
    int cnt = res->getField(0,0).getValue<int64_t>();
    delete res;
    return cnt;

}

vector<string> DataUtilities::readTextFile(const string &filename) {
    std::vector<std::string> lines;
    std::ifstream input(filename);
    std::string line;


    if(!input)
    {
        string cwd = Utilities::getCurrentWorkingDirectory();
        throw std::invalid_argument("Unable to open file: " + filename + " from " + cwd);
    }


    while (std::getline(input, line))
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

string DataUtilities::printByteArray(const int8_t *bytes, const size_t &byte_cnt) {
    stringstream  ss;
    ss << "(";
    for(int i = 0; i < byte_cnt; ++i) {
        ss << (int) bytes[i];
        if(i < (byte_cnt - 1)) ss << ", ";
    }
    ss << ")";
    return ss.str();
}

bool DataUtilities::verifyCollation(PlainTable *sorted) {
    SortDefinition  collation = sorted->getSortOrder();
    // delete dummies
    int true_card = sorted->getTrueTupleCount();
    auto no_dummies = new QueryTable<bool>(true_card, sorted->getSchema(), sorted->getSortOrder());
    int cursor = 0;
    for(int i = 0; i < sorted->getTupleCount(); ++i) {
        if(!sorted->getDummyTag(i)) {
            no_dummies->cloneRow(cursor, 0, sorted, i);
            no_dummies->setDummyTag(cursor, false);
            ++cursor;
        }
    }


    for(int i = 1; i < no_dummies->getTupleCount(); ++i) {
//        cout << "Comparing row " << no_dummies.getPlainTuple(i-1) << " with " << no_dummies.getPlainTuple(i) << endl;
        for(auto col_sort : collation) {
                auto lhs_field = no_dummies->getField(i-1, col_sort.first);
                auto rhs_field = no_dummies->getField(i, col_sort.first);
//                cout << "   (" << col_sort.first << ", "
//                << ((col_sort.second == SortDirection::ASCENDING) ? "ASC" : "DESC" ) << ") "
//                <<  "Comparing " << lhs_field << " with " << rhs_field << endl;
                if(lhs_field == rhs_field) continue;

                if(col_sort.second == SortDirection::ASCENDING)
                    if(lhs_field > rhs_field) return false;
                        else break;
                else  // DESC
                    if(lhs_field < rhs_field)  return false;
                        else break;
            }
        }// each tuple correctly ordered wrt its predecessor

    delete no_dummies;
    return true;
}

bool DataUtilities::verifyCollation(SecureTable *sorted) {
    auto tmp = sorted->revealInsecure();
    bool verified = verifyCollation(tmp);
    delete tmp;
    return verified;
}

string DataUtilities::printBitArray(const int8_t *bits, const size_t &byte_cnt) {
    std::stringstream  s;
    for(int i = 0; i < byte_cnt; ++i) {
        int8_t b = bits[i];
        for(int j = 0; j < 8; ++j) {
            s << ((b & (1<<j)) != 0);
        }
        s << " ";

    }
    return s.str();
}

string DataUtilities::printTable(const SecureTable *table, int tuple_limit, bool show_dummies) {
    if(tuple_limit <= 0 || tuple_limit > table->getTupleCount()) {
        auto tmp = table->revealInsecure();
        stringstream ss;
        ss << tmp->toString(tuple_limit, show_dummies);
        delete tmp;
        return ss.str();
    }

    stringstream ss;
    int tuples_written = 0;
    size_t cursor = 0;

    QuerySchema plain_schema = QuerySchema::toPlain(table->getSchema());
    while((cursor < table->tuple_cnt_) && (tuples_written < tuple_limit)) {
        PlainTuple tuple = table->revealRow(cursor, plain_schema);
        if(show_dummies  // print unconditionally
           || !tuple.getDummyTag()) {
            ss << cursor << ": " << tuple.toString(show_dummies) << std::endl;
            ++tuples_written;
        }
        ++cursor;
    }
    return ss.str();
}

// to make templated code compile
string DataUtilities::printTable(const PlainTable *table, int tuple_limit, bool show_dummies) {
    stringstream  ss;
    ss << table->toString(tuple_limit, show_dummies);
    return ss.str();
}
