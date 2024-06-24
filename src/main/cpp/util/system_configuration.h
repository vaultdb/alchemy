#ifndef _SYSTEM_CONFIGURATION_H
#define _SYSTEM_CONFIGURATION_H
#include <string>
#include <map>
#include "common/defs.h"
#include <iostream>
#include <util/emp_manager/emp_manager.h>
#include "util/buffer_pool/buffer_pool_manager.h"

using namespace std;

// warning: do not include emp-based classes like netio here - it conflicts with the static, singleton setup
// creates mysterious compile-time bugs

namespace vaultdb{
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


        string getUnionedDbName() const { return unioned_db_name_; }
        string getEmptyDbName() const { return empty_db_name_; }
        void setEmptyDbName(const string & db_name) { empty_db_name_ = db_name; }
        
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
