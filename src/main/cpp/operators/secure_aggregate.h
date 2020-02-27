//
// Created by madhav on 2/17/20.
//

#ifndef TESTING_SECURE_AGGREGATE_H
#define TESTING_SECURE_AGGREGATE_H

#include <querytable/query_table.h>
struct AggregateDef {
  std::vector<int> index {0, 1, 2};
  //int ordinals[5];

};
std::unique_ptr<QueryTable> Aggregate(QueryTable *input,
                                      const AggregateDef &def);
#endif // TESTING_SECURE_AGGREGATE_H

