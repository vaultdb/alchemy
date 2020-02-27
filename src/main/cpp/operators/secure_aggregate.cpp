//
// Created by madhav on 2/17/20.
//

#include "secure_aggregate.h"
#include <querytable/query_table.h>
#include <common/macros.h>
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
  // emp::Integer *value = new emp::Integer
  // int count_star = input->GetNumTuples();

  emp::Integer count_emp(64, 0, emp::BOB);

  // std::cout<<"Count(*) = "<<count_star;
  // if(count_emp){std::cout<<"There's our size for count"<<count_emp->size();}

  emp::Integer one(64, 1, emp::PUBLIC);
  emp::Integer zero(64, 1, emp::PUBLIC);
  // types::Value isDummy;
  // emp::Integer value;
  for (int row = 0; row < input->GetNumTuples(); row++) {
    //    *sum = *sum +
    //        emp::If(*input->GetTuple(row)->InitDummy())
    input->GetTuple(row)->InitDummy();
    *isDummy = *input->GetTuple(row)->GetDummyFlag()->GetEmpBit();
    count_emp = count_emp + emp::If(*isDummy, zero, one);
    //*isDummy =
    //*input->GetTuple(row)->GetField(def.index)->GetValue()->GetEmpBit();
    for (int idx = 0; idx < def.index.size(); idx++) {
      sum_vector[idx] =
          sum_vector[idx] + emp::If(*isDummy, zero,
                                    *input->GetTuple(row)
                                         ->GetField(def.index[idx])
                                         ->GetValue()
                                         ->GetEmpInt());


      std::cout << input->GetTuple(row)
                       ->GetField(def.index[idx])
                       ->GetValue()
                       ->GetEmpInt()
                       ->reveal<int64_t>(emp::PUBLIC)
                << "|\t";
    }


    // std::cout<<"\n=========== count ============
    // "<<average->reveal<int64_t>(emp::PUBLIC); *sum emp::Float::operator/
    //*count_emp;

    // std::cout<<"\n=========== count ===========
    // "<<count_emp->reveal<int64_t>(emp::PUBLIC); emp::Float f =
    // emp::Float(emp::Float());
  }
  std::cout << std::endl;
  VAULTDB_DEBUG(count_emp, count_emp.reveal<int64_t>(emp::PUBLIC));
  std::vector<emp::Integer> average_vector;
  for (int i = 0; i < def.index.size(); i++) {
    average_vector.emplace_back(64, 0, emp::BOB);
    average_vector[i] = (sum_vector[i])/ count_emp;
    const QueryField f(average_vector[i], average_vector[i].length, i);
    aggregate_output->GetTuple(0)->PutField(i, &f);
  }

  // int table_sum = sum->reveal<int64_t>(emp::PUBLIC);
  // std::cout << "\nOutput of sum " << table_sum;
  // std::cout << "\nAverage of the table: "<< emp::Float(table_sum/count_star);

  // TODO: implement this for float as well (returning int(AVG) for now
  std::cout << "**************" << std::endl;
  return aggregate_output;
}
