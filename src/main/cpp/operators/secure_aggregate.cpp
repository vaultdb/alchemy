//
// Created by madhav on 2/17/20.
//

#include "secure_aggregate.h"
#include "operators/support/aggregate_id.h"
#include <querytable/query_table.h>

std::unique_ptr<QueryTable> Aggregate(QueryTable *input,
                                      const AggregateDef &def) {

  vaultdb::types::Value trueBool = (input->isEncrypted())
                                       ? vaultdb::types::Value(emp::Bit(1))
                                       : vaultdb::types::Value(true);

  std::unique_ptr<QueryTable> aggregate_output;

  int aggregateCount = def.scalarAggregates.size();

  if (def.groupByOrdinals.size() == 0) {
    aggregate_output =
        std::make_unique<QueryTable>(1, aggregateCount, input->isEncrypted());
  } else {
    aggregate_output = std::make_unique<QueryTable>(
        input->getTupleCount(), aggregateCount, input->isEncrypted());
  }

  emp::Integer one(64, 1, emp::PUBLIC);
  emp::Integer zero(64, 0, emp::PUBLIC);

  vaultdb::types::Value one_enc(vaultdb::types::TypeId::ENCRYPTED_INTEGER32,
                                one);
  vaultdb::types::Value zero_enc(vaultdb::types::TypeId::ENCRYPTED_INTEGER32,
                                 zero);

  vaultdb::types::Value one_int((int32_t)1);
  vaultdb::types::Value zero_int((int32_t)0);

  vaultdb::types::Value prev_dummy = (!trueBool); // equivalent to False?

  // result vector for each tuple in the relation
  std::vector<vaultdb::types::Value> res_vec;
  for (int idx = 0; idx < def.scalarAggregates.size(); idx++) {
    res_vec[idx] = input->getTuple(0).getField(idx).getValue();
  }

  std::map<int, std::pair<vaultdb::types::Value, vaultdb::types::Value>> r_avg;

  vaultdb::types::Value is_not_dummy(trueBool);
  std::vector<vaultdb::types::Value> gby_vector;

  // shadow vector to keep track of the running average
  // initializing running average (shadow vector)
  // same shadow vector can be used for each bin
  for (int idx = 0; idx < def.scalarAggregates.size(); idx++) {
    if (def.scalarAggregates[idx].id == AggregateId ::AVG) {
      vaultdb::types::Value val(input->getTuple(0).getField(idx).getValue());
      vaultdb::types::Value tick = zero_enc;
      r_avg[idx] = std::make_pair(val, tick);
    }
  }

  // check if the query is scalar (no groupby clause)
  if (def.groupByOrdinals.size() == 0) {

    // TODO (shawshank-cs) : Verify - starting loop from first row
    //  might have caused a bug in the running avg logic as we already
    //  have our running_count as 0 but running_sum initialized to first row
    for (int row = 0; row < input->getTupleCount(); row++) {

      // dummy flag to determine if the tuple is a Dummy value
      is_not_dummy = input->getTuple(row).getDummyTag();

      for (int idx = 0; idx < def.scalarAggregates.size(); idx++) {

        // switch on the Aggregate ID
        switch (def.scalarAggregates[idx].id) {

        case AggregateId::COUNT: {
            /* JR revert this before build
            res_vec[idx] = emp::If(is_not_dummy.getEmpBit(),
                                 (res_vec[idx] + one_enc), res_vec[idx]); */
          break;
        }
        case AggregateId::SUM: {
          /*res_vec[idx] =
              emp::If(is_not_dummy.getEmpBit(),
                      (res_vec[idx] +
                       (input->getTuple(row)
                             .getField(def.scalarAggregates[idx].ordinal)
                             .getValue())),
                      res_vec[idx]); */

          //              emp::If(isDummy, zero,
          //                      *input->getTuple(row)
          //                           ->GetField(def.scalarAggregates[idx].ordinal)
          //                           ->GetValue());
          //          AddToCount(res_vec[idx], res_vec[idx]);
          break;
        }
        case AggregateId::AVG: {
          /*r_avg[idx].first =
              emp::If(is_not_dummy.getEmpBit(),
                      r_avg[idx].first +
                          (input->getTuple(row)
                                .getField(def.scalarAggregates[idx].ordinal)
                                .getValue()),
                      r_avg[idx].first);
          r_avg[idx].second =
              emp::If(is_not_dummy.getEmpBit(), (r_avg[idx].second + one_enc),
                      r_avg[idx].second);*/
          break;
        }
        }
      }
      // accumulating sum and count from the shadow vector and computing AVG()
      for (int idx = 0; idx < def.scalarAggregates.size(); idx++) {
        if (def.scalarAggregates[idx].id == AggregateId ::AVG) {
          res_vec[idx] = r_avg[idx].first / r_avg[idx].second;
        }
      }
    }

    // creates resultant relation; inserting QueryField into tuple (just 1 row)
    for (int i = 0; i < def.scalarAggregates.size(); i++) {
      const QueryField f(i, res_vec[i]);
      aggregate_output->getTuple(0).putField(i, f);
    }

    vaultdb::types::Value curr_dval(trueBool);
    aggregate_output->getTuple(0).setDummyTag(curr_dval);
  }

  // otherwise, if the GROUP-BY clause is present
  else {

    vaultdb::types::Value prev_gby =
        input->getTuple(0).getField(0).getValue();
    vaultdb::types::Value curr_gby =
        input->getTuple(0).getField(0).getValue();

    // initializing group-by vector only if there's a GB clause
    for (int idx = 0; idx < def.groupByOrdinals.size(); idx++) {
      gby_vector[idx] = input->getTuple(0).getField(idx).getValue();
    }

    for (int cursor = 0; cursor < input->getTupleCount(); cursor++) {

      vaultdb::types::Value gby_equality = trueBool;
      // assuming that every tuple belongs to the previous tuple's bin
      // this condition holds true even for the very first tuple
      // Since the default value is true, result vector would remain unaffected

      is_not_dummy = input->getTuple(cursor).getDummyTag();

      if (cursor != 0)
        prev_dummy = aggregate_output->getTuple(cursor - 1).getDummyTag();

      // groupby_eq (equality): bool=> checks for equality with prev. tuples GB
      for (int idx = 0; idx < def.groupByOrdinals.size(); idx++) {

        int ord = def.groupByOrdinals[idx];
        curr_gby = input->getTuple(cursor).getField(ord).getValue();

        /*gby_equality =
            emp::If((curr_gby.operator==(gby_vector[idx])).getEmpBit(),
                    gby_equality, !trueBool); */

        gby_vector[idx] = curr_gby;
      }

      for (int idx = 0; idx < def.scalarAggregates.size(); idx++) {

        switch (def.scalarAggregates[idx].id) {
        case AggregateId::COUNT: {
          /*res_vec[idx] =
              emp::If(is_not_dummy.getEmpBit(), one_enc, zero_enc) +
              emp::If(gby_equality.getEmpBit(), res_vec[idx], zero_enc); */
          break;
        }
        case AggregateId::SUM: {
         /* res_vec[idx] =
              emp::If(is_not_dummy.getEmpBit(),
                      (input->getTuple(cursor)
                            .getField(def.scalarAggregates[idx].ordinal)
                            .getValue()),
                      zero_enc) +
              emp::If(gby_equality.getEmpBit(), res_vec[idx], zero_enc); */
          break;
        }
        case AggregateId::AVG: {
          if (cursor != 0) {
            /*r_avg[idx].first =
                emp::If(gby_equality.getEmpBit(), r_avg[idx].first, zero_enc);
            r_avg[idx].second =
                emp::If(gby_equality.getEmpBit(), r_avg[idx].second, zero_enc); */
          }
          /*r_avg[idx].first =
              r_avg[idx].first +
              emp::If(is_not_dummy.getEmpBit(),
                      (input->getTuple(cursor)
                            .getField(def.scalarAggregates[idx].ordinal)
                            .getValue()),
                      zero_enc);
          r_avg[idx].second =
              r_avg[idx].second +
              emp::If(is_not_dummy.getEmpBit(), one_enc, zero_enc);
          res_vec[idx] = r_avg[idx].first / r_avg[idx].second;
          break;*/
        }
        }
      }
      // updating current dummy (nested If)
      if (cursor != 0) {
/*        is_not_dummy = emp::If(gby_equality.getEmpBit(),
                               emp::If(aggregate_output->getTuple(cursor - 1)
                                            .getDummyTag()
                                            .getEmpBit(),
                                       trueBool, !trueBool),
                               is_not_dummy);*/

        // updating previous tuple's dummy (if necessary), and setting that val
       /* prev_dummy = emp::If(gby_equality.getEmpBit(),
                             emp::If(aggregate_output->getTuple(cursor - 1)
                                          .getDummyTag()
                                          .getEmpBit(),
                                     prev_dummy, trueBool),
                             prev_dummy); */

        vaultdb::types::Value prev_dval(prev_dummy);
        aggregate_output->getTuple(cursor - 1).setDummyTag(prev_dval);
      }

      for (int i = 0; i < def.scalarAggregates.size(); i++) {
        const QueryField f(i, res_vec[i]);
        aggregate_output->getTuple(cursor).putField(i, f);
      }
      vaultdb::types::Value curr_dval(is_not_dummy);
      aggregate_output->getTuple(cursor).setDummyTag(curr_dval);
    }
  }
  return aggregate_output;
}