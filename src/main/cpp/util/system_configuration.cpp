#include "system_configuration.h"
#include "data_utilities.h"
#include "utilities.h"

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
        if(inputParty()) {
            auto d = DataUtilities::readFile(stored_db_path_ + "/delta");
            assert(d.size() == sizeof(block));
            memcpy((int8_t *) &delta, d.data(), sizeof(block));
        }

        emp_manager_->setDelta(delta);

        string all_tables = Utilities::runCommand("ls *.metadata", stored_db_path_);
        vector<string> tables = Utilities::splitStringByNewline(all_tables);

        for(auto & metadata_file : tables) {
            TableMetadata md;
            md.name_ = metadata_file.substr(0, metadata_file.size() - 9);
            auto metadata = DataUtilities::readTextFile(stored_db_path_ + "/" + metadata_file);
            // drop ".metadata" suffix
            md.schema_ = QuerySchema(metadata.at(0));
            md.collation_ = DataUtilities::parseCollation(metadata.at(1));
            md.tuple_cnt_ = atoi(metadata.at(2).c_str());
            table_metadata_[md.name_] = md;
        }


    }


}