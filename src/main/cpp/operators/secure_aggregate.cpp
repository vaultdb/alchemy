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

  emp::Integer count_emp(64, 0, emp::BOB);

  //std::cout<<"Count(*) = "<<count_star;
  //if(count_emp){std::cout<<"There's our size for count"<<count_emp->size();}

  emp::Integer one(64, 1, emp::PUBLIC);
  for(int row = 0; row < input->GetNumTuples(); row++) {

    *sum = *sum +
        *input->GetTuple(row)
        ->GetField(def.index)
        ->GetValue()
        ->GetEmpInt();

    count_emp = count_emp+one;

  }
  //std::cout<<"\n=========== count =========== "<<count_emp->reveal<int64_t>(emp::PUBLIC);
  //emp::Float f = emp::Float(emp::Float());

  emp::Integer* average = new emp::Integer(64, 0, emp::BOB);
  //if(average){std::cout<<"Average assigned size of :"<<average->size();}

  *average = (*sum) / count_emp;

  //std::cout<<"\n=========== count ============ "<<average->reveal<int64_t>(emp::PUBLIC);
  //*sum emp::Float::operator/ *count_emp;
  //const QueryField f(*sum, sum->length, 0);

  const QueryField f(*average, average->length, 0);
  std::unique_ptr<QueryTable> aggregate_output = std::make_unique<QueryTable>
      (input->GetIsEncrypted(), 1);
  aggregate_output->GetTuple(0)->PutField(0, &f);

  //int table_sum = sum->reveal<int64_t>(emp::PUBLIC);
  //std::cout << "\nOutput of sum " << table_sum;
  //std::cout << "\nAverage of the table: "<< emp::Float(table_sum/count_star);

  // TODO: implement this for float as well (returning int(AVG) for now

  return aggregate_output;
}

