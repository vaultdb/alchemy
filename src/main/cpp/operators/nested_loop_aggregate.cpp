#include <util/type_utilities.h>
#include <util/data_utilities.h>
#include <util/field_utilities.h>

#include "nested_loop_aggregate.h"
#include <query_table/plain_tuple.h>
#include <query_table/secure_tuple.h>



using namespace vaultdb;
using namespace std;

template<typename B>
NestedLoopAggregate<B>::NestedLoopAggregate(Operator<B> *child, const vector<int32_t> &groupBys,
                                      const vector<ScalarAggregateDefinition> &aggregates,
				      const SortDefinition &sort,
				      const int & output_card) : Operator<B>(child, sort),
                                                                    aggregate_definitions_(aggregates),
								    group_by_(groupBys), output_cardinality_(output_card)
{
  setup();
}


template<typename B>
NestedLoopAggregate<B>::NestedLoopAggregate(Operator<B> *child, const vector<int32_t> &groupBys,
                                      const vector<ScalarAggregateDefinition> &aggregates, 
				      const int output_card) : Operator<B>(child, SortDefinition()),
                                                                    aggregate_definitions_(aggregates),
                                                                    group_by_(groupBys), output_cardinality_(output_card) {
    output_cardinality_ = output_card;
    this->setOperatorCardinality(output_cardinality_);
    setup();
 }

template<typename B>
NestedLoopAggregate<B>::NestedLoopAggregate(shared_ptr<QueryTable<B>> child, const vector<int32_t> &groupBys,
                                      const vector<ScalarAggregateDefinition> &aggregates,
				      const SortDefinition &sort,
				      const int & output_card) : Operator<B>(child, sort),
                                                                    aggregate_definitions_(aggregates),
                                                                    group_by_(groupBys), output_cardinality_(output_card) {

      setup();
 }

template<typename B>
NestedLoopAggregate<B>::NestedLoopAggregate(shared_ptr<QueryTable<B>> child, const vector<int32_t> &groupBys,
                                      const vector<ScalarAggregateDefinition> &aggregates,
				      const int & output_card) : Operator<B>(child, SortDefinition()),
                      aggregate_definitions_(aggregates),
                      group_by_(groupBys) {

      setup();
 }


template<typename B>
shared_ptr<QueryTable<B> > NestedLoopAggregate<B>::runSelf() {
    shared_ptr<QueryTable<B> > input = Operator<B>::children_[0]->getOutput();
    shared_ptr<QuerySchema> input_schema = input->getSchema();
    shared_ptr<QuerySchema> output_schema(new QuerySchema(Operator<B>::output_schema_));



    if(output_cardinality_ == 0) { // naive case - go full oblivious
      output_cardinality_ = input->getTupleCount();
    }
    
    // output sort order equal to first group-by-col-count entries in input sort order
    // TODO: make this only work when we are sorted on the group by cols.
    //  May produce incorrect results otherwise
//    SortDefinition inputSort = input->getSortOrder();
//    SortDefinition outputSort = vector<ColumnSort>(inputSort.begin(), inputSort.begin() + group_by_.size());

    Operator<B>::output_ = shared_ptr<QueryTable<B>>(new QueryTable<B>(output_cardinality_, Operator<B>::output_schema_, SortDefinition()));

    vector<vector<shared_ptr<UnsortedAggregateImpl<B>>> > per_tuple_aggregators;
    //confirm all output dummyTag as true
    for (int i = 0; i < output_cardinality_; ++i) {
        NestedLoopAggregate<B>::output_->getTuple(i).setDummyTag(true);
        per_tuple_aggregators.emplace_back(vector<shared_ptr<UnsortedAggregateImpl<B>>>());
        vector<shared_ptr<UnsortedAggregateImpl<B>> > row_aggregators(aggregators_.size());
        for(int j = 0; j < aggregators_.size(); ++j) {
            shared_ptr<UnsortedAggregateImpl<B> > a = (aggregators_[j]->agg_type_ == AggregateId::AVG)
                                                      ?   shared_ptr<UnsortedAggregateImpl<B> >(new UnsortedAvgImpl<B>(AggregateId::AVG, aggregators_[j]->field_type_, aggregators_[j]->input_ordinal_, aggregators_[j]->output_ordinal_))
                                                     :     aggregators_[j];
            per_tuple_aggregators[i].emplace_back(a);
        }
    }


    // create output tuples with managed memory for ease of use
    for(int i = 0; i < input->getTupleCount(); ++i) {

        QueryTuple<B> input_row = (*input)[i];
        B input_dummy = input_row.getDummyTag();
        B matched = FieldUtilities::select(input_dummy, B(true), B(false)); // already "matched" if dummy

        for (int j = 0; j < output_cardinality_; ++j) {
            QueryTuple<B> output_row = (*Operator<B>::output_)[j];
            B group_by_match = groupByMatch(input_row, output_row);
            B output_dummy = output_row.getDummyTag();

            // if output is dummy and no match so far, then initialize group-by cols
            B initialize_group_by = output_dummy & !matched;

            for (int k = 0; k < group_by_.size(); ++k) {
                Field<B> src = input_row.getField(group_by_[k]);
                Field<B> dst = Field<B>::If(initialize_group_by, src, output_row.getField(k));
                output_row.setField(k, dst);
            }

            for (int k = 0; k < aggregators_.size(); ++k) {
                shared_ptr<UnsortedAggregateImpl<B> > a = per_tuple_aggregators[j][k];
                a->update(input_row, output_row, matched, group_by_match);
            } // end aggregators


            // if group-by match or output_dummy, then matched = true
            matched = FieldUtilities::select(group_by_match | initialize_group_by, B(true), matched);
            output_dummy =  FieldUtilities::select(initialize_group_by, B(false), output_dummy);
            output_row.setDummyTag(output_dummy);

        } // end for each  output tuple
    }// end for each input tuple


    return Operator<B>::output_;

}



