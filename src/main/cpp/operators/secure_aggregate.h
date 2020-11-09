#ifndef _SECURE_AGGREGATE_H
#define _SECURE_AGGREGATE_H

#include "operators/support/aggregate_id.h"
#include <operator.h>
#include <query_table/query_table.h>



struct AggregateDef {
  std::vector<ScalarAggregateDefinition> scalarAggregates;
  std::vector<int> groupByOrdinals;
};
std::unique_ptr<QueryTable> Aggregate(QueryTable *input,
                                      const AggregateDef &def);

void AddToCount(types::Value count_so_far, types::Value add);
// emp::Integer GetAggregateById(emp::Integer result_vector,
//                              AggregateId agg_type, emp::Bit dummy);
#endif // _SECURE_AGGREGATE_H
