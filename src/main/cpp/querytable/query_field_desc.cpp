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
