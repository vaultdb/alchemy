//
// Created by madhav on 2/17/20.
//

#include "secure_aggregate.h"
#include "aggregate_id.h"
#include <common/macros.h>
#include <querytable/query_table.h>

#define AGG_COUNT(is_dummy, result_ref)                                        \
  do {                                                                         \
    emp::Integer res = emp::If(*is_dummy, zero, one);                          \
    AddToCount(result_ref, res);                                               \
  } while (0)

#define AGG_SUM(is_dummy, result_ref, row, idx)                                \
  do {                                                                         \
    emp::Integer res = emp::If(*isDummy, zero, *input->GetTuple(row)           \
    ->GetField(def.scalarAggregates[idx].ordinal) ->GetValue() ->GetEmpInt());             \
    AddToCount(result_ref, res);                                               \
  } while (0)

std::unique_ptr<QueryTable> Aggregate(QueryTable *input,
                                      const AggregateDef &def) {
  // TODO(madhavsuresh): for tpc-h 1 need to implement SUM, AVG, and COUNT
  // need to be able to set the output schema with the "as" clause
  // that will have to be in the AggregateDef

  std::unique_ptr<QueryTable> aggregate_output =
      std::make_unique<QueryTable>(input->GetIsEncrypted(), 1);

  std::vector<emp::Integer> result_vector;

  for (int i = 0; i < def.scalarAggregates.size(); i++) {
    result_vector.emplace_back(64, 0, emp::BOB);
  }

  // shadow vector to keep track of the running average
  std::map<int, std::pair<emp::Integer, emp::Integer>> running_avg;

  // copy constructors for initializing pair<emp:Int,emp:Int>
  emp::Integer i1(64, 0, emp::BOB);
  emp::Integer i2(64, 0, emp::BOB);
  
  for (int idx = 0; idx < def.scalarAggregates.size(); idx++) {
    if (def.scalarAggregates[idx].id == AggregateId ::AVG) {
      running_avg[idx] = std::make_pair(i1, i2);
    }
  }

  emp::Bit *isDummy = new emp::Bit(false, emp::PUBLIC);
  emp::Integer one(64, 1, emp::PUBLIC);
  emp::Integer zero(64, 0, emp::PUBLIC);

  for (int row = 0; row < input->GetNumTuples(); row++) {

    // dummy flag to determine if the tuplle is a Dummy value
    *isDummy = *input->GetTuple(row)->GetDummyFlag()->GetEmpBit();

    for (int idx = 0; idx < def.scalarAggregates.size(); idx++) {

      // switch on the Aggregate ID
      switch (def.scalarAggregates[idx].id) {

      case AggregateId::COUNT: {
        AGG_COUNT(isDummy, result_vector[idx]);
        break;
      }

      case AggregateId::SUM: {

        AGG_SUM(isDummy, result_vector[idx], row, idx);
        break;
      }

      case AggregateId::AVG: {

        AGG_SUM(isDummy, running_avg[idx].first, row, idx);
        AGG_COUNT(isDummy, running_avg[idx].second);

        break;
      }
      }
    }
    // accumulating sum and count from the shadow vector and computing AVG()
    for (int idx = 0; idx < def.scalarAggregates.size(); idx++) {
      if (def.scalarAggregates[idx].id == AggregateId ::AVG) {
        result_vector[idx] = running_avg[idx].first / running_avg[idx].second;
      }
    }
  }

  // creates resultant relation; inserting QueryField into tuple (just 1 row)
  for (int i = 0; i < def.scalarAggregates.size(); i++) {
    const QueryField f(result_vector[i], result_vector[i].length, i);
    aggregate_output->GetTuple(0)->PutField(i, &f);
  }
  return aggregate_output;
}


// function for basic in-place addition to go along with the macros
void AddToCount(emp::Integer &count_so_far, emp::Integer add) {
  count_so_far = count_so_far + add;
  return;
}
