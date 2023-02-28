#include <util/utilities.h>
#include "csv_reader.h"
#include <boost/algorithm/string.hpp>
#include <query_table/field/field.h>
#include <query_table/field/field_factory.h>
#include <query_table/query_table.h>
#include <query_table/plain_tuple.h>

#include <util/data_utilities.h>

using namespace vaultdb;

std::unique_ptr<PlainTable> CsvReader::readCsv(const string &filename, const QuerySchema &schema) {

    std::vector<std::string> tuple_entries = DataUtilities::readTextFile(filename);
    return readCsvFromBatch(tuple_entries, schema);

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
    }

}

std::unique_ptr<PlainTable> CsvReader::readCsvFromBatch(const vector<string> &input, const QuerySchema &schema) {
    QuerySchema dst_schema = convertDatesToLong(schema);
   
    std::unique_ptr<PlainTable> result(new PlainTable(input.size(), dst_schema));
    int cursor = 0;

    for(std::string line : input) {
        parseTuple(line, schema, result, cursor);
        ++cursor;
    }

    return result;

}

QuerySchema CsvReader::convertDatesToLong(const QuerySchema &input_schema) {
    QuerySchema dst_schema(input_schema);

    // replace dates with LONG
    for(size_t i = 0; i < input_schema.getFieldCount(); ++i) {
        if(input_schema.getField(i).getType() == FieldType::DATE) {
            QueryFieldDesc dst_field_desc(input_schema.getField(i), FieldType::LONG);
            dst_schema.putField(dst_field_desc);
        }
    }
    
    dst_schema.initializeFieldOffsets();
    return dst_schema;
}


