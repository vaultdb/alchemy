#include <util/type_utilities.h>
#include <util/data_utilities.h>
#include <util/field_utilities.h>

#include "group_by_aggregate.h"
#include <query_table/plain_tuple.h>
#include <query_table/secure_tuple.h>
#include "query_table/table_factory.h"
#include "util/system_configuration.h"


using namespace vaultdb;
using namespace std;

template<typename B>
GroupByAggregate<B>::GroupByAggregate(Operator<B> *child, const vector<int32_t> &group_bys,
                                      const vector<ScalarAggregateDefinition> &aggregates,
                                      const SortDefinition &sort) : Operator<B>(child, sort),
                                                                    aggregate_definitions_(aggregates),
                                                                    group_by_(group_bys)
{

    setup();
}


template<typename B>
GroupByAggregate<B>::GroupByAggregate(Operator<B> *child, const vector<int32_t> &group_bys,
                                      const vector<ScalarAggregateDefinition> &aggregates) : Operator<B>(child, SortDefinition()),
                                                                    aggregate_definitions_(aggregates),
                                                                    group_by_(group_bys) {

    setup();
}

template<typename B>
GroupByAggregate<B>::GroupByAggregate(QueryTable<B> *child, const vector<int32_t> &groupBys,
                                      const vector<ScalarAggregateDefinition> &aggregates,
                                      const SortDefinition &sort) : Operator<B>(child, sort),
                                                                    aggregate_definitions_(aggregates),
                                                                    group_by_(groupBys) {

    setup();
}

template<typename B>
GroupByAggregate<B>::GroupByAggregate(QueryTable<B> *child, const vector<int32_t> &groupBys,
                                      const vector<ScalarAggregateDefinition> &aggregates) : Operator<B>(child, SortDefinition()),
                                                                    aggregate_definitions_(aggregates),
                                                                    group_by_(groupBys){

    setup();
}

template<typename B>
GroupByAggregate<B>::GroupByAggregate(Operator<B> *child, const vector<int32_t> &groupBys,
                                      const vector<ScalarAggregateDefinition> &aggregates, const bool &check_sort)
        :  Operator<B>(child, SortDefinition()), check_sort_(check_sort), aggregate_definitions_(aggregates),
                                                                    group_by_(groupBys) {
    setup();
}

template<typename B>
GroupByAggregate<B>::GroupByAggregate(Operator<B> *child, const vector<int32_t> &groupBys,
                                      const vector<ScalarAggregateDefinition> &aggregates, const bool &check_sort, const int &json_cardinality)
        :  Operator<B>(child, SortDefinition()), check_sort_(check_sort), aggregate_definitions_(aggregates),
           group_by_(groupBys), json_cardinality_(json_cardinality) {
    setup();
}


template<typename B>
GroupByAggregate<B>::GroupByAggregate(Operator<B> *child, const vector<int32_t> &groupBys,
                                      const vector<ScalarAggregateDefinition> &aggregates, const bool &check_sort, const SortDefinition &effective_sort, const map<int32_t, std::set<int32_t>> &functional_dependency)
        :  Operator<B>(child, SortDefinition()), check_sort_(check_sort), aggregate_definitions_(aggregates),
           group_by_(groupBys), effective_sort_(effective_sort), functional_dependency_(functional_dependency) {
    setup();
}

template<typename B>
GroupByAggregate<B>::GroupByAggregate(Operator<B> *child, const vector<int32_t> &groupBys,
                                      const vector<ScalarAggregateDefinition> &aggregates, const bool &check_sort, const int &json_cardinality, const SortDefinition &effective_sort, const map<int32_t, std::set<int32_t>> &functional_dependency)
        :  Operator<B>(child, SortDefinition()), check_sort_(check_sort), aggregate_definitions_(aggregates),
           group_by_(groupBys), json_cardinality_(json_cardinality), effective_sort_(effective_sort), functional_dependency_(functional_dependency) {
    setup();
}


