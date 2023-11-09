#ifndef _SORT_MERGE_AGGREGATE_H_
#define _SORT_MERGE_AGGREGATE_H_

#include "group_by_aggregate.h"
#include <operators/support/group_by_aggregate_impl.h>

namespace vaultdb {
    template<typename B>

    class SortMergeAggregate : public GroupByAggregate<B> {

        vector<GroupByAggregateImpl<B> *> aggregators_;

    public:

        SortMergeAggregate(Operator<B> *child, const vector<int32_t> &group_bys, const vector<ScalarAggregateDefinition> &aggregates, const SortDefinition & effective_sort = SortDefinition(), const int & cardinality_bound = -1) :  GroupByAggregate<B>(child, group_bys, aggregates, effective_sort, cardinality_bound) {
            setup();
        }

        SortMergeAggregate(QueryTable<B> *child, const vector<int32_t> &group_bys, const vector<ScalarAggregateDefinition> &aggregates, const SortDefinition & effective_sort = SortDefinition(), const int & cardinality_bound = -1) :  GroupByAggregate<B>(child, group_bys, aggregates, effective_sort, cardinality_bound) {
            setup();
        }

        SortMergeAggregate(const SortMergeAggregate<B> & src) : GroupByAggregate<B>(src) {
            is_optimized_cloned_ = src.is_optimized_cloned_;
            setup();
        }

        Operator<B> *clone() const override {
            return new SortMergeAggregate<B>(*this);
        }

        void updateCollation() override {
            this->getChild(0)->updateCollation();

            SortDefinition  child_sort = this->getChild(0)->getSortOrder();
            assert(this->sortCompatible(child_sort));

            if(!this->effective_sort_.empty()) {
                this->sort_definition_ = this->effective_sort_;
                return;
            }

            SortDefinition  output_sort_def;
            // map sort order to that of child
            for(size_t idx = 0; idx < this->group_by_.size(); ++idx) {
                // projecting the attribute in group_by_[idx] to the ith position in output
                output_sort_def.emplace_back(idx, child_sort[this->group_by_[idx]].second);
            }

            this->sort_definition_ = output_sort_def;
        }

        virtual ~SortMergeAggregate()  {
            for(size_t i = 0; i < aggregators_.size(); ++i) {
                delete aggregators_[i];
            }
        }

        bool is_optimized_cloned_ = false;

    protected:
        QueryTable<B> *runSelf() override;


        inline OperatorType getType() const override {
            return OperatorType::SORT_MERGE_AGGREGATE;
        }


    private:
        void setup();
        GroupByAggregateImpl<B> *aggregateFactory(const AggregateId &aggregator_type, const int32_t &ordinal,
                                                  const QueryFieldDesc &input_schema) const {
            // for most aggs the output type is the same as the input type
            // for COUNT(*) and others with an ordinal of < 0, then we set it to an INTEGER instead
            FieldType input_type = (ordinal >= 0) ?
                                   input_schema.getType() :
                                   (std::is_same_v<B, emp::Bit> ? FieldType::SECURE_LONG : FieldType::LONG);

            switch (aggregator_type) {
                case AggregateId::COUNT:
                    return new GroupByCountImpl<B>(ordinal, input_type, this->getChild(0)->getOutput()->tuple_cnt_);
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
            }
        }


    };
}

#endif
