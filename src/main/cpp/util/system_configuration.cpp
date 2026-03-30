#include "system_configuration.h"
#include "data_utilities.h"
#include "parsing_utilities.h"
#include "util/buffer_pool/buffer_pool_manager.h"

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

SystemConfiguration::SystemConfiguration()
  : bpm_(BufferPoolManager::getInstance()) {}

void SystemConfiguration::initialize(const string &db_name, const std::map<ColumnReference, BitPackingDefinition> &bp,
    const StorageModel &model) {
    unioned_db_name_ = db_name;
    bit_packing_ = bp;
    storage_model_ = model;
    table_cnt_ = 0; // reset table count
    operator_cnt_ = 0;

    if (temp_db_path_.empty()) {
        temp_db_path_ = Utilities::getCurrentWorkingDirectory() + "/temp";
    }

    // reset temp table contents
    Utilities::mkdir(temp_db_path_);
    // Utilities::runCommand("rm " + temp_db_path_ + "/*");


    if(buffer_pool_enabled_) {
        bpm_.initialize(bp_page_size_bytes_, bp_page_cnt_);
    }
}

void SystemConfiguration::initialize(const string &db_name, const std::map<ColumnReference, BitPackingDefinition> &bp,
                                     const StorageModel &model, const int &bp_page_size_bytes, const int &bp_page_cnt) {
    unioned_db_name_ = db_name;
    bit_packing_ = bp;
    storage_model_ = model;
    bp_page_size_bytes_ = bp_page_size_bytes;
    bp_page_cnt_ = bp_page_cnt;
    table_cnt_ = 0; // reset table count
    operator_cnt_ = 0;

    if (temp_db_path_.empty()) {
        temp_db_path_ = Utilities::getCurrentWorkingDirectory() + "/temp";
    }

    // clear out temp dir
    Utilities::mkdir(temp_db_path_);
    // Utilities::runCommand("rm " + temp_db_path_ + "/*");

    if(buffer_pool_enabled_) {
        bpm_.initialize(bp_page_size_bytes_, bp_page_cnt_);
    }

}

void SystemConfiguration::initializeWirePacking(const std::string &db_path, const std::string & temp_data_path) {
    {
        stored_db_path_ = db_path;
        temp_db_path_ = temp_data_path;
        Utilities::mkdir(temp_db_path_);

        assert(storageModel() == StorageModel::PACKED_COLUMN_STORE);
        table_metadata_ = ParsingUtilities::parseTableMetadata(db_path);

    }
}


void SystemConfiguration::initializeSecretShares(const string & db_path, const std::string & temp_data_path) {
    stored_db_path_ = db_path;
    temp_db_path_ = temp_data_path;
    Utilities::mkdir(temp_db_path_);

    assert(storageModel() == StorageModel::COLUMN_STORE);
    table_metadata_ = ParsingUtilities::parseTableMetadata(db_path);

}


