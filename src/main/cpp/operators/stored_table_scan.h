#ifndef _STORED_TABLE_SCAN_
#define _STORED_TABLE_SCAN_

#include "operator.h"
#include "util/operator_utilities.h"

namespace vaultdb {
    template<typename B>
    class StoredTableScan : public Operator<B> {
    public:
        // if no limit set (+ no projection) return whole table
        StoredTableScan(const string & table_name, const int & limit = -1) : Operator<B>(SortDefinition()), limit_(limit) {
            setup(table_name);
        }

        StoredTableScan(const string & table_name, const string & col_names_csv, const int & limit = -1) : Operator<B>(SortDefinition()),  limit_(limit) {

            ordinals_ = OperatorUtilities::getOrdinalsFromColNames(SystemConfiguration::getInstance().table_metadata_.at(table_name).schema_, col_names_csv);
            setup(table_name);

        }

        StoredTableScan(const string & table_name, const vector<int> & ordinals, const int & limit = -1) : Operator<B>(SortDefinition()), limit_(limit), ordinals_(ordinals) {
            setup(table_name);

        }

        static QueryTable<B> *readStoredTable(string table_name, const vector<int> & col_ordinals, const int & limit = -1);

        QueryTable<B> *runSelf() override {
            this->start_time_ = clock_start();
            this->start_gate_cnt_ = this->system_conf_.andGateCount();

            this->output_ = StoredTableScan<B>::readStoredTable(md_.name_, ordinals_, limit_);

            // update output card based on current table size - just in case the one on disk is smaller
            this->output_cardinality_ = this->output_->tuple_cnt_;

            return this->output_;

        }

        StoredTableScan(const StoredTableScan<B> & src) : Operator<B>(src), limit_(src.limit_), md_(src.md_), ordinals_(src.ordinals_){ }

        ~StoredTableScan() = default;

        Operator<B> *clone() const override {
            return new StoredTableScan<B>(*this);
        }

        void updateCollation() override {}

        bool operator==(const Operator<B> &other) const override {
            if (other.getType() != OperatorType::STORED_TABLE_SCAN) {
                return false;
            }

            auto other_node = dynamic_cast<const StoredTableScan &>(other);

            if(Utilities::vectorEquality(ordinals_, other_node.ordinals_) && md_ == other_node.md_ && limit_ == other_node.limit_) {
                return this->operatorEquality(other);
            }

            return false;
        }


        string getTableName() const {
            return md_.name_;
        }

        int getLimit() const {
            return limit_;
        }

    protected:

        string getParameters() const override {
            return "table_name=" + md_.name_;
        }

        OperatorType getType() const override {
            return OperatorType::STORED_TABLE_SCAN;
        }


    private:
        int limit_ = -1;
        TableMetadata  md_;
        vector<int> ordinals_;


        void setup(const string & table_name) {
            // lazy table scan
            md_ = SystemConfiguration::getInstance().table_metadata_.at(table_name);
            if(ordinals_.empty()) {
                this->output_schema_ = md_.schema_;
                this->setSortOrder(md_.collation_);
            }
            else {
                this->output_schema_ = OperatorUtilities::deriveSchema(md_.schema_, ordinals_);
                this->setSortOrder(OperatorUtilities::deriveCollation(md_.collation_, ordinals_));
            }

            this->output_cardinality_ = (limit_ < md_.tuple_cnt_ && limit_ > -1) ? limit_ : md_.tuple_cnt_;

        }
    };
}


#endif // STORED_TABLE_SCAN
