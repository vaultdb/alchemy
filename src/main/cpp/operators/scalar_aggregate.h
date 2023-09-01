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
        ScalarAggregate(const ScalarAggregate & src) : Operator<B>(src), aggregate_definitions_(src.aggregate_definitions_) {
            setup();
        }

        Operator<B> *clone() const override {
            return new ScalarAggregate<B>(*this);
        }

        // no collation needed
        void updateCollation() override {}

        virtual ~ScalarAggregate() {
            for(auto agg : aggregators_) {
                delete agg;
            }
        }

        bool operator==(const Operator<B> & other) const override {
            if (other.getType() != OperatorType::SCALAR_AGGREGATE) {
                return false;
            }

            auto other_node = dynamic_cast<const ScalarAggregate<B> &>(other);
            if(this->aggregate_definitions_.size() != other_node.aggregate_definitions_.size()) return false;
            for(int i = 0; i < this->aggregate_definitions_.size(); ++i) {
                if(this->aggregate_definitions_[i] != other_node.aggregate_definitions_[i]) return false;
            }

            return this->operatorEquality(other);
        }

    protected:

        QueryTable<B> *runSelf() override;
        OperatorType getType() const override {     return OperatorType::SCALAR_AGGREGATE; }
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
