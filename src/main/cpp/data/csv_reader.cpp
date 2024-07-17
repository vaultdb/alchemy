#include "csv_reader.h"
#include <boost/algorithm/string.hpp>
#include <query_table/field/field.h>
#include <query_table/field/field_factory.h>
#include <util/data_utilities.h>

using namespace vaultdb;

PlainTable *CsvReader::readCsv(const string &filename, const QuerySchema &schema) {

    std::vector<std::string> tuple_entries = DataUtilities::readTextFile(filename);
    return readCsvFromBatch(tuple_entries, schema);

}



vector<string> CsvReader::split(const string &tuple_entry) {
    std::vector<std::string> tokens, result;
    std::string latest_entry;
    boost::split(tokens, tuple_entry,boost::is_any_of(","));

    // make a pass and merge any strings
    for(size_t i = 0; i < tokens.size(); ++i) {

        std::string token = tokens[i];
        if(token[0] == '"' && latest_entry.empty()) {  // starts with " and the beginning of an entry
            latest_entry =  token;

        }
        else if(latest_entry.empty()) {
            result.push_back(token);
        }
        else  {
            latest_entry += "," + token;
            if(*(--token.end()) == '"') { // if it is the end of the string
                result.push_back(latest_entry.substr(1, latest_entry.length() - 2)); // chop off leading and trailing double-quotes
                latest_entry.clear();
            }
        }
    } // end for

    return result;
}

void CsvReader::parseTuple(const std::string &csv_line, const QuerySchema &src_schema, PlainTable *dst,
                           const int &idx) {

    std::vector<std::string> tuple_fields;
    QuerySchema schema = dst->getSchema();
    size_t field_cnt = schema.getFieldCount();


    tuple_fields = split(csv_line);
    assert(field_cnt == tuple_fields.size()); // verify the field count

    for(size_t i = 0; i < field_cnt; ++i) {
        PlainField field = FieldFactory<bool>::getFieldFromString(src_schema.getField(i).getType(), schema.getField(i).getStringLength(), tuple_fields[i]);
        dst->setField(idx, i, field);
    }
    // implicitly not dummy
    dst->setDummyTag(idx, false);

}

PlainTable *CsvReader::readCsvFromBatch(const vector<string> &input, const QuerySchema &schema) {
    QuerySchema dst_schema = convertDatesToLong(schema);
   
    PlainTable *result = PlainTable::getTable(input.size(), dst_schema);
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
    for(int i = 0; i < input_schema.getFieldCount(); ++i) {
        if(input_schema.getField(i).getType() == FieldType::DATE) {
            QueryFieldDesc dst_field_desc(input_schema.getField(i), FieldType::LONG);
            dst_schema.putField(dst_field_desc);
        }
    }
    
    dst_schema.initializeFieldOffsets();
    return dst_schema;
}