template<typename B>
B NestedLoopAggregate<B>::groupByMatch(const QueryTuple<B> &input, const QueryTuple<B> &output) const {

    B result = (input.getField(group_by_[0]) == output.getField(0));
    size_t cursor = 1;

    while(cursor < group_by_.size()) {
        result = result &
                 (input.getField(group_by_[cursor]) == output.getField(cursor));
        ++cursor;
    }

    // if one or both inputs are dummies, then set to NOT matched
    result = FieldUtilities::select(input.getDummyTag() | output.getDummyTag(), B(false), result);
    return result;
}


template<typename B>
QuerySchema NestedLoopAggregate<B>::generateOutputSchema(const QuerySchema & input_schema) const {
    QuerySchema outputSchema;
    size_t i;

    for(i = 0; i < group_by_.size(); ++i) {
        QueryFieldDesc srcField = input_schema.getField(group_by_[i]);
        QueryFieldDesc dstField(i, srcField.getName(), srcField.getTableName(), srcField.getType());
        dstField.setStringLength(srcField.getStringLength());
        outputSchema.putField(dstField);
    }


    for(i = 0; i < aggregate_definitions_.size(); ++i) {
        ScalarAggregateDefinition agg = aggregate_definitions_[i];
        FieldType agg_type = (agg.ordinal >= 0) ?
                             input_schema.getField(agg.ordinal).getType() :
                             (std::is_same_v<B, emp::Bit> ? FieldType::SECURE_LONG : FieldType::LONG);

        QueryFieldDesc fieldDesc(i + group_by_.size(), aggregate_definitions_[i].alias, "", agg_type);
        outputSchema.putField(fieldDesc);
    }

    outputSchema.initializeFieldOffsets();
    return outputSchema;

}

template<typename B>
void NestedLoopAggregate<B>::setup() {
    QuerySchema input_schema = Operator<B>::getChild(0)->getOutputSchema();
    int output_ordinal = group_by_.size();

    for(ScalarAggregateDefinition agg : aggregate_definitions_) {
        // for most aggs the output type is the same as the input type
        // for COUNT(*) and others with an ordinal of < 0, then we set it to an INTEGER instead
        FieldType aggValueType = (agg.ordinal >= 0) ?
                                 input_schema.getField(agg.ordinal).getType() :
                                 (std::is_same_v<B, emp::Bit> ? FieldType::SECURE_LONG : FieldType::LONG);

        shared_ptr<UnsortedAggregateImpl<B>> a = (agg.type == AggregateId::AVG) ?
                  shared_ptr<UnsortedAggregateImpl<B>>(new UnsortedAvgImpl<B>(AggregateId::AVG, aggValueType, agg.ordinal, output_ordinal))
                : shared_ptr<UnsortedAggregateImpl<B>>(new UnsortedStatelessAggregateImpl<B>(agg.type, aggValueType, agg.ordinal, output_ordinal));

        aggregators_.push_back(a);
        ++output_ordinal;
    }



    Operator<B>::output_schema_ = generateOutputSchema(input_schema);

}

template<typename B>
string NestedLoopAggregate<B>::getOperatorType() const {
    return "NestedLoopAggregate";
}

template<typename B>
string NestedLoopAggregate<B>::getParameters() const {
    stringstream  ss;
   ss << "group-by: (" << group_by_[0];
   for(uint32_t i = 1; i < group_by_.size(); ++i)
       ss << ", " << group_by_[i];

   ss << ") aggs: (" << aggregate_definitions_[0].toString();

   for(uint32_t i = 1; i < aggregate_definitions_.size(); ++i) {
       ss << ", " << aggregate_definitions_[i].toString();
   }

   ss << ")";
   return ss.str();
}



template class vaultdb::NestedLoopAggregate<bool>;
template class vaultdb::NestedLoopAggregate<emp::Bit>;
