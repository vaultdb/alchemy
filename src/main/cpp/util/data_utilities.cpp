#include <assert.h>
#include <data/psql_data_provider.h>
#include <sys/stat.h>
#include <operators/union.h>
#include "data_utilities.h"
#include <algorithm>
#include <query_table/query_table.h>

#include <algorithm>

using namespace vaultdb;

// in some cases, like with LIMIT, we can't just run over tpch_unioned
PlainTable *
DataUtilities::getUnionedResults(const std::string &alice_db, const std::string &bob_db, const std::string &sql,
                                 const bool &has_dummy_tag, const size_t &limit) {

    PsqlDataProvider dataProvider;

    PlainTable *alice = dataProvider.getQueryTable(alice_db, sql, has_dummy_tag); // dummyTag true not yet implemented
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



PlainTable *DataUtilities::getQueryResults(const std::string &db_name, const std::string &sql,
                                           const bool &has_dummy_tag) {
    PsqlDataProvider dataProvider;
    return dataProvider.getQueryTable(db_name, sql, has_dummy_tag);
}

void DataUtilities::runQueryNoOutput(const std::string &db_name, const std::string &sql) {
    PsqlDataProvider data_provider;
    data_provider.runQuery(db_name, sql);
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

void DataUtilities::appendFile(const string &fileName, vector<int8_t> contents) {
    std::ofstream outFile(fileName.c_str(), std::ios::binary | std::ios::app);
    if(!outFile.is_open()) {
        throw std::invalid_argument("Could not write output file " + fileName);
    }
    outFile.write((char *) contents.data(), contents.size());
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
    if(out_tuple_cnt == table->tuple_cnt_) return;
    int write_cursor = 0;

    for(int i = 0; i < table->tuple_cnt_; ++i) {
        if(!table->getDummyTag(i)) {
            table->cloneRow(write_cursor, 0, table, i );
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
    expected->order_by_ = expected_sort;
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
    if(filename[0] != '/') { // relative path
        string cwd = Utilities::getCurrentWorkingDirectory();
        string full_path = cwd + "/" + filename;
        return readTextFile(full_path);
    }

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
    if(filename[0] != '/') { // relative path
        string cwd = Utilities::getCurrentWorkingDirectory();
        string full_path = cwd + "/" + filename;
        return readTextFileToString(full_path);
    }

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

// plaintext input schema, setup for XOR-shared data
SecureTable *DataUtilities::readSecretSharedInput(const string &secret_shares_input, const QuerySchema &plain_schema, const int & limit) {
    QuerySchema secure_schema = QuerySchema::toSecure(plain_schema);
    // read in binary and then xor it with other side to secret share it.
    std::vector<int8_t> src_data = DataUtilities::readFile(secret_shares_input);
    size_t src_byte_cnt = src_data.size();
    size_t src_bit_cnt = src_byte_cnt * 8;
    size_t tuple_cnt = src_bit_cnt / plain_schema.size();
    size_t tuples_to_read = (limit > 0) ?
                            ( limit < tuple_cnt ? limit : tuple_cnt)
            : tuple_cnt;
    size_t tuples_to_skip = (tuple_cnt > tuples_to_read) ? (tuple_cnt - tuples_to_read) : 0;

    // convert serialized representation from byte-aligned to bit-by-bit
    size_t dst_bit_cnt = tuples_to_read * secure_schema.size();
    bool *dst_bools = new bool[dst_bit_cnt]; // dst_bit_alloc
    bool *dst_cursor = dst_bools;
    int8_t *src_cursor = src_data.data();

    assert(src_bit_cnt % plain_schema.size() == 0);
    for (int i = 0; i < plain_schema.getFieldCount(); ++i) {
        auto plain_field = plain_schema.getField(i);
        auto secure_field = secure_schema.getField(i);

        if (plain_field.size() == secure_field.size()) { // 1:1, just serialize it
            int write_size = secure_schema.getField(i).size() * tuples_to_read;
            int read_size = secure_schema.getField(i).size() * tuple_cnt;
            emp::to_bool<int8_t>(dst_cursor, src_cursor, write_size, false);
            dst_cursor += write_size;
            src_cursor += read_size / 8;
        } else { // bool case
            assert(plain_field.size() == (secure_field.size() + 7)); // only for bools
            for (int j = 0; j < tuples_to_read; ++j) {
                *dst_cursor = ((*src_cursor & 1) != 0); // (bool) *src_cursor;
                ++dst_cursor;
                ++src_cursor;
            } // end for each tuple
            src_cursor += tuples_to_skip;
        }
    } // end for all fields

    // copy dummy tag
    for (int i = 0; i < tuples_to_read; ++i) {
        *dst_cursor = ((*src_cursor & 1) != 0);
        ++dst_cursor;
        ++src_cursor;
    }


    Integer alice(dst_bit_cnt, 0L, emp::PUBLIC);
    Integer bob(dst_bit_cnt, 0L, emp::PUBLIC);
    int party = SystemConfiguration::getInstance().party_;
    EmpManager *manager = SystemConfiguration::getInstance().emp_manager_;

    if (party == ALICE) {
        // feed through Alice's data, then wait for Bob's
        manager->feed(alice.bits.data(), ALICE, dst_bools, dst_bit_cnt);
        manager->feed(bob.bits.data(), BOB, nullptr, dst_bit_cnt);
    } else {
        manager->feed(alice.bits.data(), ALICE, nullptr, dst_bit_cnt);
        manager->feed(bob.bits.data(), BOB, dst_bools, dst_bit_cnt);

    }

    Integer shared_data = alice ^ bob;

    SecureTable *shared_table = QueryTable<Bit>::deserialize(secure_schema, shared_data.bits);

    delete[] dst_bools;
    return shared_table;

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
    SortDefinition  collation = sorted->order_by_;
    // delete dummies
    int true_card = sorted->getTrueTupleCount();
    auto no_dummies = QueryTable<bool>::getTable(true_card, sorted->getSchema(), sorted->order_by_);
    int cursor = 0;
    for(int i = 0; i < sorted->tuple_cnt_; ++i) {
        if(!sorted->getDummyTag(i)) {
            no_dummies->cloneRow(cursor, 0, sorted, i);
            no_dummies->setDummyTag(cursor, false);
            ++cursor;
        }
    }


    for(int i = 1; i < no_dummies->tuple_cnt_; ++i) {
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

string DataUtilities::printTable(SecureTable *table, int tuple_limit, bool show_dummies) {
    stringstream ss;
    // print all
    if(tuple_limit <= 0 || tuple_limit > table->tuple_cnt_) {
        auto tmp = table->revealInsecure();
        ss << tmp->toString(tuple_limit, show_dummies);
        delete tmp;
        return ss.str();
    }
    ss << table->getSchema() << endl;
    int tuples_written = 0;
    size_t cursor = 0;

    while((cursor < table->tuple_cnt_) && (tuples_written < tuple_limit)) {
        PlainTuple tuple = table->revealRow(cursor);
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
