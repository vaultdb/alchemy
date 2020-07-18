//
// Created by madhav on 2/17/20.
//

#ifndef _SECURE_AGGREGATE_H
#define _SECURE_AGGREGATE_H

#include <querytable/query_table.h>
#include "aggregate_id.h"


struct ScalarAggregateDef {
  int ordinal;
  AggregateId id;
  string alias; // TODO: integrate this with QueryTable

  ScalarAggregateDef(int anOrdinal, AggregateId aggregateId, string anAlias)  : ordinal(anOrdinal), id(aggregateId), alias(anAlias)
    {}
};

struct AggregateDef {
  std::vector<ScalarAggregateDef> scalarAggregates;
  std::vector<int> groupByOrdinals;
};
std::unique_ptr<QueryTable> Aggregate(QueryTable *input,
                                      const AggregateDef &def);
void AddToCount(emp::Integer &count_so_far, emp::Integer add);
emp::Integer GetAggregateById(emp::Integer result_vector,
                              AggregateId agg_type, emp::Bit dummy);
#endif // _SECURE_AGGREGATE_H

