//
// Created by madhav on 2/17/20.
//

#include "secure_aggregate.h"
#include <querytable/query_table.h>

std::unique_ptr<QueryTable> Aggregate(QueryTable *input,
                                      const AggregateDef &def) {
  //TODO(madhavsuresh): for tpc-h 1 need to implement SUM, AVG, and COUNT
  // need to be able to set the output schema with the "as" clause
  // that will have to be in the AggregateDef
  //pseudocode:


  // 1. iterate over the table (using getTuples(), getRowNum())
  // 2. aggregator (sum) = 0
  //     2.5 initiate and increment a count(*)
  // 3. go over each field and sum it up to final
  // 4. print out the sum (for now)

  emp::Integer* sum = new emp::Integer(64,0, emp::BOB);
  int count_star = input->GetNumTuples();
  std::cout<<"Count(*) = "<<count_star;
  for(int row = 0; row < input->GetNumTuples(); row++) {

    *sum = *sum + *input->GetTuple(row)->GetField(def.index)->GetValue()->GetEmpInt();
    //std::cout<<sum;
    //std::cout<<"Sum = "<< sum;
    //input->GetTuple(row)->GetField(def.index)->GetValue()->GetType();

  }
  int table_sum = sum->reveal<int64_t>(emp::PUBLIC);
  std::cout << "\nOutput of sum " << table_sum;
  std::cout << "\nAverage of the table: "<< emp::Float(table_sum/count_star);

  return nullptr;
}


