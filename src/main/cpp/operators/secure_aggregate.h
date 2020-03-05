//
// Created by madhav on 2/17/20.
//

#ifndef TESTING_SECURE_AGGREGATE_H
#define TESTING_SECURE_AGGREGATE_H

#include <querytable/query_table.h>
#include "aggregate_id.h"
//#include <querytable/expression/aggregate
struct ScalarAggregateDef {
  int ordinal;
  AggregateId id;
};

struct AggregateDef {
  std::vector<ScalarAggregateDef> defs;
};
std::unique_ptr<QueryTable> Aggregate(QueryTable *input,
                                      const AggregateDef &def);
void AddToCount(emp::Integer &count_so_far, emp::Integer add);
emp::Integer GetAggregateById(emp::Integer result_vector,
                              AggregateId agg_type, emp::Bit dummy);
#endif // TESTING_SECURE_AGGREGATE_H

