#ifndef _PACKED_TABLE_SCAN_
#define _PACKED_TABLE_SCAN_

#if  __has_include("emp-sh2pc/emp-sh2pc.h") || __has_include("emp-zk/zk.h")
#include "operator.h"
#include "util/table_manager.h"
// class for pulling tables directly from TableManager
// for use in pilot for reading in secret-shared data that has no DB equivalent

namespace vaultdb {
    template<typename B>
    class PackedTableScan : public Operator<B> {
    public:
        // if no limit set, return whole table
        PackedTableScan<B>(const string &db, const string & table_name, const string &data_path, const int & input_party, const int & limit = -1) : Operator<B>(SortDefinition()), table_name_(table_name), limit_(limit) {

            this->setSortOrder(this->output_->order_by_);
            this->output_schema_ = this->output_->getSchema();
            this->output_cardinality_ = this->output_->tuple_cnt_;



        }

        QueryTable<B> *runSelf() override {
            this->start_time_ = clock_start();
            this->start_gate_cnt_ = this->system_conf_.andGateCount();

            // update output card based on current table size
            this->output_cardinality_ = this->output_->tuple_cnt_;

            return this->output_;

        }

        ~PackedTableScan() = default;

        Operator<B> *clone() const override {
            return new PackedTableScan(*this);
        }

        void updateCollation() override {}

        bool operator==(const Operator<B> &other) const override {
            return false;
        }


        string getTableName() const {
            return table_name_;
        }

        int getLimit() const {
            return limit_;
        }

    protected:

        string getParameters() const override {
            return "table_name=" + table_name_;
        }

        OperatorType getType() const override {
            return OperatorType::PACKED_TABLE_SCAN;
        }


    private:
        std::string db_name_;
        std::string table_name_;
        string data_path_ = "";
        int input_party_ = 0;
        int limit_ = -1;
    };
}

#else
#include "operator.h"
#include "util/table_manager.h"
#include "data/secret_shared_data/secret_share_and_pack_data_from_query.h"
#include "query_table/column_table.h"
#include "query_table/packed_column_table.h"
// class for pulling tables directly from TableManager
// for use in pilot for reading in secret-shared data that has no DB equivalent

namespace vaultdb {
    template<typename B>
    class PackedTableScan : public Operator<B> {
    public:
        // if no limit set, return whole table
        PackedTableScan<B>(const string &db, const string & table_name, const string &data_path, const int & input_party, const int & limit = -1, const SortDefinition & sort_def = SortDefinition()) : Operator<B>(sort_def), table_name_(table_name), limit_(limit) {
            this->db_name_ = db;
            this->data_path_ = data_path;
            this->input_party_ = input_party;

            this->ssp_ = new SecretShareAndPackDataFromQuery(this->db_name_, "", this->table_name_);
            this->table_path_ = this->data_path_ + this->table_name_ + "_" + this->db_name_ + "/";
            this->output_schema_ = is_same_v<B, Bit> ? this->ssp_->load_schema_from_disk(table_path_) : QuerySchema();
        }

        QueryTable<B> *runSelf() override {
            this->start_time_ = clock_start();
            this->start_gate_cnt_ = this->system_conf_.andGateCount();

            if(SystemConfiguration::getInstance().emp_mode_ == EmpMode::OUTSOURCED) {
                ((OMPCBackend<N> *) emp::backend)->multi_pack_delta = this->ssp_->load_backend_parameters(this->data_path_, this->input_party_);

                this->start_time_ = clock_start();
                this->start_gate_cnt_ = this->system_conf_.andGateCount();
                this->output_ = is_same_v<B, Bit> ? (QueryTable<B> *) this->ssp_->load_table_from_disk(this->table_path_, this->input_party_) : (QueryTable<B> *) new ColumnTable<B>(0, this->output_schema_, this->sort_definition_);
            }
            else {
                throw std::runtime_error("Load data from disk only supports outsourced mode.");
            }

            // update output card based on current table size
            if(this->limit_ > -1) this->output_->resize(this->limit_);

            this->output_schema_ = this->output_->getSchema();
            this->output_cardinality_ = this->output_->tuple_cnt_;

            return this->output_;

        }

        ~PackedTableScan() = default;

        Operator<B> *clone() const override {
            return new PackedTableScan(*this);
        }

        void updateCollation() override {}

        bool operator==(const Operator<B> &other) const override {
            if (other.getType() != OperatorType::PACKED_TABLE_SCAN) {
                return false;
            }

            auto other_node = dynamic_cast<const PackedTableScan &>(other);

            if(this->table_name_ != other_node.table_name_) return false;
            return this->operatorEquality(other);
        }


        string getTableName() const {
            return table_name_;
        }

        int getLimit() const {
            return limit_;
        }

    protected:

        string getParameters() const override {
            return "table_name=" + table_name_;
        }

        OperatorType getType() const override {
            return OperatorType::PACKED_TABLE_SCAN;
        }


    private:
        std::string db_name_;
        std::string table_name_;
        string data_path_ = "";
        string table_path_ = "";
        int input_party_ = 0;
        int limit_ = -1;

        SecretShareAndPackDataFromQuery *ssp_;
    };
}

#endif


#endif
