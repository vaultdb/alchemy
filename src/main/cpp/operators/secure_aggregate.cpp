#include "secure_aggregate.h"
#include "operators/support/aggregate_id.h"

using namespace vaultdb::types;

std::unique_ptr<QueryTable> Aggregate(QueryTable *input,
                                      const AggregateDef &def) {

    Value trueBool = (input->isEncrypted())
                     ? Value(emp::Bit(1))
                     : Value(true);
    Value falseBool = !trueBool; // equivalent to False?

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

    Value one_enc(vaultdb::types::TypeId::ENCRYPTED_INTEGER32,
                  one);
    Value zero_enc(vaultdb::types::TypeId::ENCRYPTED_INTEGER32,
                   zero);

    Value one_int((int64_t)1);
    Value zero_int((int64_t)0);

    Value prev_dummy = falseBool;

    // result vector for each tuple in the relation
    std::vector<Value> res_vec;
    for (int idx = 0; idx < def.scalarAggregates.size(); idx++) {
        res_vec[idx] = input->getTuplePtr(0)->getField(idx).getValue();
    }

    std::map<int, std::pair<Value, Value>> r_avg;

    Value is_not_dummy(trueBool);
    std::vector<Value> gby_vector;

    // shadow vector to keep track of the running average
    // initializing running average (shadow vector)
    // same shadow vector can be used for each bin
    for (int idx = 0; idx < def.scalarAggregates.size(); idx++) {
        if (def.scalarAggregates[idx].type == AggregateId ::AVG) {
            Value val(
                    input->getTuplePtr(0)->getField(idx).getValue());
            Value tick = zero_enc;
            std::pair<Value, Value> thePair = std::make_pair(val, tick);
            r_avg[idx] = thePair;
        }
    }

    // check if the query is scalar (no group-by clause)
    if (def.groupByOrdinals.size() == 0) {

        // TODO (shawshank-cs) : Verify - starting loop from first row
        //  might have caused a bug in the running avg logic as we already
        //  have our running_count as 0 but running_sum initialized to first row
        for (int row = 0; row < input->getTupleCount(); row++) {

            // dummy flag to determine if the tuple is a Dummy value
            is_not_dummy = input->getTuplePtr(row)->getDummyTag();

            for (int idx = 0; idx < def.scalarAggregates.size(); idx++) {

                // switch on the Aggregate ID
                switch (def.scalarAggregates[idx].type) {

                    case AggregateId::COUNT: {
                        Value curr_val = res_vec[idx];
                        Value next_val = res_vec[idx] + one_enc;

                        res_vec[idx] =
                                Value::obliviousIf(is_not_dummy.getEmpBit(), next_val, curr_val);
                        break;
                    }
                    case AggregateId::SUM: {

                        Value curr_val = res_vec[idx];
                        Value next_val =
                                curr_val + input->getTuplePtr(row)
                                        ->getField(def.scalarAggregates[idx].ordinal)
                                        .getValue();
                        res_vec[idx] =
                                Value::obliviousIf(is_not_dummy.getEmpBit(), next_val, curr_val);

                        break;
                    }
                    case AggregateId::AVG: {
                        Value curr_val = r_avg[idx].first;
                        Value next_val =
                                curr_val + (input->getTuplePtr(row)
                                        ->getField(def.scalarAggregates[idx].ordinal)
                                        .getValue());
                        r_avg[idx].first =
                                Value::obliviousIf(is_not_dummy.getEmpBit(), next_val, curr_val);

                        curr_val = r_avg[idx].second;
                        next_val = curr_val + one_enc;
                        r_avg[idx].second =
                                Value::obliviousIf(is_not_dummy.getEmpBit(), next_val, curr_val);
                        break;
                    }
                }
            }
            // accumulating sum and count from the shadow vector and computing AVG()
            for (int idx = 0; idx < def.scalarAggregates.size(); idx++) {
                if (def.scalarAggregates[idx].type == AggregateId ::AVG) {
                    res_vec[idx] = r_avg[idx].first / r_avg[idx].second;
                }
            }
        }

        // creates resultant relation; inserting QueryField into tuple (just 1 row)
        for (int i = 0; i < def.scalarAggregates.size(); i++) {
            const QueryField f(i, res_vec[i]);
            aggregate_output->getTuplePtr(0)->putField(f);
        }

        Value curr_dval(trueBool);
        aggregate_output->getTuplePtr(0)->setDummyTag(curr_dval);
    }

        // otherwise, if the GROUP-BY clause is present
    else {

        Value prev_gby =
                input->getTuplePtr(0)->getField(0).getValue();
        Value curr_gby =
                input->getTuplePtr(0)->getField(0).getValue();

        // initializing group-by vector only if there's a GB clause
        for (int idx = 0; idx < def.groupByOrdinals.size(); idx++) {
            gby_vector[idx] = input->getTuplePtr(0)->getField(idx).getValue();
        }

        for (int cursor = 0; cursor < input->getTupleCount(); cursor++) {

            Value gby_equality = trueBool;
            // assuming that every tuple belongs to the previous tuple's bin
            // this condition holds true even for the very first tuple
            // Since the default value is true, result vector would remain unaffected

            is_not_dummy = input->getTuplePtr(cursor)->getDummyTag();

            if (cursor != 0)
                prev_dummy = aggregate_output->getTuplePtr(cursor - 1)->getDummyTag();

            // groupby_eq (equality): bool=> checks for equality with prev. tuples GB
            for (int idx = 0; idx < def.groupByOrdinals.size(); idx++) {

                int ord = def.groupByOrdinals[idx];
                curr_gby = input->getTuplePtr(cursor)->getField(ord).getValue();
                gby_equality = Value::obliviousIf(
                        (curr_gby.operator==(gby_vector[idx])).getEmpBit(), gby_equality,
                        falseBool);
                gby_vector[idx] = curr_gby;
            }

            for (int idx = 0; idx < def.scalarAggregates.size(); idx++) {

                switch (def.scalarAggregates[idx].type) {
                    case AggregateId::COUNT: {
                        res_vec[idx] =
                                Value::obliviousIf(is_not_dummy.getEmpBit(), one_enc, zero_enc) +
                                Value::obliviousIf(gby_equality.getEmpBit(), res_vec[idx],
                                                   zero_enc);
                        break;
                    }
                    case AggregateId::SUM: {
                        Value next_val = input->getTuplePtr(cursor)
                                ->getField(def.scalarAggregates[idx].ordinal)
                                .getValue();
                        res_vec[idx] =
                                Value::obliviousIf(is_not_dummy.getEmpBit(), next_val, zero_enc) +
                                Value::obliviousIf(gby_equality.getEmpBit(), res_vec[idx],
                                                   zero_enc);
                        break;
                    }
                    case AggregateId::AVG: {
                        if (cursor != 0) {
                            r_avg[idx].first = Value::obliviousIf(gby_equality.getEmpBit(),
                                                                  r_avg[idx].first, zero_enc);
                            r_avg[idx].second = Value::obliviousIf(gby_equality.getEmpBit(),
                                                                   r_avg[idx].second, zero_enc);
                        }
                        Value next_val = input->getTuplePtr(cursor)
                                ->getField(def.scalarAggregates[idx].ordinal)
                                .getValue();
                        r_avg[idx].first =
                                r_avg[idx].first +
                                Value::obliviousIf(is_not_dummy.getEmpBit(), next_val, zero_enc);
                        r_avg[idx].second =
                                r_avg[idx].second +
                                Value::obliviousIf(is_not_dummy.getEmpBit(), one_enc, zero_enc);
                        res_vec[idx] = r_avg[idx].first / r_avg[idx].second;
                        break;
                    }
                }
            }
            // updating current dummy (nested If)
            if (cursor != 0) {
                Value dummy_check =
                        Value::obliviousIf(aggregate_output->getTuplePtr(cursor - 1)
                                                   ->getDummyTag()
                                                   .getEmpBit(),
                                           trueBool, falseBool);
                is_not_dummy = Value::obliviousIf(gby_equality.getEmpBit(), dummy_check,
                                                  is_not_dummy);

                dummy_check =
                        Value::obliviousIf(aggregate_output->getTuplePtr(cursor - 1)
                                                   ->getDummyTag()
                                                   .getEmpBit(),
                                           prev_dummy, trueBool);
                // updating previous tuple's dummy (if necessary)
                prev_dummy = Value::obliviousIf(gby_equality.getEmpBit(), dummy_check,
                                                prev_dummy);

                Value prev_dval(prev_dummy);
                aggregate_output->getTuplePtr(cursor - 1)->setDummyTag(prev_dval);
            }

            for (int i = 0; i < def.scalarAggregates.size(); i++) {
                const QueryField f(i, res_vec[i]);
                QueryTuple *tuple = aggregate_output->getTuplePtr(cursor);
                tuple->putField(f);
                aggregate_output->putTuple(cursor, *tuple);
            }
            Value curr_dval(is_not_dummy);
            aggregate_output->getTuplePtr(cursor)->setDummyTag(curr_dval);
        }
    }
    return aggregate_output;
}