#ifndef _TABLE_SCAN_
#define _TABLE_SCAN_

#include "operator.h"
#include "util/table_manager.h"
// class for pulling tables directly from TableManager
// for use in pilot for reading in secret-shared data that has no DB equivalent

namespace vaultdb {
    template<typename B>
    class TableScan : public Operator<B> {
    public:
        // if no limit set, return whole table
        TableScan(const string & table_name, const int & limit = -1) : Operator<B>(SortDefinition()), table_name_(table_name), limit_(limit) {

            if(TableManager::getInstance().getTable<B>(table_name) == nullptr) {
                throw std::runtime_error("Table " + table_name + " not found in TableManager!");
            }
            auto table = TableManager::getInstance().getTable<B>(table_name);
            this->setSortOrder(table->order_by_);
            this->output_schema_ = table->getSchema();
            this->output_cardinality_ = (limit > -1) ? limit_ :  table->tuple_cnt_;

        }

        QueryTable<B> *runSelf() override {
            this->start_time_ = clock_start();
            this->start_gate_cnt_ = this->system_conf_.andGateCount();

            this->output_ = TableManager::getInstance().getTable<B>(table_name_)->clone();
            if(limit_ >= 0) {
                this->output_->resize(limit_);
            }
            else {
                // update output card based on current table size
                this->output_cardinality_ = this->output_->tuple_cnt_;
            }
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

    };
}


#endif
