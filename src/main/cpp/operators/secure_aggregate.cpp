//
// Created by madhav on 2/17/20.
//

#include "secure_aggregate.h"
#include "operators/support/aggregate_id.h"
#include <common/macros.h>
#include <querytable/query_table.h>

#define AGG_COUNT(is_dummy)                                                    \
  do {                                                                         \
    not_dummy = emp::If(is_dummy, zero, one);                                  \
  } while (0)

#define AGG_SUM(is_dummy, row, idx)                                            \
  do {                                                                         \
    not_dummy = emp::If(is_dummy, zero,                                        \
                        input->getTuple(cursor)                               \
                             .getField(def.scalarAggregates[idx].ordinal)                 \
                             .getValue()                                      \
                             .getEmpInt());                                   \
  } while (0)

#define SCALAR_COUNT(is_dummy, result_ref)                                        \
  do {                                                                         \
    vaultdb::types::Value res(vaultdb::types::TypeId::ENCRYPTED_INTEGER32,     \
                              emp::If(is_dummy, one, zero));                   \
    AddToCount(result_ref, res);                                               \
  } while (0)

#define SCALAR_SUM(isDummy, result_ref, row, idx)                                \
  do {                                                                           \
  emp::Integer sumVal = input->getTuple(row)           \
    .getField(def.scalarAggregates[idx].ordinal).getValue().getEmpInt(); \
    emp::Integer res = emp::If(isDummy, zero, sumVal);             \
    AddToCount(result_ref, res);                                               \
  } while (0)

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
  }
  else {
    aggregate_output = std::make_unique<QueryTable>(
        input->getTupleCount(), aggregateCount, input->isEncrypted());
  }

  emp::Integer one(64, 1, emp::PUBLIC);
  emp::Integer zero(64, 0, emp::PUBLIC);
  emp::Bit isDummy(false, emp::PUBLIC);

  vaultdb::types::Value one_enc(vaultdb::types::TypeId::ENCRYPTED_INTEGER32,
                                one);
  vaultdb::types::Value zero_enc(vaultdb::types::TypeId::ENCRYPTED_INTEGER32,
                                 zero);

  vaultdb::types::Value one_int((int32_t)1);
  vaultdb::types::Value zero_int((int32_t)0);

  emp::Integer not_dummy(64, 0, emp::PUBLIC);
  emp::Bit true_dummy(true, emp::PUBLIC);
  emp::Bit false_dummy(false, emp::PUBLIC);
  emp::Bit prev_dummy(false, emp::PUBLIC);

  vaultdb::types::Value last_dummy = (!trueBool); // equivalent to False?

  // result vector for each tuple in the relation
  //  std::vector<emp::Integer> result_vector;

  std::vector<vaultdb::types::Value> res_vec;
  for (int idx = 0; idx < def.scalarAggregates.size(); idx++) {
    res_vec[idx] = input->getTuple(0).getField(idx).getValue();
  }

  //  for (int i = 0; i < def.scalarAggregates.size(); i++) {
  //    result_vector.emplace_back(64, 0, emp::BOB);
  //  }

  std::map<int, std::pair<vaultdb::types::Value, vaultdb::types::Value>> r_avg;

  // shadow vector to keep track of the running average
  //  std::map<int, std::pair<emp::Integer, emp::Integer>> running_avg;

  // copy constructors for initializing pair<emp:Int,emp:Int>
  emp::Integer i1(64, 0, emp::PUBLIC);
  emp::Integer i2(64, 0, emp::PUBLIC);

  std::vector<vaultdb::types::Value> gby_vector;

  // same shadow vector can be used for each bin
  for (int idx = 0; idx < def.scalarAggregates.size(); idx++) {
    if (def.scalarAggregates[idx].id == AggregateId ::AVG) {
      vaultdb::types::Value val(*input->getTuple(0)->GetField(idx)->GetValue());
      vaultdb::types::Value tick = (input->isEncrypted()) ? zero_enc : zero_int;
      r_avg[idx] = std::make_pair(val, tick);
    }
  }

  // check if the query is scalar (no groupby clause)
  if (def.groupByOrdinals.size() == 0) {

    //    emp::Bit *isDummy = new emp::Bit(false, emp::PUBLIC);
    vaultdb::types::Value is_not_dummy(trueBool);

    // TODO (shawshank-cs) : Verify - starting loop from first row
    //  might have caused a bug in the running avg logic as we already
    //  have our running_count as 0 but running_sum initialized to first row
    for (int row = 0; row < input->getTupleCount(); row++) {

      // dummy flag to determine if the tuple is a Dummy value
      is_not_dummy = input->getTuple(row).getDummyTag();
      //      isDummy = *input->getTuple(row)->GetDummyTag()->getEmpBit();

      for (int idx = 0; idx < def.scalarAggregates.size(); idx++) {

        // switch on the Aggregate ID
        switch (def.scalarAggregates[idx].id) {

        case AggregateId::COUNT: {
          // SCALAR_COUNT(isDummy, result_vector[idx]);
          res_vec[idx] = emp::If(
              input->isEncrypted(),
              emp::If((is_not_dummy.getEmpBit() == trueBool.getEmpBit()),
                      (res_vec[idx] + one_enc), res_vec[idx]),
              emp::If((is_not_dummy.getBool() == trueBool.getBool()),
                      (res_vec[idx] + one_int), res_vec[idx]));
          break;
        }
        case AggregateId::SUM: {

          // SCALAR_SUM(isDummy, res_vec[idx], row, idx);
          res_vec[idx] = emp::If(
              input->isEncrypted(),
              emp::If((is_not_dummy.getEmpBit() == trueBool.getEmpBit()),
                      (res_vec[idx] +
                       (input->getTuple(row)
                             .getField(def.scalarAggregates[idx].ordinal)
                             .getValue())),
                      res_vec[idx]),
              emp::If((is_not_dummy.getBool() == trueBool.getBool()),
                      (res_vec[idx] +
                       (*input->getTuple(row)
                             .getField(def.scalarAggregates[idx].ordinal)
                             .getValue())),
                      res_vec[idx]));

          //              emp::If(isDummy, zero,
          //                      *input->getTuple(row)
          //                           ->GetField(def.scalarAggregates[idx].ordinal)
          //                           ->GetValue());
          //          AddToCount(res_vec[idx], res_vec[idx]);
          break;
        }
        case AggregateId::AVG: {
          // SCALAR_SUM(isDummy, r_avg[idx].first, row, idx);
          r_avg[idx].first = emp::If(
              input->isEncrypted(),
              emp::If((is_not_dummy.getEmpBit() == trueBool.getEmpBit()),
                      r_avg[idx].first +
                          (input->getTuple(row)
                                .getField(def.scalarAggregates[idx].ordinal)
                                .getValue()),
                      r_avg[idx].first),
              emp::If((is_not_dummy.getBool() == trueBool.getBool()),
                      (r_avg[idx].first +
                       (input->getTuple(row)
                             .getField(def.scalarAggregates[idx].ordinal)
                             .getValue())),
                      r_avg[idx].first));

          //  SCALAR_COUNT(isDummy, r_avg[idx].second);
          r_avg[idx].second = emp::If(
              input->isEncrypted(),
              emp::If((is_not_dummy.getEmpBit() == trueBool.getEmpBit()),
                      (r_avg[idx].second + one_enc), res_vec[idx]),
              emp::If((is_not_dummy.getBool() == trueBool.getBool()),
                      (r_avg[idx].second + one_int), res_vec[idx]));

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
      const QueryField f(i, types::Value(types::TypeId::ENCRYPTED_INTEGER32, res_vec[i]));
        aggregate_output->getTuple(0).putField(i, f);
    }

    vaultdb::types::Value curr_dval(
            trueBool);
      aggregate_output->getTuple(0).setDummyTag(curr_dval);
  }

  // otherwise, if the GROUPBY clause is present
  else {

    //    emp::Integer prev_group_by(64, 0, emp::PUBLIC);
    //    emp::Integer group_by(64, 0, emp::PUBLIC);

    vaultdb::types::Value prev_gby =
        input->getTuple(0).getField(0).getValue();
    vaultdb::types::Value curr_gby =
        input->getTuple(0).getField(0).getValue();

    // initializing groupby vector only if there's a GB clause
    for (int idx = 0; idx < def.groupByOrdinals.size(); idx++) {
      gby_vector[idx] = *input->getTuple(0)->GetField(idx)->GetValue();
    }

    for (int cursor = 0; cursor < input->getTupleCount(); cursor++) {

      //      emp::Bit groupby_eq(true, emp::PUBLIC);
      vaultdb::types::Value gby_equality = trueBool;
      // assuming that every tuple belongs to the previous tuple's bin
      // this condition holds true even for the very first tuple
      // Since the default value is true, result vector would remain unaffected

      isDummy = input->getTuple(cursor).getDummyTag().getEmpBit();
      if (cursor != 0)
        //        prev_dummy =
        //            *aggregate_output->getTuple(cursor -
        //            1)->GetDummyTag()->getEmpBit();
        last_dummy = *aggregate_output->getTuple(cursor - 1)->GetDummyTag();
        prev_dummy = aggregate_output->getTuple(cursor - 1)
                .getDummyTag()
                .getEmpBit();

      // groupby_eq (equality): bool=> checks for equality with prev. tuples GB
      for (int idx = 0; idx < def.groupByOrdinals.size(); idx++) {
        int ord = def.groupByOrdinals[idx];
        curr_gby = input->getTuple(cursor).getField(ord).getValue();
        //        group_by =
        //            *input->getTuple(cursor)->GetField(ord)->GetValue()->getEmpInt();
        gby_equality =
            If((gby_vector[idx] == curr_gby) ? gby_equality : (!trueBool));
        //        groupby_eq =
        //            If((groupby_vector[idx] == group_by), groupby_eq,
        //            false_dummy);
        gby_vector[idx] = curr_gby;
        //        groupby_vector[idx] = group_by;
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
                     If(aggregate_output->getTuple(cursor - 1)
                             .getDummyTag()
                             .getEmpBit(),
                        true_dummy, false_dummy),
                     isDummy);

        // updating previous tuple's dummy (if necessary), and setting that
        // value
        prev_dummy = If(groupby_eq,
                        If(aggregate_output->getTuple(cursor - 1)
                                .getDummyTag()
                                .getEmpBit(),
                           prev_dummy, true_dummy),
                        prev_dummy);

        vaultdb::types::Value prev_dval(prev_dummy);
        aggregate_output->getTuple(cursor - 1).setDummyTag(prev_dval);
      }

      for (int i = 0; i < def.scalarAggregates.size(); i++) {
        const QueryField f(i, types::Value(types::TypeId::ENCRYPTED_INTEGER32, result_vector[i]));
        aggregate_output->getTuple(cursor).putField(i, f);
      }
      vaultdb::types::Value curr_dval(isDummy);
      aggregate_output->getTuple(cursor).setDummyTag(curr_dval);
    }
  }
  return aggregate_output;
}


// in-place updation of count (helper function)
void AddToCount(vaultdb::types::Value &count_so_far,
                vaultdb::types::Value add) {
  count_so_far = count_so_far + add;
  return;
}
