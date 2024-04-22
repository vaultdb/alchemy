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

        bool bp_enabled_ = false;
        BufferPoolManager *bpm_ = nullptr;

        int num_tables_;


        static SystemConfiguration& getInstance() {
            static SystemConfiguration  instance;
            return instance;
        }

        void initialize(const string &db_name, const std::map<ColumnReference, BitPackingDefinition> &bp,
                        const StorageModel &model) {
            unioned_db_name_ = db_name;
            bit_packing_ = bp;
            storage_model_ = model;

            // can only benefit from buffer pool in outsourced setting
            // mirroring the logic in QueryTable<B>::getTable()
            // this prevents the system from reaching an invalid state
            // if changing QueryTable, also change this
            if(storage_model_ != StorageModel::PACKED_COLUMN_STORE) {
                bp_enabled_ = false;
            }

            if(bp_enabled_) {
                bpm_ = new BufferPoolManager(256, 100, 5, 1000, emp_manager_);
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
            if(bpm_ != nullptr) delete bpm_;
        }



    private:
        SystemConfiguration() { }

        string unioned_db_name_, empty_db_name_; // empty DB used for schema lookups (for public info)

        StorageModel storage_model_ = StorageModel::COLUMN_STORE; // only support one storage model at a time

        std::map<ColumnReference, BitPackingDefinition> bit_packing_;
    };

}
#endif //_SYSTEM_CONFIGURATION_H
