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
        virtual ~ScalarAggregate() {
            for(auto agg : aggregators_) {
                delete agg;
            }
        }

    protected:

        QueryTable<B> *runSelf() override;
        string getOperatorType() const override;
        string getParameters() const override;

    private:
        void setup();
    };

}
#endif //_SCALAR_AGGREGATE_H
