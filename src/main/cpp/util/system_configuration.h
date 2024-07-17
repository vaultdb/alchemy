#ifndef _SYSTEM_CONFIGURATION_H
#define _SYSTEM_CONFIGURATION_H
#include <string>
#include <map>
#include "common/defs.h"
#include <iostream>
#include <util/emp_manager/emp_manager.h>
#include "util/buffer_pool/buffer_pool_manager.h"
#include <query_table/query_schema.h>

using namespace std;

// warning: do not include emp-based classes like netio here - it conflicts with the static, singleton setup
// creates mysterious compile-time bugs

namespace vaultdb{

    typedef struct table_metadata_ {
        string name_;
        QuerySchema schema_;
        SortDefinition collation_;
        size_t tuple_cnt_;

        bool operator==(const table_metadata_ &other) const {
            // need to do vectorEquality here without Utilities because of circular dependency
            if(collation_.size() != other.collation_.size()) return false;
            for(int i = 0; i < collation_.size(); ++i) if(collation_[i] != other.collation_[i]) return false;
            return name_ == other.name_ && schema_ == other.schema_  && tuple_cnt_ == other.tuple_cnt_;
        }

    } TableMetadata;


    class SystemConfiguration {

    public:

        EmpManager *emp_manager_ = nullptr;
        EmpMode emp_mode_ = EmpMode::PLAIN;
        int party_;
        int input_party_ = 10086;

        bool bp_enabled_ = false;
        BufferPoolManager & bpm_ = BufferPoolManager::getInstance();
        int num_tables_ = 0;
        int bp_page_size_bits_ = 2048;
        int bp_page_cnt_ = 50;
        string stored_db_path_;
        map<string, TableMetadata> table_metadata_;


        static SystemConfiguration& getInstance() {
            static SystemConfiguration  instance;
            return instance;
        }

        // for sh2pc and ZK mostly
        void initialize(const string &db_name, const std::map<ColumnReference, BitPackingDefinition> &bp, const StorageModel &model) {
            unioned_db_name_ = db_name;
            bit_packing_ = bp;
            storage_model_ = model;

            if(storage_model_ == StorageModel::PACKED_COLUMN_STORE) {
                bp_enabled_ = true;
                bpm_.initialize(bp_page_size_bits_, bp_page_cnt_, emp_manager_);
            }
        }

        void initialize(const string &db_name, const std::map<ColumnReference, BitPackingDefinition> &bp,
                        const StorageModel &model, const int & bp_page_size_bits, const int & bp_page_cnt) {
            unioned_db_name_ = db_name;
            bit_packing_ = bp;
            storage_model_ = model;
            bp_page_size_bits_ = bp_page_size_bits;
            bp_page_cnt_ = bp_page_cnt;

            if(storage_model_ == StorageModel::PACKED_COLUMN_STORE) {
                bp_enabled_ = true;
                bpm_.initialize(bp_page_size_bits_, bp_page_cnt_, emp_manager_);
            }
        }

        void initializeWirePackedDb(const string & db_path);

        string getUnionedDbName() const { return unioned_db_name_; }
        string getEmptyDbName() const { return empty_db_name_; }
        void setEmptyDbName(const string & db_name) { empty_db_name_ = db_name; }
        inline bool inputParty() { return party_ == input_party_; }

        BitPackingDefinition getBitPackingSpec(const string & table_name, const string & col_name);
        SystemConfiguration(const SystemConfiguration&) = delete;
        SystemConfiguration& operator=(const SystemConfiguration &) = delete;

        inline void clearBitPacking() {
            bit_packing_.clear();
        }


        inline StorageModel storageModel() const {
            return storage_model_;
        }

        inline void setStorageModel(const StorageModel & model) {
            storage_model_ = model;
        }

        inline size_t andGateCount() const {
            return (emp_manager_ != nullptr)  ? emp_manager_->andGateCount() : 0L;
        }

        inline void flush() const {
            if(emp_manager_ != nullptr) emp_manager_->flush();
        }

        ~SystemConfiguration() {
            if(emp_manager_ != nullptr) delete emp_manager_;

        }



    private:
        SystemConfiguration() { }

        string unioned_db_name_, empty_db_name_; // empty DB used for schema lookups (for public info)

        StorageModel storage_model_ = StorageModel::COLUMN_STORE; // only support one storage model at a time

        std::map<ColumnReference, BitPackingDefinition> bit_packing_;
    };

}
#endif //_SYSTEM_CONFIGURATION_H
