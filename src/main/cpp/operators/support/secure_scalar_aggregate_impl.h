#ifndef _SECURE_SCALAR_AGGREGATE_IMPL_H
#define _SECURE_SCALAR_AGGREGATE_IMPL_H

#include "scalar_aggregate_impl.h"

namespace vaultdb {
    class SecureScalarAverage : public ScalarAggregateImpl {
     public:
         SecureScalarAverage(const uint32_t & ordinal) :  ScalarAggregateImpl(ordinal) {};
         void initialize(const QueryTuple & tuple) override; // needs to run this once with first tuple to set up state
         void accumulate(const QueryTuple & tuple) override;
         types::Value getResult() override;
         types::TypeId getType() override;

     private:
         types::Value runningSum;
         types::Value runningCount;
         types::Value zero;
         types::Value one;

     };

}

// TODO: implement sum/count/min/max

#endif //_SECURE_SCALAR_AGGREGATE_IMPL_H
