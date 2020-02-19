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

  emp::Integer* sum = new emp::Integer(64,0, emp::BOB);
  int count_star = input->GetNumTuples();
  std::cout<<"Count(*) = "<<count_star;
  for(int row = 0; row < input->GetNumTuples(); row++) {

    *sum = *sum + *input->GetTuple(row)->GetField(def.index)->GetValue()->GetEmpInt();

  }
  const QueryField f(*sum, sum->length, 0);
  std::unique_ptr<QueryTable> aggregate_output = std::make_unique<QueryTable>
      (input->GetIsEncrypted(), 1);
  aggregate_output->GetTuple(0)->PutField(0, &f);

  //int table_sum = sum->reveal<int64_t>(emp::PUBLIC);
  //std::cout << "\nOutput of sum " << table_sum;
  // TODO: do a tuple creation instead where this average is returned
  //  i) do it for count(*)
  //  ii) for sum()
  //  iii) average()
  //  Do one better : -> make a table out of these   -> return encrypted block
  //      -> return to main()     -> print output there to verify
  //  HINT: look at secure_join.cpp
  //std::cout << "\nAverage of the table: "<< emp::Float(table_sum/count_star);

  return aggregate_output;
}

