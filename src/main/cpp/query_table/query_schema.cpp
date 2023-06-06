#include <iostream>
#include "query_schema.h"

using namespace vaultdb;


// relies on initializeFieldOffsets()
size_t QuerySchema::size() const {
    return tuple_size_;
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

    fields_.clear();
    offsets_.clear();

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
    // empty query table
    if(fields_.empty()) return;

    size_t col_count = fields_.size();
    if(fields_.find(-1) != fields_.end()) --col_count;
    for(int i = 0; i < col_count; ++i) {
        QueryFieldDesc fd = fields_.at(i);
        offsets_[i] = running_offset;
        running_offset += fd.size();

    }
    // dummy tag at end
    offsets_[-1] = running_offset;

     QueryFieldDesc dummy_tag(-1, fields_[0].getTableName(), "dummy_tag",
                   isSecure() ?
                   FieldType::SECURE_BOOL :
                   FieldType::BOOL, 0);
    fields_[-1] = dummy_tag;

    int dummy_size = isSecure() ?   TypeUtilities::getTypeSize(FieldType::SECURE_BOOL) :  TypeUtilities::getTypeSize(FieldType::BOOL);

    tuple_size_ = running_offset +  dummy_size;

}




