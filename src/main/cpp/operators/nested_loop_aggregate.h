#ifndef _NESTED_LOOP_AGGREGATE_H
#define _NESTED_LOOP_AGGREGATE_H

#include "group_by_aggregate.h"
#include <operators/support/unsorted_aggregate_impl.h>

namespace vaultdb {
    template<typename B>
    class NestedLoopAggregate : public GroupByAggregate<B> {

    public:
        vector<UnsortedAggregateImpl<B> *> aggregators_;

        NestedLoopAggregate(Operator<B> *child, const vector<int32_t> &group_by,
                            const vector<ScalarAggregateDefinition> &aggregates, const SortDefinition & effective_sort = SortDefinition (), const int & output_card = 0) : GroupByAggregate<B>(child, group_by, aggregates, effective_sort, output_card) {
            setup();
        }

        NestedLoopAggregate(QueryTable<B> *child, const vector<int32_t> &group_by,
                            const vector<ScalarAggregateDefinition> &aggregates, const SortDefinition & effective_sort = SortDefinition (), const int & output_card = 0) : GroupByAggregate<B>(child, group_by, aggregates, effective_sort, output_card) {
            setup();
        }

        NestedLoopAggregate(const NestedLoopAggregate & src) : GroupByAggregate<B>(src) {
            setup();
        }

        ~NestedLoopAggregate(){ }


        Operator<B> *clone() const override {
            return new NestedLoopAggregate<B>(*this);
        }

        // if group-by cols are sorted, then they will be emitted in the order in which they were first visited in NLA
        void updateCollation() override {
            this->getChild(0)->updateCollation();
            auto child_sort = this->getChild(0)->getSortOrder();
            SortDefinition  output_sort;

            for(int i = 0; i < this->group_by_.size(); ++i) {
                if(child_sort.size() <= (i+1)) break; // nothing to match
                if(this->group_by_[i] == child_sort[i].first) {
                    output_sort.push_back(child_sort[i]);
                } else {
                    break;
                }
            }
            this->sort_definition_ = output_sort;
       }

    protected:
        enum OperatorType getType() const override { return OperatorType::NESTED_LOOP_AGGREGATE; }
        QueryTable<B> *runSelf() override;

    private:


        // returns boolean for whether two tuples are in the same group-by bin
        B groupByMatch(const QueryTable<B> *src, const int & src_row, const QueryTable<B> *dst, const int & dst_row)  const;


        void setup();



    };
}

#endif //_NESTED_LOOP_AGGREGATE_H