template<typename B>
QueryTable<B> *GroupByAggregate<B>::runSelf() {
    QueryTable<B> *input = Operator<B>::getChild()->getOutput();
    QuerySchema input_schema = input->getSchema();

    this->start_time_ = clock_start();
    this->start_gate_cnt_ = this->system_conf_.andGateCount();

    int output_cursor = group_by_.size();
    for (ScalarAggregateDefinition agg: aggregate_definitions_) {
        GroupByAggregateImpl<B> *agg_impl = aggregateFactory(agg.type, agg.ordinal,
                                                                 input_schema.getField(agg.ordinal));

        // if an aggregator operates on packed bits (e.g. min/max/count), then copy its output definition from source
        if (std::is_same_v<Bit, B> && SystemConfiguration::getInstance().bitPackingEnabled()) {
            if ((agg.type == AggregateId::MIN || agg.type == AggregateId::MAX)){
                QueryFieldDesc packed_field(input_schema.getField(agg.ordinal), output_cursor);
                packed_field.setName("", agg.alias);

                this->output_schema_.putField(packed_field);
            }

            if (agg.type == AggregateId::COUNT) {
                QueryFieldDesc packed_field(output_cursor, agg.alias, "", FieldType::SECURE_LONG);
                packed_field.initializeFieldSizeWithCardinality(input->getTupleCount());
                this->output_schema_.putField(packed_field);
            }

        }

            aggregators_.push_back(agg_impl);
            ++output_cursor;
        }
    this->setOutputCardinality(input->getTupleCount());

    this->output_ = TableFactory<B>::getTable(input->getTupleCount(), Operator<B>::output_schema_, input->storageModel(), this->sort_definition_);
    QueryTable<B> *output = this->output_; // shorthand
    // SMA: if all dummies at the end, this would be simpler.  But we can't do that if there are MPC joins, filters, etc before this op because they will sprinkle dummies throughout the table
    for(int j = 0; j < group_by_.size(); ++j) {
        output->assignField(0, j, input, 0, group_by_[j]); //memcpy the input values
    }

    int cursor = group_by_.size();


    // for first row only
    for(GroupByAggregateImpl<B> *aggregator : aggregators_) {
        aggregator->initialize(input); // don't need group_by_match, only for first pass
        output->setPackedField(0, cursor, aggregator->getResult());
        ++cursor;
    }

    output->setDummyTag(0, input->getDummyTag(0));
    B true_lit = true;
    B matched, input_dummy_tag;


    for(int i = 1; i < input->getTupleCount(); ++i) {
        matched = true;
        input_dummy_tag = input->getDummyTag(i);
        for(int j = 0; j < group_by_.size(); ++j) {
            matched = matched & (input->getPackedField(i, group_by_[j]) == output->getPackedField(i-1, j));
            // initialize output - if input is dummy, copy from predecessor, otherwise copy from input
            Field<B> dst_group_by = Field<B>::If(input_dummy_tag, output->getPackedField(i-1, j), input->getPackedField(i, group_by_[j]));
            output->setPackedField(i, j, dst_group_by);
        }

        //  if uninitialized (seen no non-dummies yet), don't create a new group-by bin
        //  if input a dummy also leave group-by bin boundaries as-is
        matched = matched |  output->getDummyTag(i-1) | input_dummy_tag;

        cursor = group_by_.size();

        for(auto agg : aggregators_) {
            agg->accumulate(input, i, matched);
            output->setPackedField(i, cursor, agg->getResult());
            ++cursor;
        }
        B out_dummy_tag = output->getDummyTag(i-1) & input_dummy_tag; // both need to be dummies for current cell to remain dummy
        output->setDummyTag(i, out_dummy_tag);
        // for output[i-1]
        // if matched, replace previous with current, set old dummy tag to true
        out_dummy_tag = FieldUtilities::select(matched, true_lit, output->getDummyTag(i-1));
        output->setDummyTag(i-1, out_dummy_tag);
    }



    return this->output_;

}

