//
// Created by madhav on 2/17/20.
//

#include "secure_aggregate.h"
#include "aggregate_id.h"
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

  std::vector<emp::Integer> result_vector;

  for (int i = 0; i < def.defs.size(); i++) {
    result_vector.emplace_back(64, 0, emp::BOB);
  }

  // checking the number of columns having the AVG() clause
  int avg_count = 0;
  for (int idx = 0; idx < def.defs.size(); idx++) {
    if (def.defs[idx].id == AggregateId ::AVG) {
      avg_count++;
    }
  }

  // using this shadow vector to compute the running average
  std::vector<std::pair<emp::Integer, emp::Integer>> running_avg;

  // NOTE: emplace_back or push_back isn't working for pairs
  // I beleive this is where the problem is..
  for (int i = 0; i < avg_count; i++) {
    //std::pair pair = std::make_pair((64, 0, emp::BOB),(64, 0, emp::BOB));
    running_avg.emplace_back(std::make_pair((64, 0, emp::BOB), (64, 0, emp::BOB)));
  }

  emp::Bit *isDummy = new emp::Bit(false, emp::PUBLIC);
  // emp::Integer count_emp(64, 0, emp::BOB);

  emp::Integer one(64, 1, emp::PUBLIC);
  emp::Integer zero(64, 0, emp::PUBLIC);

  for (int row = 0; row < input->GetNumTuples(); row++) {

    *isDummy = *input->GetTuple(row)->GetDummyFlag()->GetEmpBit();

    for (int idx = 0; idx < def.defs.size(); idx++) {

      int n = avg_count;    // keeps count of the AVG ordinals

      switch (def.defs[idx].id) {

      case AggregateId::COUNT: {
        emp::Integer res = emp::If(*isDummy, zero, one);
        AddToCount(result_vector[idx], res);
        break;
      }

      case AggregateId::SUM: {
        emp::Integer res = emp::If(*isDummy, zero,
                                   *input->GetTuple(row)
                                        ->GetField(def.defs[idx].ordinal)
                                        ->GetValue()
                                        ->GetEmpInt());
        AddToSum(result_vector[idx], res);
        break;
      }

      case AggregateId ::AVG:
        emp::Integer sum = emp::If(*isDummy, zero,
                                   *input->GetTuple(row)
                                       ->GetField(def.defs[idx].ordinal)
                                       ->GetValue()
                                       ->GetEmpInt());
        emp::Integer cnt = emp::If(*isDummy, zero, one);

        // not sure if its the right way to iterate through a vector of pairs
        if (n > 0) {
          running_avg[n].first = running_avg[n].first + sum;
          running_avg[n].second = running_avg[n].second + cnt;

          result_vector[idx] = running_avg[n].first/running_avg[n].second;
          n-- ;
        }

        break;
      }

    }

  }
  //  std::vector<emp::Integer> average_vector;
  //  for (int i = 0; i < def.defs.size(); i++) {
  //    average_vector.emplace_back(64, 0, emp::BOB);
  //    average_vector[i] = (result_vector[i]) / count_emp;
  //    const QueryField f(average_vector[i], average_vector[i].length, i);
  //    aggregate_output->GetTuple(0)->PutField(i, &f);
  //  }

  for (int i = 0; i < def.defs.size(); i++) {
    // average_vector.emplace_back(64, 0, emp::BOB);
    // average_vector[i] = (result_vector[i]) / count_emp;
    const QueryField f(result_vector[i], result_vector[i].length, i);
    aggregate_output->GetTuple(0)->PutField(i, &f);
  }
  // TODO: implement this for float as well (returning int(AVG) for now
  return aggregate_output;
}

emp::Integer AddToCount(emp::Integer &count_so_far, emp::Integer add) {
  count_so_far = count_so_far + add;
  return count_so_far;
}

emp::Integer AddToSum(emp::Integer &sum_so_far, emp::Integer add) {
  sum_so_far = sum_so_far + add;
  return sum_so_far;
}
