#include "system_configuration.h"


vaultdb::BitPackingDefinition
vaultdb::SystemConfiguration::getBitPackingSpec(const string &table_name, const string &col_name) {

    std::cout << "Querying: bit packing metadata has " << bit_packing_.size() << " entries." << std::endl;
    std::cout << "System config at " << (int64_t) this << std::endl;

    ColumnReference c(table_name, col_name);
    try {
        if (bit_packing_.find(c) != bit_packing_.end()) {
            std::cout << "Key found! |" <<  table_name << "|, |" << col_name << "|" << std::endl;
            return bit_packing_.at(c);
        }
        else
            std::cout << "**Key  not found! |" <<  table_name << "|, |" << col_name << "|" <<  std::endl;
    } catch(const std::out_of_range& e) {}
    return BitPackingDefinition();
}
