//
// Created by madhav on 1/31/20.
//

#ifndef _PRIVATE_SHARE_UTILITY_H
#define _PRIVATE_SHARE_UTILITY_H

#include "query_schema.h"
#include "query_table.h"
#include <common/defs.h>

using namespace vaultdb;

class QueryTable;

struct ShareCount {
  EmpParty party;
  int num_tuples;
};

struct ShareDef {
  std::map<EmpParty, ShareCount> share_map;
};
std::unique_ptr<QueryTable> ShareData(const QuerySchema *shared_schema,
                                       EmpParty party,
                                       QueryTable *input_table,
                                       ShareDef &def);

#endif // _PRIVATE_SHARE_UTILITY_H
