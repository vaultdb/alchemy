//
// Created by madhav on 1/13/20.
//

#include "query_field_desc.h"
#include "util/type_utilities.h"

// default constructor setting up unique_ptr
QueryFieldDesc::QueryFieldDesc() {

}

int QueryFieldDesc::getOrdinal() const {
  return QueryFieldDesc::ordinal_;
}

bool QueryFieldDesc::getIsPrivate() const {
  return QueryFieldDesc::is_private_;
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
        fieldSize *= string_length;
    }

    return fieldSize;

}

void QueryFieldDesc::setStringLength(size_t len) {

    string_length = len;

}

std::ostream &operator<<(std::ostream &os, const QueryFieldDesc &desc) {
    os << "#" << desc.ordinal_ << " " << TypeUtilities::getTypeIdString(desc.type_);
    if(desc.type_ == vaultdb::types::TypeId::VARCHAR) {
        os << "(" << desc.string_length << ")";
    }

    os << " " << desc.table_name << "." << desc.name_;
    return os;
}

QueryFieldDesc& QueryFieldDesc::operator=(const QueryFieldDesc& src)  {


    std::cout << "Copy field desc on " << src << std::endl;
    this->type_ = src.type_;
    this->name_ = src.name_;
    this->ordinal_ = src.ordinal_;
    this->table_name = src.table_name;
    this->is_private_ = src.getIsPrivate();
    this->string_length = src.string_length;
    std::cout << "operator copied out type id: " << TypeUtilities::getTypeIdString(this->type_) << " from " << TypeUtilities::getTypeIdString(src.getType()) << std::endl;

    std::string typeStr = TypeUtilities::getTypeIdString(this->type_);
    std::cout << "searching " << typeStr << std::endl;

    assert(typeStr.find("unsupported") == std::string::npos); // string does not match unsupported

    return *this;
}


