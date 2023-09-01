#ifndef _AGGREGATE_ID_H
#define _AGGREGATE_ID_H

#include <string>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include "query_table/query_schema.h"

using boost::property_tree::ptree;

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

        bool operator==(const ScalarAggregateDefinition & rhs) const {
            return ordinal == rhs.ordinal && type == rhs.type && alias == rhs.alias && is_distinct == rhs.is_distinct;
        }

        bool operator!=(const ScalarAggregateDefinition & rhs) const {
            return !(*this == rhs);
        }

        static std::string getAggregatorString(const AggregateId & id) {
            switch(id) {
                case AggregateId::COUNT:
                    return "COUNT";
                case AggregateId::SUM:
                    return "SUM";
                case AggregateId::AVG:
                    return "AVG";
                case AggregateId::MIN:
                    return "MIN";
                case AggregateId::MAX:
                    return "MAX";
                default:
                    return "";
            }
        }

        std::string toString() const {
            std::stringstream ss;

            switch (type) {
                case AggregateId::COUNT:
                    ss << "COUNT("
                       << ((ordinal == -1) ? "*" : "$" + std::to_string(ordinal))
                       << ") " << alias;
                    break;
                case AggregateId::SUM:
                    ss << "SUM("
                       << "$" + std::to_string(ordinal)
                       << ") " << alias;
                    break;

                case AggregateId::AVG:
                    ss << "AVG("
                       << "$" + std::to_string(ordinal)
                       << ") " << alias;
                    break;

                case AggregateId::MIN:
                    ss << "SUM("
                       << "$" + std::to_string(ordinal)
                       << ") " << alias;
                    break;

                case AggregateId::MAX:
                    ss << "SUM("
                       << "$" + std::to_string(ordinal)
                       << ") " << alias;
            };

            return ss.str();
        }

      static ptree  aggregateDefinitionToPTree(const ScalarAggregateDefinition &def, const QuerySchema &schema) {
            ptree agg_parent;
            ptree agg;
            agg.put("name", ScalarAggregateDefinition::getAggregatorString(def.type));
            agg.put("kind", ScalarAggregateDefinition::getAggregatorString(def.type));

            agg.put("syntax", "FUNCTION");

            agg_parent.add_child("agg", agg);

            ptree type;
            FieldType  f = schema.getField(def.ordinal).getType();
            type.put("type", TypeUtilities::getJSONTypeString(f));
            type.put("nullable", false);
            agg_parent.add_child("type", type);

            agg_parent.put("distinct", false);


            ptree ordinal, operands;
            ordinal.put_value(def.ordinal);
            operands.push_back(std::make_pair("", ordinal));

            agg_parent.add_child("operands", operands);
            agg_parent.put("name", def.alias);

            return agg_parent;
        }

   };




}
#endif // _AGGREGATE_ID_H
