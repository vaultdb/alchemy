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
    return vaultdb::types::TypeId::FLOAT32;
  case dbquery::DOUBLE:
    return vaultdb::types::TypeId::VAULT_DOUBLE;
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
    tup->SetIsEncrypted(false);
    tup->InitDummy();
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
      case vaultdb::types::TypeId::VAULT_DOUBLE:
//        throw;
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

dbquery::OIDType GetEncryptedOIDFromVaultDBType(vaultdb::types::TypeId t) {
  switch (t) {

  case types::TypeId::ENCRYPTED_INTEGER64:
    return dbquery::OIDType::ENCRYPTED_INTEGER64;
  default:
    throw;
  }
}

const dbquery::Schema GetTableXorSchema(const QuerySchema *s) {

  dbquery::Schema ps;
  for (int i = 0; i < s->GetNumFields(); i++) {
    dbquery::ColumnInfo columnInfo;
    columnInfo.set_name(s->GetField(i)->GetName());
    columnInfo.set_type(
        GetEncryptedOIDFromVaultDBType(s->GetField(i)->GetType()));
    columnInfo.set_columnnumber(i);
    columnInfo.set_is_private(true);
    (*ps.mutable_column())[i] = columnInfo;
  }
  return ps;
}

const dbquery::Table QueryTableToXorProto(const QueryTable *input_table) {
  dbquery::Table t;
  dbquery::Schema s = GetTableXorSchema(input_table->GetSchema());
  t.mutable_schema()->CopyFrom(s);
  for (int i = 0; i < input_table->GetNumTuples(); i++) {
    dbquery::Row row;
    for (int j = 0; j < input_table->GetSchema()->GetNumFields(); j++) {
      dbquery::ColumnVal val;
      switch (input_table->GetSchema()->GetField(j)->GetType()) {

      case types::TypeId::INVALID:
      case types::TypeId::BOOLEAN:
      case types::TypeId::INTEGER32:
      case types::TypeId::INTEGER64:
      case types::TypeId::FLOAT32:
      case types::TypeId::FLOAT64:
      case types::TypeId::VAULT_DOUBLE:
      case types::TypeId::NUMERIC:
      case types::TypeId::TIMESTAMP:
      case types::TypeId::TIME:
      case types::TypeId::DATE:
      case types::TypeId::ENCRYPTED_BOOLEAN:
      case types::TypeId::VARCHAR:
      case types::TypeId::ENCRYPTED_INTEGER32:
        throw;
      case types::TypeId::ENCRYPTED_INTEGER64:
        auto s = input_table->GetTuple(i)
                     ->GetField(j)
                     ->GetValue()
                     ->GetEmpInt()
                     ->reveal<string>(emp::XOR);
        val.set_xorfield(s);
        (*row.mutable_column())[j] = val;
        break;
      }
    }
    t.add_row()->CopyFrom(row);
  }
  return t;
}
