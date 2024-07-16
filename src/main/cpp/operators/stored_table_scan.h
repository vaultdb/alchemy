#ifndef _STORED_TABLE_SCAN_
#define _STORED_TABLE_SCAN_

#include "operator.h"
#include "util/table_manager.h"
// class for pulling tables directly from TableManager
// for use in pilot for reading in secret-shared data that has no DB equivalent

namespace vaultdb {
    template<typename B>
    class StoredTableScan : public Operator<B> {
    public:
        // if no limit set, return whole table
        StoredTableScan(const string & table_name, const int & limit = -1) : Operator<B>(SortDefinition()), table_name_(table_name), limit_(limit) {

            updateTable();

            this->setSortOrder(this->output_->order_by_);
            this->output_schema_ = this->output_->getSchema();
            this->output_cardinality_ = this->output_->tuple_cnt_;



        }

        StoredTableScan(const string & table_name, const vector<int> & ordinals, const int & limit = -1) {


        }


        QueryTable<B> *runSelf() override {
            this->start_time_ = clock_start();
            this->start_gate_cnt_ = this->system_conf_.andGateCount();

            updateTable();

            // update output card based on current table size
            this->output_cardinality_ = this->output_->tuple_cnt_;

            return this->output_;

        }

        ~TableScan() = default;

        Operator<B> *clone() const override {
            return new TableScan<B>(*this);
        }

        void updateCollation() override {}

        bool operator==(const Operator<B> &other) const override {
            if (other.getType() != OperatorType::TABLE_SCAN) {
                return false;
            }

            auto other_node = dynamic_cast<const TableScan &>(other);

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
            return OperatorType::TABLE_SCAN;
        }


    private:
        std::string table_name_;
        int limit_ = -1;

        void updateTable() {
            QueryTable<B> *table;
            if(std::is_same_v<B, bool>) {
                // casting to address compile-time syntax errors
                table = (QueryTable<B> *) TableManager::getInstance().getPlainTable(table_name_);
            }
            else {
                table = (QueryTable<B> *) TableManager::getInstance().getSecureTable(table_name_);

            }
            if(table == nullptr) {
                throw std::runtime_error("Table " + table_name_ + " not found in TableManager!");
            }

            this->output_ = table->clone();
            if(limit_ > -1) this->output_->resize(limit_);

        }

    };
}


#endif // STORED_TABLE_SCAN
