#include <util/type_utilities.h>
#include <util/data_utilities.h>
#include "CsvReader.h"
#include <boost/algorithm/string.hpp>

using namespace vaultdb;

std::unique_ptr<QueryTable> CsvReader::readCsv(const string &filename, const QuerySchema &schema) {

    std::vector<std::string> tupleEntries = readFile(filename);

    std::unique_ptr<QueryTable> result(new QueryTable(tupleEntries.size(), schema.getFieldCount()));
    result->setSchema(schema);
    int cursor = 0;

    for(std::string line : tupleEntries) {
        QueryTuple newTuple = parseTuple(line, schema);
        result->putTuple(cursor, newTuple);
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
        string cwd = DataUtilities::getCurrentWorkingDirectory();
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
    for(int i = 0; i < tokens.size(); ++i) {

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

QueryTuple CsvReader::parseTuple(const string &line, const QuerySchema &schema) {
    std::vector<std::string> tupleFields;
    size_t fieldCount = schema.getFieldCount();


    tupleFields = split(line);
    assert(fieldCount == tupleFields.size()); // verify the field count
    QueryTuple newTuple(fieldCount);

    // TODO: split this into a parseLine method that takes in the string and schema and returns a tuple.  Use this to debug line 16
    for(int i = 0; i < fieldCount; ++i) {
        types::Value fieldValue = TypeUtilities::decodeStringValue(tupleFields[i], schema.getField(i));
        QueryField queryField(i, fieldValue);
        newTuple.putField(queryField);
    }

    return newTuple;
}


