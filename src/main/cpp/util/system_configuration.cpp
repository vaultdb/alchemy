#include "system_configuration.h"
#include "data_utilities.h"
#include "parsing_utilities.h"

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



void SystemConfiguration::initializeWirePackedDb(const std::string &db_path) {
    {
        stored_db_path_ = db_path;
        assert(storageModel() == StorageModel::PACKED_COLUMN_STORE);

        block delta;
        // if input party, initialize delta first from file
        if (inputParty()) {
            auto d = DataUtilities::readFile(stored_db_path_ + "/delta");
            assert(d.size() == sizeof(block));
            memcpy((int8_t *) &delta, d.data(), sizeof(block));
        }

        emp_manager_->setDelta(delta);

        table_metadata_ = ParsingUtilities::parseTableMetadata(db_path);

    }
}


void SystemConfiguration::initializeOutsourcedSecretShareDb(const string & db_path) {
    stored_db_path_ = db_path;
    assert(storageModel() == StorageModel::COLUMN_STORE);
    table_metadata_ = ParsingUtilities::parseTableMetadata(db_path);

}


