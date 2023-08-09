#ifndef _SCALAR_AGGREGATE_H
#define _SCALAR_AGGREGATE_H

#include <expression/generic_expression.h>
#include <operators/support/aggregate_id.h>
#include <operators/support/scalar_aggregate_impl.h>
#include "operator.h"


namespace vaultdb {
    template<typename B>
    class ScalarAggregate : public Operator<B> {
    public:
        std::vector<ScalarAggregateDefinition> aggregate_definitions_;
        std::vector<ScalarAggregateImpl<B> *> aggregators_;


        // aggregates are sorted by their output order in aggregate's output schema
        ScalarAggregate(Operator<B> *child, const std::vector<ScalarAggregateDefinition> &aggregates, const SortDefinition & sort = SortDefinition());

        ScalarAggregate(QueryTable<B> *child, const std::vector<ScalarAggregateDefinition> &aggregates, const SortDefinition & sort = SortDefinition());
        Operator<B> *clone() const override {
            return new ScalarAggregate<B>(this->lhs_child_->clone(), this->aggregate_definitions_, this->sort_definition_);
        }

        virtual ~ScalarAggregate() {
            for(auto agg : aggregators_) {
                delete agg;
            }
        }

    protected:

        QueryTable<B> *runSelf() override;
        string getOperatorTypeString() const override {     return "ScalarAggregate"; }
        OperatorType getOperatorType() const override {     return OperatorType::SCALAR_AGGREGATE; }
        string getParameters() const override {
            stringstream ss;
            ss << "aggs: (" << aggregate_definitions_[0].toString();

            for(uint32_t i = 1; i < aggregate_definitions_.size(); ++i) {
                ss << ", " << aggregate_definitions_[i].toString();
            }

            ss << ")";
            return ss.str();
        }

    private:
        void setup();
    };

}
#endif //_SCALAR_AGGREGATE_H
