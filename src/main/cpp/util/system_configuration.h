#ifndef _SYSTEM_CONFIGURATION_H
#define _SYSTEM_CONFIGURATION_H
#include <string>
#include <map>
#include "common/defs.h"
#include <iostream>
#include <util/emp_manager/emp_manager.h>

using namespace std;

// warning: do not include emp-based classes like netio here - it conflicts with the static, singleton setup
// creates mysterious compile-time bugs

namespace vaultdb{
    class SystemConfiguration {

    public:

        EmpManager *emp_manager_ = nullptr;
        EmpMode emp_mode_ = EmpMode::PLAIN;
        int party_;

        // value to be maintained by EMPManager
        int emp_bit_size_bytes_ = sizeof(emp::Bit);

        static SystemConfiguration& getInstance() {
            static SystemConfiguration  instance;
            return instance;
        }

        void initialize(const string &db_name, const std::map<ColumnReference, BitPackingDefinition> &bp,
                        const StorageModel &model) {
            unioned_db_name_ = db_name;
            bit_packing_ = bp;
            bit_packing_enabled_ = true;
            storage_model_ = model;

        }

        string getUnionedDbName() const { return unioned_db_name_; }
        BitPackingDefinition getBitPackingSpec(const string & table_name, const string & col_name);
        SystemConfiguration(const SystemConfiguration&) = delete;
        SystemConfiguration& operator=(const SystemConfiguration &) = delete;

        inline void clearBitPacking() {
            bit_packing_.clear();
            bit_packing_enabled_ = false;
            packed_expressions_ = false;
        }

        inline bool bitPackingEnabled() const {
            return bit_packing_enabled_;
        }

        inline bool packedExpressions() const {
            return packed_expressions_;
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
        SystemConfiguration() {
        }
        string unioned_db_name_;
        StorageModel storage_model_ = StorageModel::ROW_STORE; // only support one storage model at a time
        std::map<ColumnReference, BitPackingDefinition> bit_packing_;
        bool bit_packing_enabled_ = false;
        bool packed_expressions_ = false;
    };

}
#endif //_SYSTEM_CONFIGURATION_H