template<typename B>
GroupByAggregateImpl<B> *GroupByAggregate<B>::aggregateFactory(const AggregateId &aggregator_type, const int32_t &ordinal,
                                                               const QueryFieldDesc &input_schema) const {

    // for most aggs the output type is the same as the input type
    // for COUNT(*) and others with an ordinal of < 0, then we set it to an INTEGER instead
    FieldType input_type = (ordinal >= 0) ?
                             input_schema.getType() :
                             (std::is_same_v<B, emp::Bit> ? FieldType::SECURE_LONG : FieldType::LONG);

    switch (aggregator_type) {
        case AggregateId::COUNT:
            return new GroupByCountImpl<B>(ordinal, input_type, this->getChild(0)->getOutput()->getTupleCount());
        case AggregateId::SUM:
            return new GroupBySumImpl<B>(ordinal, input_type);
        case AggregateId::AVG:
            return new GroupByAvgImpl<B>(ordinal, input_type);
        case AggregateId::MIN:
            return new GroupByMinImpl<B>(ordinal, input_schema);
        case AggregateId::MAX:
            return new GroupByMaxImpl<B>(ordinal, input_schema);
        default:
            throw;
    };
}


template<typename B>
QuerySchema GroupByAggregate<B>::generateOutputSchema(const QuerySchema & input_schema) const {
    QuerySchema output_schema;
    size_t i;

    for(i = 0; i < group_by_.size(); ++i) {
        QueryFieldDesc src_field = input_schema.getField(group_by_[i]);
        QueryFieldDesc dst_field(i, src_field.getName(), src_field.getTableName(), src_field.getType());
        dst_field.setStringLength(src_field.getStringLength());
        output_schema.putField(dst_field);
    }


    for(i = 0; i < aggregate_definitions_.size(); ++i) {
        ScalarAggregateDefinition agg = aggregate_definitions_[i];
        FieldType agg_type = (agg.ordinal >= 0) ?
                             input_schema.getField(agg.ordinal).getType() :
                             (std::is_same_v<B, emp::Bit> ? FieldType::SECURE_LONG : FieldType::LONG);

        QueryFieldDesc field_desc(i + group_by_.size(), aggregate_definitions_[i].alias, "", agg_type);
        output_schema.putField(field_desc);
    }

    output_schema.initializeFieldOffsets();
    return output_schema;

}


template<typename B>
bool GroupByAggregate<B>::sortCompatible(const SortDefinition & sorted_on, const vector<int32_t> &group_by_idxs) {
    // TODO: incorporate effective_sort for functional dependencies
    // then delete check_sort flag - this will be covered with effective_sort

    if(sorted_on.size() < group_by_idxs.size())
        return false;

    for(size_t idx = 0; idx < group_by_idxs.size(); ++idx) {
        // ASC || DESC does not matter here
        if(group_by_idxs[idx] != sorted_on[idx].first) {
            return false;
        }
    }

    return true;
}

template<typename B>
bool GroupByAggregate<B>::sortCompatible(const SortDefinition & sorted_on,
                                         const vector<int32_t> &group_by_idxs,
                                         const map<int32_t, std::set<int32_t>> &dependencies) {
    std::set<int32_t> coveredColumns;
    for(const auto& sort_def : sorted_on) {
        coveredColumns.insert(sort_def.first);
        if(dependencies.count(sort_def.first) > 0) {
            coveredColumns.insert(dependencies.at(sort_def.first).begin(), dependencies.at(sort_def.first).end());
        }
    }

    for(const auto& group_by_idx : group_by_idxs) {
        if(coveredColumns.count(group_by_idx) == 0) {
            return false;
        }
    }

    return true;
}


template<typename B>
void GroupByAggregate<B>::setup() {
    QuerySchema input_schema = Operator<B>::getChild(0)->getOutputSchema();
    SortDefinition input_sort = Operator<B>::getChild(0)->getSortOrder();

    Operator<B>::output_schema_ = generateOutputSchema(input_schema);

    // sorted on group-by cols
    if(check_sort_)
        if(effective_sort_.size() > 0)
            assert(sortCompatible(effective_sort_, group_by_));
        else
            assert(sortCompatible(input_sort, group_by_));

    this->setOutputCardinality(this->getChild()->getOutputCardinality());
    updateCollation();
}


template class vaultdb::GroupByAggregate<bool>;
template class vaultdb::GroupByAggregate<emp::Bit>;