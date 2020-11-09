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
        int ordinal; // input ordinal
        AggregateId type;
        std::string alias;

        ScalarAggregateDefinition(int anOrdinal, AggregateId aggregateId, std::string anAlias)
                : ordinal(anOrdinal), type(aggregateId), alias(anAlias) {}
    };
}
#endif // _AGGREGATE_ID_H
