#ifndef _GROUP_BY_AGGREGATE_H
#define _GROUP_BY_AGGREGATE_H

#include <expression/generic_expression.h>
#include <operators/support/aggregate_id.h>
#include <operators/support/group_by_aggregate_impl.h>
#include "operator.h"

namespace vaultdb {
    template<typename B>
    class GroupByAggregate : public Operator<B> {

        vector<GroupByAggregateImpl<B> *> aggregators_;

    public:
        std::vector<ScalarAggregateDefinition> aggregate_definitions_;
        std::vector<int32_t> group_by_;
        bool check_sort_ = true;
        // truncated output not yet implemented.  Placeholder member variable below
        size_t output_cardinality_ = 0;

        GroupByAggregate(Operator<B> *child, const vector<int32_t> &group_bys,
                         const vector<ScalarAggregateDefinition> &aggregates, const SortDefinition & sort);
        GroupByAggregate(Operator<B> *child, const vector<int32_t> &group_bys,
			       const vector<ScalarAggregateDefinition> &aggregates);

        GroupByAggregate(Operator<B> *child, const vector<int32_t> &group_bys,
                         const vector<ScalarAggregateDefinition> &aggregates, const bool &check_sort);
        GroupByAggregate(Operator<B> *child, const vector<int32_t> &group_bys,
                         const vector<ScalarAggregateDefinition> &aggregates, const bool &check_sort, const int &json_cardinality);

        GroupByAggregate(QueryTable<B> *child, const vector<int32_t> &group_bys,
                         const vector<ScalarAggregateDefinition> &aggregates, const SortDefinition & sort);

        GroupByAggregate(QueryTable<B> *child, const vector<int32_t> &group_bys,
                         const vector<ScalarAggregateDefinition> &aggregates);

        GroupByAggregate(const GroupByAggregate<B> & src) : Operator<B>(src), aggregate_definitions_(src.aggregate_definitions_),
                check_sort_(src.check_sort_), group_by_(src.group_by_), json_cardinality_(src.json_cardinality_) {
            setup();
        }

        Operator<B> *clone() const override {
            return new GroupByAggregate<B>(*this);
        }

        virtual ~GroupByAggregate()  {
            for(size_t i = 0; i < aggregators_.size(); ++i) {
                delete aggregators_[i];
            }
        }
        static bool sortCompatible(const SortDefinition & lhs, const vector<int32_t> &group_by_idxs);
        void setJsonOutputCardinality(size_t cardinality) { json_cardinality_ = cardinality; }
        size_t getJsonOutputCardinality() const { return json_cardinality_; }



    protected:
        size_t json_cardinality_ = 0;
        QueryTable<B> *runSelf() override;


        inline OperatorType getType() const override {
            return OperatorType::SORT_MERGE_AGGREGATE;
        }

        inline string getParameters() const override {
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


    private:
        GroupByAggregateImpl<B> *aggregateFactory(const AggregateId &aggregator_type, const int32_t &ordinal,
                                                  const QueryFieldDesc &input_schema) const;


        QuerySchema generateOutputSchema(const QuerySchema & srcSchema) const;


        void setup();


    };
}

#endif //_GROUP_BY_AGGREGATE_H
