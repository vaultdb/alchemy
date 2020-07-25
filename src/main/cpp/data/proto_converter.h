//
// Created by madhav on 12/28/19.
//

#ifndef _PROTO_CONVERTER_H
#define _PROTO_CONVERTER_H

#include "rpc/dbquery.pb.h"
#include <querytable/query_table.h>

std::unique_ptr<QueryTable> ProtoToQueryTable(const dbquery::Table &t);
const dbquery::Table
QueryTableToXorProto(const QueryTable* input_table);
#endif // _PROTO_CONVERTER_H
