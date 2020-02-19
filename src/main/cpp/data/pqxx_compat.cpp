//
// Created by madhav on 12/27/19.
//
#include "pqxx_compat.h"

using namespace dbquery;
using namespace std;

// OID constants taken from postgres/catalog/pg_type.h, not included in
// Ubuntu 16.04 postgres package. These are global constants set in postgres
#define BPCHAROID 1042
#define VARCHAROID 1043
#define INT8OID 20
#define INT4OID 23
#define DATEOID 1082
#define TIMESTAMPOID 1114
#define FLOAT4OID 700
#define FLOAT8OID 701
#define NUMERIC 1700

Row row_from_pq(pqxx::row row, Schema s) {}

OIDType get_OID_field_type(pqxx::oid oid) {
  switch (oid) {
  case BPCHAROID:
  case VARCHAROID:
    return VARCHAR;
  case INT4OID:
    return INTEGER;
  case INT8OID:
    return BIGINT;
    // TODO(madhavsuresh): numeric was moved here because of TPC-H
  case NUMERIC:
  case FLOAT4OID:
  case FLOAT8OID:
    return DOUBLE;
  case TIMESTAMPOID:
  case DATEOID:
    return TIMESTAMP;
  default: {
    throw std::invalid_argument("Unsupported column type");
  }
  }
}

const Schema GetSchemaFromQuery(pqxx::result res) {
  Schema schema;
  schema.set_numcolumns(res.columns());
  for (int i = 0; i < schema.numcolumns(); i++) {
    ColumnInfo columnInfo;
    columnInfo.set_name(res.column_name(i));
    columnInfo.set_type(get_OID_field_type(res.column_type(i)));
    (*schema.mutable_column())[i] = columnInfo;
  }
  return schema;
}

Row get_row(pqxx::row pqrow, Schema &schema) {
  int counter = 0;
  Row row;
  for (auto field : pqrow) {
    ColumnVal val;
    if (field.is_null()) {
      // TODO(madhavsuresh): properly handle these exceptions
      throw;
    }
    val.set_type(schema.column().at(counter).type());
    switch (schema.column().at(counter).type()) {
    case dbquery::INTEGER:
      val.set_int32field(field.as<int32_t>());
    case dbquery::BIGINT:
      val.set_int64field(field.as<int64_t>());
      break;
    case dbquery::VARCHAR:
      val.set_strfield(field.c_str());
      break;
    case dbquery::TIMESTAMP:
      throw;
    case dbquery::DOUBLE:
      val.set_doublefield(field.as<double>());
      break;
    default:
      throw;
    }
    (*row.mutable_column())[counter] = val;
    counter++;
  }
  return row;
}

Table get_table_from_query(pqxx::result &res, Schema &schema) {
  Table t;
  int counter = 0;
  for (auto psql_row : res) {
    Row row = get_row(psql_row, schema);
    t.add_row()->CopyFrom(row);
  }
  return t;
}

Table GetPqTable(string dbname, string query_string) {
  auto res = query(dbname, query_string);
  Schema schema = GetSchemaFromQuery(res);
  Table table = get_table_from_query(res, schema);
  table.mutable_schema()->CopyFrom(schema);
  return table;
}

pqxx::result query(string dbname, string query_string) {
  pqxx::result res;
  try {
    pqxx::connection c(dbname);
    pqxx::work txn(c);

    res = txn.exec(query_string);
    txn.commit();
    // TODO(madhavsuresh): make this exception handling more robust
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    throw e;
  }
  return res;
}
