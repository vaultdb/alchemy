//
// Created by madhav on 2/17/20.
//

#include "secure_aggregate.h"
#include <common/macros.h>
#include <querytable/query_table.h>
#include "aggregate_id.h"

std::unique_ptr<QueryTable> Aggregate(QueryTable *input,
                                      const AggregateDef &def) {
  // TODO(madhavsuresh): for tpc-h 1 need to implement SUM, AVG, and COUNT
  // need to be able to set the output schema with the "as" clause
  // that will have to be in the AggregateDef
  // pseudocode:

  std::unique_ptr<QueryTable> aggregate_output =
      std::make_unique<QueryTable>(input->GetIsEncrypted(), 1);

  //emp::Integer Aggregate_Result(, ) ;
  //emp::Integer *sum = new emp::Integer(64, 0, emp::BOB);
  std::vector<emp::Integer> result_vector;

  for (int i = 0; i < def.defs.size(); i++) {
    result_vector.emplace_back(64, 0, emp::BOB);
  }

  emp::Bit *isDummy = new emp::Bit(false, emp::PUBLIC);
  //emp::Integer count_emp(64, 0, emp::BOB);

  emp::Integer one(64, 1, emp::PUBLIC);
  emp::Integer zero(64, 0, emp::PUBLIC);

  for (int row = 0; row < input->GetNumTuples(); row++) {

    *isDummy = *input->GetTuple(row)->GetDummyFlag()->GetEmpBit();

    for (int idx = 0; idx < def.defs.size(); idx++) {

      switch(def.defs[idx].id) {

      case AggregateId::COUNT:
        emp::If(*isDummy,
            AddToCount(result_vector[idx], zero),
            AddToCount(result_vector[idx], one)) ;

        break;

      case AggregateId::SUM:
        emp::If(*isDummy,
            AddToSum(result_vector[idx], zero),
            AddToSum(result_vector[idx],
                *input->GetTuple(row)
                ->GetField(def.defs[idx].ordinal)
                ->GetValue()
                ->GetEmpInt())) ;
        break;

      case AggregateId ::AVG:
        result_vector[idx];
        break;
      }
//      result_vector[idx] =
//          result_vector[idx] + emp::If(*isDummy, zero,
//                                       *input->GetTuple(row)
//                                           ->GetField(def.defs[idx].ordinal)
//                                           ->GetValue()
//                                           ->GetEmpInt());
    }

//    count_emp = count_emp + emp::If(*isDummy, zero, one);
//    for (int idx = 0; idx < def.defs.size(); idx++) {
//      result_vector[idx] =
//          result_vector[idx] + emp::If(*isDummy, zero,
//                                    *input->GetTuple(row)
//                                         ->GetField(def.defs[idx].ordinal)
//                                         ->GetValue()
//                                         ->GetEmpInt());
//    }
  }
//  std::vector<emp::Integer> average_vector;
//  for (int i = 0; i < def.defs.size(); i++) {
//    average_vector.emplace_back(64, 0, emp::BOB);
//    average_vector[i] = (result_vector[i]) / count_emp;
//    const QueryField f(average_vector[i], average_vector[i].length, i);
//    aggregate_output->GetTuple(0)->PutField(i, &f);
//  }

  for (int i = 0; i < def.defs.size(); i++) {
    //average_vector.emplace_back(64, 0, emp::BOB);
    //average_vector[i] = (result_vector[i]) / count_emp;
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
