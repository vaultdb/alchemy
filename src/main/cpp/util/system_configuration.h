#ifndef _SYSTEM_CONFIGURATION_H
#define _SYSTEM_CONFIGURATION_H
#include <string>
#include <map>
#include "common/defs.h"
#include <iostream>
#include <util/emp_manager/emp_manager.h>
// #include "util/buffer_pool/buffer_pool_manager.h"
#include <query_table/query_schema.h>


using namespace std;

// warning: do not include emp-based classes like netio here - it conflicts with the static, singleton setup
// creates mysterious compile-time bugs

namespace vaultdb{
    class BufferPoolManager;
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

        string toString() const {
            std::stringstream s;
            s << name_ << ": " << schema_.prettyPrint() << ", cardinality: " << tuple_cnt_ << ", collation: ";
            // copy and paste of DataUtilities::printSortDefinition to avoid dependency
            s << "{";
            bool init = false;
            for(ColumnSort c : collation_) {
                if(init)
                    s << ", ";
                string direction = (c.second == SortDirection::ASCENDING) ? "ASC" : (c.second == SortDirection::DESCENDING) ? "DESC" : "INVALID";
                s << "<" << c.first << ", "
                       << direction << "> ";

                init = true;
            }

            s << "}";
            return s.str();

        }

    } TableMetadata;


    class SystemConfiguration {

    public:

        EmpManager *emp_manager_ = nullptr;
        int party_;
        int input_party_ = 10086;

        bool buffer_pool_enabled_ = true;
        BufferPoolManager & bpm_;
        int table_cnt_ = 0;
        int operator_cnt_ = 0;
        int bp_page_size_bytes_ = 8192; // formerly 2048 bits
        int bp_page_cnt_ = 122070; // about 1 GB of memory, 122070 pages of 8192 bytes each
        // int bp_page_cnt_ = 260000;; // about 2 GB of memory, 122070 pages of 8192 bytes each
        // int bp_page_cnt_ = 5242880;; // about 40 GB of memory, for r6i.4xl testing


        string temp_db_path_;
        string stored_db_path_;
        map<string, TableMetadata> table_metadata_;


        static SystemConfiguration& getInstance() {
            static SystemConfiguration  instance;
            return instance;
        }

        // for sh2pc and ZK mostly
        void initialize(const string &db_name, const std::map<ColumnReference, BitPackingDefinition> &bp, const StorageModel &model);

        void initialize(const string &db_name, const std::map<ColumnReference, BitPackingDefinition> &bp,
                        const StorageModel &model, const int & bp_page_size_bytes, const int & bp_page_cnt);

        void initializeWirePacking(const string & db_path, const std::string & temp_data_path);
        void initializeSecretShares(const string & db_path, const std::string & temp_data_path);

        string getUnionedDbName() const { return unioned_db_name_; }
        void setUnionedDbName(const string & db_name) { unioned_db_name_ = db_name; }
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

        bool bitPackingEnabled() const {
            return !bit_packing_.empty();
        }

        void setBitPackingMetadata(const std::map<ColumnReference, BitPackingDefinition> &bp) {
            bit_packing_ = bp;
        }

    private:
        SystemConfiguration();

        string unioned_db_name_, empty_db_name_; // empty DB used for schema lookups (for public info)
        StorageModel storage_model_ = StorageModel::COLUMN_STORE; // only support one storage model at a time
        std::map<ColumnReference, BitPackingDefinition> bit_packing_;
    };

}
#endif //_SYSTEM_CONFIGURATION_H
