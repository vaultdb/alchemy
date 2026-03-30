#ifndef _PROTO_CONVERTER_H
#define _PROTO_CONVERTER_H

#include "rpc/dbquery.pb.h"
#include "query_table/query_table.h"
#include "query_table/query_field_desc.h"
#include "query_table/query_schema.h"

std::unique_ptr<QueryTable> ProtoToQueryTable(const dbquery::Table &t);
const dbquery::Table
QueryTableToXorProto(const QueryTable* input_table);
#endif // _PROTO_CONVERTER_H
