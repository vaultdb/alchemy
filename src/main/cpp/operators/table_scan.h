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
        explicit TableScan(const string & table_name) : Operator<B>(SortDefinition()) {
            table_name_ = table_name;
            if(TableManager::getInstance().getTable<B>(table_name) != nullptr) {
                auto table = TableManager::getInstance().getTable<B>(table_name);
                auto collation = table->order_by_;
                this->setSortOrder(collation);
            }
        }

        QueryTable<B> *runSelf() override {
            return TableManager::getInstance().getTable<B>(table_name_);
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

    protected:

        string getParameters() const override {
            return "table_name=" + table_name_;
        }

        OperatorType getType() const override {
            return OperatorType::TABLE_SCAN;
        }

    private:
        std::string table_name_;

    };
}


#endif
