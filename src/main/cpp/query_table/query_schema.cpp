#include <iostream>
#include "query_schema.h"
#include "data/csv_reader.h"
#include "util/data_utilities.h"

using namespace vaultdb;



QuerySchema::QuerySchema(const std::string &schema_spec) {
    string schema_str = schema_spec;
    // chop off any trailing whitespace or endlines
    schema_str.erase(std::remove_if(schema_str.begin(), schema_str.end(), ::isspace), schema_str.end());
    // chop off parens
    if(schema_str.at(0) == '(' && schema_str.at(schema_str.length() - 1) == ')') {
        schema_str = schema_str.substr(1, schema_str.length() - 2);
    }
    // delete any spaces
    schema_str.erase(std::remove_if(schema_str.begin(), schema_str.end(), ::isspace), schema_str.end());

    // split on commas
    vector<string> tokens = CsvReader::split(schema_str);

    int counter = 0;
    int last_field = tokens.size() - 1;

    for(auto token : tokens) {
        auto f = QueryFieldDesc(counter, token);
        if(counter == last_field && f.getName() == "dummy_tag") {
            // do not add dummy tag
            assert(f.getType() == FieldType::BOOL || f.getType() == FieldType::SECURE_BOOL);
            break;
        }
        putField(f);
        ++counter;
    }
    // dummy tag's place is deterministic, will be added in initializeFieldOffsets()
    initializeFieldOffsets();
}

std::ostream &vaultdb::operator<<(std::ostream &os, const QuerySchema &schema) {
    if(schema.getFieldCount() < 1) {  os << "()"; return os;  }

    os << "(" << schema.getField(0);

    for(size_t i = 1; i < schema.getFieldCount(); ++i) {
        os << ", " << schema.getField(i);
    }

    os << ")";
    return os;
}

QuerySchema &QuerySchema::operator=(const QuerySchema &other) {

   reset();

    bool valid_fields = true;

    for (int i = 0; i < other.getFieldCount(); i++) {
        fields_[i] = other.fields_.at(i);
        if(fields_[i].getType() == FieldType::INVALID)
            valid_fields = false;
    }
    if(valid_fields)
        initializeFieldOffsets();

    return *this;
}


bool QuerySchema::operator==(const QuerySchema &other) const {


    if(this->getFieldCount() != other.getFieldCount()) {
        return false;
    }

    for(uint32_t i = 0; i < this->getFieldCount(); ++i) {
        QueryFieldDesc thisFieldDesc = this->getField(i);
        QueryFieldDesc otherFieldDesc = other.getField(i);

        if(thisFieldDesc != otherFieldDesc) {
            return false;
        }
    }

    return true;
}

QuerySchema QuerySchema::toSecure(const QuerySchema &plain_schema) {
    QuerySchema dst_schema;

    for(int i  = 0; i < plain_schema.getFieldCount(); ++i) {
        QueryFieldDesc src = plain_schema.fields_.at(i);
        QueryFieldDesc dst(src, TypeUtilities::toSecure(src.getType()));
        dst_schema.fields_[i] = dst;
    }

    dst_schema.initializeFieldOffsets();
    return dst_schema;
}

QuerySchema QuerySchema::toPlain(const QuerySchema &secure_schema) {
    QuerySchema dst_schema;

    for(int i  = 0; i < secure_schema.getFieldCount(); ++i) {
        QueryFieldDesc src = secure_schema.fields_.at(i);
        QueryFieldDesc dst(src, TypeUtilities::toPlain(src.getType()));
        dst_schema.fields_[i] = dst;
    }

    dst_schema.initializeFieldOffsets();
    return dst_schema;
}


QueryFieldDesc QuerySchema::getField(const string &field_name) const {
    for(int i = 0; i < getFieldCount(); ++i) {
        if(fields_.at(i).getName() == field_name)
            return fields_.at(i);
    }
    if(field_name == "dummy_tag") return fields_.at(-1);

    throw; // not found
}

// in bits, idx == -1 means dummy_tag
void QuerySchema::initializeFieldOffsets()  {
    // populate ordinal --> offset mapping

    size_t running_offset = 0L;
    bool is_secure = TypeUtilities::isEncrypted(fields_[0].getType());
    // empty query table
    EmpMode mode = SystemConfiguration::getInstance().emp_mode_;
    bool wire_packing = SystemConfiguration::getInstance().wire_packing_enabled_;

    // empty query table
    if(fields_.empty()) return;

    size_t col_count = fields_.size();
    if(fields_.find(-1) != fields_.end()) --col_count;

    for(int i = 0; i < col_count; ++i) {
        QueryFieldDesc fd = fields_.at(i);
        offsets_[i] = running_offset;
        running_offset +=  (is_secure && wire_packing) ? fd.packedWires() : fd.size();

    }
    // dummy tag at end
    offsets_[-1] = running_offset;

     QueryFieldDesc dummy_tag(-1, "dummy_tag", fields_[0].getTableName(),
                   isSecure() ?
                   FieldType::SECURE_BOOL :
                   FieldType::BOOL, 0);
    fields_[-1] = dummy_tag;

    int dummy_size = isSecure() ?   TypeUtilities::getTypeSize(FieldType::SECURE_BOOL) :  TypeUtilities::getTypeSize(FieldType::BOOL);

    tuple_size_ = running_offset +  dummy_size;

}


QuerySchema QuerySchema::concatenate(const QuerySchema & lhs, const QuerySchema & rhs, const bool &append_bool) {
    uint32_t output_cols = lhs.getFieldCount() + rhs.getFieldCount() + append_bool;
    QuerySchema result;
    uint32_t cursor = lhs.getFieldCount();

    for(int i = 0; i < lhs.getFieldCount(); ++i) {
        QueryFieldDesc src_field = lhs.getField(i);
        QueryFieldDesc dst_field(src_field, i);
        result.putField(dst_field);
    }


    for(int i = 0; i < rhs.getFieldCount(); ++i) {
        QueryFieldDesc src_field = rhs.getField(i);
        QueryFieldDesc dst_field(src_field, cursor);
        result.putField(dst_field);
        ++cursor;
    }

    // bool denotes whether table comes from primary key side (0) or foreign key side (1)
    if(append_bool) {
        FieldType field_type = (!result.isSecure()) ? FieldType::BOOL : FieldType::SECURE_BOOL;
        QueryFieldDesc bool_field(output_cols-1, "table_id", "", field_type, 0);
        result.putField(bool_field);
    }
    result.initializeFieldOffsets();
    return result;

}

QuerySchema QuerySchema::fromFile(const std::string &filename) {
    string schema_str = DataUtilities::readTextFileToString(filename);
    return QuerySchema(schema_str);
}

void QuerySchema::toFile(const string &filename) const {
        DataUtilities::writeFile(filename, prettyPrint());
}



