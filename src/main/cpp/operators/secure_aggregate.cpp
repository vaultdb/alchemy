//
// Created by madhav on 2/17/20.
//

#include "secure_aggregate.h"
#include <common/macros.h>
#include <querytable/query_table.h>
std::unique_ptr<QueryTable> Aggregate(QueryTable *input,
                                      const AggregateDef &def) {
  // TODO(madhavsuresh): for tpc-h 1 need to implement SUM, AVG, and COUNT
  // need to be able to set the output schema with the "as" clause
  // that will have to be in the AggregateDef
  // pseudocode:
  std::unique_ptr<QueryTable> aggregate_output =
      std::make_unique<QueryTable>(input->GetIsEncrypted(), 1);
  emp::Integer *sum = new emp::Integer(64, 0, emp::BOB);
  std::vector<emp::Integer> sum_vector;
  for (int i = 0; i < def.index.size(); i++) {
    sum_vector.emplace_back(64, 0, emp::BOB);
  }
  emp::Bit *isDummy = new emp::Bit(false, emp::PUBLIC);

  emp::Integer count_emp(64, 0, emp::BOB);

  emp::Integer one(64, 1, emp::PUBLIC);
  emp::Integer zero(64, 1, emp::PUBLIC);
  for (int row = 0; row < input->GetNumTuples(); row++) {
    *isDummy = *input->GetTuple(row)->GetDummyFlag()->GetEmpBit();
    count_emp = count_emp + emp::If(*isDummy, zero, one);
    for (int idx = 0; idx < def.index.size(); idx++) {
      sum_vector[idx] =
          sum_vector[idx] + emp::If(*isDummy, zero,
                                    *input->GetTuple(row)
                                         ->GetField(def.index[idx])
                                         ->GetValue()
                                         ->GetEmpInt());
    }
  }
  std::vector<emp::Integer> average_vector;
  for (int i = 0; i < def.index.size(); i++) {
    average_vector.emplace_back(64, 0, emp::BOB);
    average_vector[i] = (sum_vector[i]) / count_emp;
    const QueryField f(average_vector[i], average_vector[i].length, i);
    aggregate_output->GetTuple(0)->PutField(i, &f);
  }

  // TODO: implement this for float as well (returning int(AVG) for now
  return aggregate_output;
}
