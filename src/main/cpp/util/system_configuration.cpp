#include "system_configuration.h"


vaultdb::BitPackingDefinition
vaultdb::SystemConfiguration::getBitPackingSpec(const string &table_name, const string &col_name) {


    ColumnReference c(table_name, col_name);
    try {
        if (bit_packing_.find(c) != bit_packing_.end()) {
            return bit_packing_.at(c);
        }
    } catch(const std::out_of_range& e) {}
    return BitPackingDefinition();
}
