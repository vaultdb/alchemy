//
// Created by madhav on 12/28/19.
//

#ifndef TESTING_PROTO_CONVERTER_H
#define TESTING_PROTO_CONVERTER_H

#include <dbquery.pb.h>
#include <querytable/query_table.h>

std::unique_ptr<QueryTable> ProtoToQuerytable(const dbquery::Table &t);
#endif // TESTING_PROTO_CONVERTER_H
