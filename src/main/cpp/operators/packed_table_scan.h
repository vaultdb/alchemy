#ifndef _PACKED_TABLE_SCAN_
#define _PACKED_TABLE_SCAN_

#if  __has_include("emp-sh2pc/emp-sh2pc.h") || __has_include("emp-zk/zk.h")
#include "operator.h"
#include "util/table_manager.h"
// class for pulling tables directly from TableManager
// for use in pilot for reading in secret-shared data that has no DB equivalent

namespace vaultdb {
    class PackedTableScan : public Operator<Bit> {
    public:
        // if no limit set, return whole table
        PackedTableScan(const string &db, const string & table_name, const string &data_path, const int & input_party, const int & limit = -1) : Operator<Bit>(SortDefinition()), table_name_(table_name), limit_(limit) {

            this->setSortOrder(this->output_->order_by_);
            this->output_schema_ = this->output_->getSchema();
            this->output_cardinality_ = this->output_->tuple_cnt_;



        }

        PackedTableScan<Bit> *runSelf() override {
            this->start_time_ = clock_start();
            this->start_gate_cnt_ = this->system_conf_.andGateCount();

            // update output card based on current table size
            this->output_cardinality_ = this->output_->tuple_cnt_;

            return this->output_;

        }

        ~PackedTableScan() = default;

        Operator<Bit> *clone() const override {
            return new PackedTableScan(*this);
        }

        void updateCollation() override {}

        bool operator==(const Operator<Bit> &other) const override {
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
        int input_party_ = 0;
        int limit_ = -1;
    };
}

#else
#include "operator.h"
#include "util/table_manager.h"
#include "data/secret_shared_tpch_data/secret_share_and_pack_tpch_data_from_query.h"
// class for pulling tables directly from TableManager
// for use in pilot for reading in secret-shared data that has no DB equivalent

namespace vaultdb {
    class PackedTableScan : public Operator<Bit> {
    public:
        // if no limit set, return whole table
        PackedTableScan(const string &db, const string & table_name, const string &data_path, const int & input_party, const int & limit = -1) : Operator<Bit>(SortDefinition()), table_name_(table_name), limit_(limit) {
            this->db_name_ = db;
            this->data_path_ = data_path;
            this->input_party_ = input_party;
        }

        QueryTable<Bit> *runSelf() override {
            this->start_time_ = clock_start();
            this->start_gate_cnt_ = this->system_conf_.andGateCount();

            if(system_conf_.emp_mode_ == EmpMode::OUTSOURCED) {
                SecretShareAndPackTpchDataFromQuery ssp(this->db_name_, "", this->table_name_);
                ((OMPCBackend<N> *) emp::backend)->multi_pack_delta = ssp.load_backend_parameters(this->data_path_, this->input_party_);

                string table_path = this->data_path_ + this->table_name_ + "_" + this->db_name_ + "/";

                this->start_time_ = clock_start();
                this->start_gate_cnt_ = system_conf_.andGateCount();
                this->output_ = ssp.load_table_from_disk(table_path, this->input_party_);
            }
            else {
                throw std::runtime_error("Load data from disk only supports outsourced mode.");
            }

            // update output card based on current table size
            if(limit_ > -1) this->output_->resize(limit_);

            this->setSortOrder(this->output_->order_by_);
            this->output_schema_ = this->output_->getSchema();
            this->output_cardinality_ = this->output_->tuple_cnt_;

            return this->output_;

        }

        ~PackedTableScan() = default;

        Operator<Bit> *clone() const override {
            return new PackedTableScan(*this);
        }

        void updateCollation() override {}

        bool operator==(const Operator<Bit> &other) const override {
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
        int input_party_ = 0;
        int limit_ = -1;
    };
}

#endif


#endif
