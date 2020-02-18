//
// Created by madhav on 12/27/19.
//

#include "proto_converter.h"
#include "querytable/query_field_desc.h"
#include "querytable/query_schema.h"
#include "querytable/query_table.h"

vaultdb::types::TypeId ProtoToTypeId(dbquery::OIDType oidtype) {
  switch (oidtype) {
  case dbquery::INTEGER:
  case dbquery::BIGINT:
    return vaultdb::types::TypeId::INTEGER64;
    // return vaultdb::types::TypeId::INTEGER32;
  case dbquery::VARCHAR:
    return vaultdb::types::TypeId::VARCHAR;
  case dbquery::NUMERIC:
  case dbquery::DOUBLE:
    return vaultdb::types::TypeId::DOUBLE;
  case dbquery::TIMESTAMP:
  case dbquery::TIME:
  case dbquery::UNSUPPORTED:
  default:
    throw;
  }
}

std::unique_ptr<QuerySchema>
ProtoToQuerySchema(const dbquery::Schema &proto_schema) {
  auto s = std::make_unique<QuerySchema>(proto_schema.numcolumns());
  for (int i = 0; i < proto_schema.numcolumns(); i++) {
    auto col_info = proto_schema.column().at(i);
    col_info.type();
    QueryFieldDesc fd(i, col_info.is_private(), col_info.name(),
                      col_info.tablename(), ProtoToTypeId(col_info.type()));
    s->PutField(i, fd);
  }
  return s;
}

std::unique_ptr<QueryTable> ProtoToQuerytable(const dbquery::Table &t) {
  auto query_table = std::make_unique<QueryTable>(t.row_size());
  query_table->SetSchema(ProtoToQuerySchema(t.schema()));
  int index = 0;
  for (auto &r : t.row()) {
    QueryTuple *tup = query_table->GetTuple(index);
    for (auto &c : r.column()) {
      // FieldType type = ProtoToFieldtype(c.second.type());
      vaultdb::types::TypeId type = ProtoToTypeId(c.second.type());
      std::unique_ptr<vaultdb::QueryField> qf;
      switch (type) {
      case vaultdb::types::TypeId::INTEGER32:
        qf = std::make_unique<vaultdb::QueryField>(c.second.int32field(),
                                                   c.first);
        break;
      case vaultdb::types::TypeId::INTEGER64:
        qf = std::make_unique<vaultdb::QueryField>(c.second.int64field(),
                                                   c.first);
        break;
      case vaultdb::types::TypeId::VARCHAR:
        throw;
        qf =
            std::make_unique<vaultdb::QueryField>(c.second.strfield(), c.first);
        break;
      case vaultdb::types::TypeId::DOUBLE:
        throw;
        qf = std::make_unique<vaultdb::QueryField>(c.second.doublefield(),
                                                   c.first);
        break;
      default:
        throw;
      }
      tup->PutField(c.first, std::move(qf));
    }
    index++;
  }
  return query_table;
}
