#include "system_configuration.h"

using namespace vaultdb;

BitPackingDefinition SystemConfiguration::getBitPackingSpec(const string &table_name, const string &col_name) {


    ColumnReference c(table_name, col_name);
    try {
        if (bit_packing_.find(c) != bit_packing_.end()) {
            return bit_packing_.at(c);
        }
    } catch(const std::out_of_range& e) {}
    // also try matching on the column name alone, in case table name is unavailable
    bool found = false;
    BitPackingDefinition def;
    for(auto & [name, md] : bit_packing_) {
        if(name.second == col_name) {
            if(found) {// second match implies ambiguity, return empty set
                return BitPackingDefinition();
            }
           def = md;
           found = true;
        }
    }
    return def;
}


