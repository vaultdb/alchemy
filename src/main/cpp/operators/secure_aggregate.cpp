//
// Created by madhav on 2/17/20.
//

#include "secure_aggregate.h"
#include "aggregate_id.h"
#include <common/macros.h>
#include <querytable/query_table.h>

#define AGG_COUNT(is_dummy)                                                    \
  do {                                                                         \
    not_dummy = emp::If(is_dummy, zero, one);                                  \
  } while (0)

#define AGG_SUM(is_dummy, row, idx)                                            \
  do {                                                                         \
    not_dummy = emp::If(is_dummy, zero,                                        \
                        *input->GetTuple(cursor)                               \
                             ->GetField(def.scalarAggregates[idx].ordinal)                 \
                             ->GetValue()                                      \
                             ->getEmpInt());                                   \
  } while (0)

#define SCALAR_COUNT(is_dummy, result_ref)                                        \
  do {                                                                         \
    emp::Integer res = emp::If(*is_dummy, zero, one);                          \
    AddToCount(result_ref, res);                                               \
  } while (0)

#define SCALAR_SUM(is_dummy, result_ref, row, idx)                                \
  do {                                                                         \
    emp::Integer res = emp::If(*isDummy, zero, *input->GetTuple(row)           \
    ->GetField(def.scalarAggregates[idx].ordinal) ->GetValue() ->getEmpInt());             \
    AddToCount(result_ref, res);                                               \
  } while (0)

