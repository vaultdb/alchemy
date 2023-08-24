#ifndef _NESTED_LOOP_AGGREGATE_H
#define _NESTED_LOOP_AGGREGATE_H


#include <expression/generic_expression.h>
#include <operators/support/aggregate_id.h>
#include <operators/support/unsorted_aggregate_impl.h>
#include "operator.h"
#include "common/defs.h"

namespace vaultdb {
    template<typename B>
    class NestedLoopAggregate : public Operator<B> {

    public:
        SortDefinition effective_sort_;
        std::vector<ScalarAggregateDefinition> aggregate_definitions_;
        std::vector<int32_t> group_by_;

        vector<UnsortedAggregateImpl<B> *> aggregators_;

        NestedLoopAggregate(Operator<B> *child, const vector<int32_t> &groupBys,
                            const vector<ScalarAggregateDefinition> &aggregates, const SortDefinition & sort, const int & output_card = 0);
        NestedLoopAggregate(Operator<B> *child, const vector<int32_t> &groupBys,
                            const vector<ScalarAggregateDefinition> &aggregates, const int output_card = 0);
        NestedLoopAggregate(Operator<B> *child, const vector<int32_t> &groupBys,
                            const vector<ScalarAggregateDefinition> &aggregates, const int output_card, const SortDefinition & effective_sort);

        NestedLoopAggregate(QueryTable<B> *child, const vector<int32_t> &groupBys,
                            const vector<ScalarAggregateDefinition> &aggregates, const SortDefinition & sort, const int & output_card = 0);
        NestedLoopAggregate(QueryTable<B>  *child, const vector<int32_t> &groupBys,
                            const vector<ScalarAggregateDefinition> &aggregates, const int & output_card = 0);
        NestedLoopAggregate(const NestedLoopAggregate & src) : Operator<B>(src), aggregate_definitions_(src.aggregate_definitions_),
                group_by_(src.group_by_), effective_sort_(src.effective_sort_){
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

            for(int i = 0; i < group_by_.size(); ++i) {
                if(child_sort.size() <= (i+1)) break; // nothing to match
                if(group_by_[i] == child_sort[i].first) {
                    output_sort.push_back(child_sort[i]);
                } else {
                    break;
                }
            }
            this->sort_definition_ = output_sort;
       }

    protected:
        QueryTable<B> *runSelf() override;
        string getParameters() const override {
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
        enum OperatorType getType() const override { return OperatorType::NESTED_LOOP_AGGREGATE; }

    private:


        // returns boolean for whether two tuples are in the same group-by bin
        B groupByMatch(const QueryTable<B> *src, const int & src_row, const QueryTable<B> *dst, const int & dst_row)  const;

        QuerySchema generateOutputSchema(const QuerySchema & srcSchema) const;

        void setup();



    };
}

#endif //_NESTED_LOOP_AGGREGATE_H