#include <util/utilities.h>
#include "CsvReader.h"
#include <boost/algorithm/string.hpp>
#include <field/field.h>
#include <field/field_factory.h>
#include <query_table.h>
#include <plain_tuple.h>

using namespace vaultdb;

std::unique_ptr<PlainTable> CsvReader::readCsv(const string &filename, const QuerySchema &schema) {

    std::vector<std::string> tupleEntries = readFile(filename);

    // replace dates with LONG
    QuerySchema dst_schema(schema);
    for(size_t i = 0; i < schema.getFieldCount(); ++i) {
        if(schema.getField(i).getType() == FieldType::DATE) {
            QueryFieldDesc dst_field_desc(schema.getField(i), FieldType::LONG);
            dst_schema.putField(dst_field_desc);
        }
    }

    std::cout << "Rewrote schema " << std::endl;
    std::unique_ptr<PlainTable> result(new PlainTable(tupleEntries.size(), dst_schema));
    int cursor = 0;

    for(std::string line : tupleEntries) {
        parseTuple(line, schema, result, cursor);
        std::cout << "Parsed tuple " << result->getTuple(cursor) <<  std::endl << " from "  << line << std::endl;
        ++cursor;

    }

    return result;

}

std::vector<std::string> CsvReader::readFile(const string &filename) {
    std::vector<std::string> csvLines;
    std::ifstream inFile(filename);
    std::string line;


    if(!inFile)
    {
        string cwd = Utilities::getCurrentWorkingDirectory();
        throw std::invalid_argument("Unable to open file: " + filename + " from " + cwd);
    }


    while (std::getline(inFile, line))
    {
        csvLines.push_back(line);
    }

    return csvLines;
}



vector<string> CsvReader::split(const string &tupleEntry) {
    std::vector<std::string> tokens, result;
    std::string latestEntry;
    boost::split(tokens, tupleEntry,boost::is_any_of(","));

    // make a pass and merge any strings
    for(size_t i = 0; i < tokens.size(); ++i) {

        std::string token = tokens[i];
        if(token[0] == '"' && latestEntry.empty()) {  // starts with " and the beginning of an entry
            latestEntry =  token;

        }
        else if(latestEntry.empty()) {
            result.push_back(token);
        }
        else  {
            latestEntry += "," + token;
            if(*(--token.end()) == '"') { // if it is the end of the string
                result.push_back(latestEntry.substr(1, latestEntry.length() - 2)); // chop off leading and trailing double-quotes
                latestEntry.clear();
            }
        }
    } // end for

    return result;
}

void CsvReader::parseTuple(const std::string &csvLine, const QuerySchema &src_schema, std::unique_ptr<PlainTable> &dst,
                           const size_t &tupleIdx) {

    std::vector<std::string> tupleFields;
    std::shared_ptr<QuerySchema> schema = dst->getSchema();
    size_t fieldCount = schema->getFieldCount();


    tupleFields = split(csvLine);
    assert(fieldCount == tupleFields.size()); // verify the field count
    PlainTuple newTuple = dst->getTuple(tupleIdx);

    for(size_t i = 0; i < fieldCount; ++i) {
        PlainField field = FieldFactory<bool>::getFieldFromString(src_schema.getField(i).getType(), schema->getField(i).getStringLength(), tupleFields[i]);
        newTuple.setField(i, field);
        std::cout << "Tuple: " << newTuple << std::endl;

    }

}