std::unique_ptr<QueryTable> Aggregate(QueryTable *input,
                                      const AggregateDef &def) {

  std::unique_ptr<QueryTable> aggregate_output;
  if (def.groupByOrdinals.size() == 0) {
    aggregate_output =
        std::make_unique<QueryTable>(input->GetIsEncrypted(), 1);
  }
  else {
    aggregate_output = std::make_unique<QueryTable>(
        input->GetIsEncrypted(), input->GetNumTuples());
  }

  emp::Integer one(64, 1, emp::PUBLIC);
  emp::Integer zero(64, 0, emp::PUBLIC);
  emp::Bit isDummy(false, emp::PUBLIC);

  emp::Integer not_dummy(64, 0, emp::PUBLIC);
  emp::Bit true_dummy(true, emp::PUBLIC);
  emp::Bit false_dummy(false, emp::PUBLIC);
  emp::Bit prev_dummy(false, emp::PUBLIC);

  // result vector for each tuple in the relation
  std::vector<emp::Integer> result_vector;

  for (int i = 0; i < def.scalarAggregates.size(); i++) {
    result_vector.emplace_back(64, 0, emp::BOB);
  }

  // shadow vector to keep track of the running average
  std::map<int, std::pair<emp::Integer, emp::Integer>> running_avg;

  // copy constructors for initializing pair<emp:Int,emp:Int>
  emp::Integer i1(64, 0, emp::BOB);
  emp::Integer i2(64, 0, emp::BOB);

  std::vector<emp::Integer> groupby_vector;
  for (int i = 0; i < def.groupByOrdinals.size(); i++) {
    groupby_vector.emplace_back(64, 0, emp::BOB);
  }

  // same shadow vector can be used for each bin
  for (int idx = 0; idx < def.scalarAggregates.size(); idx++) {
    if (def.scalarAggregates[idx].id == AggregateId ::AVG) {
      running_avg[idx] = std::make_pair(i1, i2);
    }
  }

  // check if the query is scalar (no groupby clause)
  if (def.groupByOrdinals.size() == 0) {

    emp::Bit *isDummy = new emp::Bit(false, emp::PUBLIC);

    for (int row = 0; row < input->GetNumTuples(); row++) {

      // dummy flag to determine if the tuplle is a Dummy value
      *isDummy = *input->GetTuple(row)->GetDummyTag()->getEmpBit();

      for (int idx = 0; idx < def.scalarAggregates.size(); idx++) {

        // switch on the Aggregate ID
        switch (def.scalarAggregates[idx].id) {

        case AggregateId::COUNT: {
          SCALAR_COUNT(isDummy, result_vector[idx]);
          break;
        }
        case AggregateId::SUM: {

          SCALAR_SUM(isDummy, result_vector[idx], row, idx);
          break;
        }
        case AggregateId::AVG: {
          SCALAR_SUM(isDummy, running_avg[idx].first, row, idx);
          SCALAR_COUNT(isDummy, running_avg[idx].second);
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
      const QueryField f(i, result_vector[i], result_vector[i].length);
      aggregate_output->GetTuple(0)->PutField(i, &f);
    }

    vaultdb::types::Value curr_dval(
            true_dummy);
      aggregate_output->GetTuple(0)->SetDummyTag(&curr_dval);
  }

    // otherwise, if the GROUPBY clause is present
  else {

    emp::Integer prev_group_by(64, 0, emp::PUBLIC);
    emp::Integer group_by(64, 0, emp::PUBLIC);

    for (int cursor = 0; cursor < input->GetNumTuples(); cursor++) {

      emp::Bit groupby_eq(true, emp::PUBLIC);
      // assuming that every tuple belongs to the previous tuple's bin
      // this condition holds true even for the very first tuple
      // Since the default value is true, result vector would remain unaffected

      isDummy = *input->GetTuple(cursor)->GetDummyTag()->getEmpBit();
      if (cursor != 0)
        prev_dummy = *aggregate_output->GetTuple(cursor - 1)
                ->GetDummyTag()
                ->getEmpBit();

      // groupby_eq (equality): bool=> checks for equality with prev. tuples GB
      for (int idx = 0; idx < def.groupByOrdinals.size(); idx++) {
        int ord = def.groupByOrdinals[idx];
        group_by =
            *input->GetTuple(cursor)->GetField(ord)->GetValue()->getEmpInt();
        groupby_eq =
            If((groupby_vector[idx] == group_by), groupby_eq, false_dummy);
        groupby_vector[idx] = group_by;
      }

      for (int idx = 0; idx < def.scalarAggregates.size(); idx++) {

        switch (def.scalarAggregates[idx].id) {
        case AggregateId::COUNT: {
          AGG_COUNT(isDummy);
          result_vector[idx] =
              not_dummy + If(groupby_eq, result_vector[idx], zero);
          break;
        }
        case AggregateId::SUM: {
            AGG_SUM(isDummy, cursor, idx);
          result_vector[idx] =
              not_dummy + If(groupby_eq, result_vector[idx], zero);
          break;
        }
        case AggregateId::AVG: {
          if (cursor != 0) {
            running_avg[idx].first =
                If(groupby_eq, running_avg[idx].first, zero);
            running_avg[idx].second =
                If(groupby_eq, running_avg[idx].second, zero);
          }
            AGG_SUM(isDummy, cursor, idx);
          running_avg[idx].first = running_avg[idx].first + not_dummy;
          AGG_COUNT(isDummy);
          running_avg[idx].second = running_avg[idx].second + not_dummy;
          result_vector[idx] = running_avg[idx].first / running_avg[idx].second;
          break;
        }
        }
      }
      // updating current dummy (nested If)
      if (cursor != 0) {
        isDummy = If(groupby_eq,
                     If(*aggregate_output->GetTuple(cursor - 1)
                                ->GetDummyTag()
                                ->getEmpBit(),
                        true_dummy, false_dummy),
                     isDummy);

        // updating previous tuple's dummy (if necessary), and setting that value
        prev_dummy = If(groupby_eq,
                        If(*aggregate_output->GetTuple(cursor - 1)
                                   ->GetDummyTag()
                                   ->getEmpBit(),
                           prev_dummy, true_dummy),
                        prev_dummy);

        vaultdb::types::Value prev_dval(prev_dummy);
          aggregate_output->GetTuple(cursor - 1)->SetDummyTag(&prev_dval);
      }

      for (int i = 0; i < def.scalarAggregates.size(); i++) {
        const QueryField f(i, result_vector[i], result_vector[i].length);
        aggregate_output->GetTuple(cursor)->PutField(i, &f);
      }
      vaultdb::types::Value curr_dval(
              isDummy);
        aggregate_output->GetTuple(cursor)->SetDummyTag(&curr_dval);
    }
  }
  return aggregate_output;
}


// in-place updation of count (helper function)
void AddToCount(emp::Integer &count_so_far, emp::Integer add) {
  count_so_far = count_so_far + add;
  return;
}
