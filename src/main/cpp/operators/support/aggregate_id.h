#ifndef _AGGREGATE_ID_H
#define _AGGREGATE_ID_H

#include <string>

namespace vaultdb {

    enum class AggregateId {
      COUNT,
      SUM,
      AVG,
      MIN,
      MAX
    };


    struct ScalarAggregateDefinition {
        int ordinal; // input ordinal/operand
        AggregateId type;
        std::string alias;
        bool is_distinct = false; // TODO: implement DISTINCT aggregates

        ScalarAggregateDefinition(int anOrdinal, AggregateId aggregateId, std::string anAlias)
                : ordinal(anOrdinal), type(aggregateId), alias(anAlias) {}

        ScalarAggregateDefinition() {}
    };




}
#endif // _AGGREGATE_ID_H
