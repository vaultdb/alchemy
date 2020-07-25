//
// Created by madhav on 1/13/20.
//

#include "query_field_desc.h"
#include "util/type_utilities.h"


int QueryFieldDesc::GetColumnNumber() const {
  return QueryFieldDesc::column_number_;
}

bool QueryFieldDesc::GetIsPrivate() const {
  return QueryFieldDesc::is_private_;
}

const std::string &QueryFieldDesc::GetName() const {
  return QueryFieldDesc::name_;
}

vaultdb::types::TypeId QueryFieldDesc::GetType() const {
    return QueryFieldDesc::type_;
}


const std::string &QueryFieldDesc::GetTableName() const {
  return QueryFieldDesc::table_name;
}

size_t QueryFieldDesc::size() const {
    vaultdb::types::TypeId typeId  = GetType();
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
    os << "#" << desc.column_number_ << " " << TypeUtilities::getTypeIdString(desc.type_);
    if(desc.type_ == vaultdb::types::TypeId::VARCHAR) {
        os << "(" << desc.string_length << ")";
    }

    os << " " << desc.table_name << "." << desc.name_;
    return os;
}

// essentially a copy constructor, designed to sidestep the need to deal with pointer semantics in QuerySchema
void QueryFieldDesc::initialize(QueryFieldDesc src) {

    this->type_ = src.type_;
    this->name_ = src.name_;
    this->column_number_ = src.column_number_;
    this->table_name = src.table_name;
    this->is_private_ = src.GetIsPrivate();
    this->string_length = src.string_length;


}

// default constructor setting up unique_ptr
QueryFieldDesc::QueryFieldDesc() {

}
