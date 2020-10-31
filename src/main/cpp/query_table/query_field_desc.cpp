#include "query_field_desc.h"
#include "util/type_utilities.h"

// default constructor setting up unique_ptr
QueryFieldDesc::QueryFieldDesc() {

}

int QueryFieldDesc::getOrdinal() const {
  return QueryFieldDesc::ordinal_;
}


const std::string &QueryFieldDesc::getName() const {
  return QueryFieldDesc::name_;
}

vaultdb::types::TypeId QueryFieldDesc::getType() const {
    return type_;
}


const std::string &QueryFieldDesc::getTableName() const {
  return QueryFieldDesc::table_name;
}

size_t QueryFieldDesc::size() const {
    vaultdb::types::TypeId typeId  = getType();
    size_t fieldSize = TypeUtilities::getTypeSize(typeId);
    if(QueryFieldDesc::type_ == vaultdb::types::TypeId::VARCHAR)  {
        fieldSize *= string_length_;
    }

    return fieldSize;

}

void QueryFieldDesc::setStringLength(size_t len) {

    string_length_ = len;

}

std::ostream &operator<<(std::ostream &os, const QueryFieldDesc &desc) {
    os << "#" << desc.ordinal_ << " " << TypeUtilities::getTypeIdString(desc.type_);
    if(desc.type_ == vaultdb::types::TypeId::VARCHAR) {
        os << "(" << desc.string_length_ << ")";
    }

    os << " " << desc.table_name << "." << desc.name_;
    return os;
}

QueryFieldDesc& QueryFieldDesc::operator=(const QueryFieldDesc& src)  {


    this->type_ = src.type_;
    this->name_ = src.name_;
    this->ordinal_ = src.ordinal_;
    this->table_name = src.table_name;
    this->string_length_ = src.string_length_;

    return *this;
}

// only checking for relation compatibility, so don't care about table name or field name
bool QueryFieldDesc::operator==(const QueryFieldDesc& other) {

    // if types are the same, or int32_t --> date
    if (!(this->getType() == other.getType() ||
          (this->getType() == types::TypeId::INTEGER32 && other.getType() == types::TypeId::DATE) ||
          (other.getType() == types::TypeId::INTEGER32 && this->getType() == types::TypeId::DATE))) {
        return false;
    }





    if(other.getOrdinal() != other.getOrdinal()) {

        return false;
    }

    if(other.string_length_ != this->string_length_) {

        return false;
    }

    return true;


}


