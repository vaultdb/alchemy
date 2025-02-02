#ifndef _SQL_INPUT_H
#define _SQL_INPUT_H


#include <util/data_utilities.h>
#include <data/psql_data_provider.h>
#include "operators/operator.h"
#include <util/system_configuration.h>
#include <vector>
#include <string>

// reads SQL input and stores in a plaintext array
namespace  vaultdb {

class SqlInput : public Operator<bool> {


public:
    // bool denotes whether the last col of the SQL statement should be interpreted as a dummy tag
    SqlInput(std::string db, std::string sql, bool dummy_tag);
    SqlInput(std::string db, std::string sql, bool dummy_tag, const SortDefinition &sort_def,
             const size_t &tuple_limit = 0);

    SqlInput(const SqlInput & src) : Operator<bool>(src), input_query_(src.input_query_), original_input_query_(src.original_input_query_), db_name_(src.db_name_), original_collation_(src.original_collation_), dummy_tagged_(src.dummy_tagged_), tuple_limit_(src.tuple_limit_) {}

    PlainOperator *clone() const override {
        return new SqlInput(*this);
    }

    void updateCollation() override {
        auto collation = this->sort_definition_;
        // take current sql and wrap it in ORDER BY
        if(this->sort_definition_.empty())  {
            this->output_->order_by_ = this->sort_definition_; // if sort is empty it does not really matter how it is stored now
            this->input_query_ = original_input_query_;
            return;
        }

         if(this->sort_definition_ == original_collation_) {
             input_query_ = original_input_query_;
             return;
         }

        // update collation
        string sql = "SELECT * FROM (" + original_input_query_ + ") to_sort ORDER BY ";
            sql += "(" + std::to_string(collation[0].first + 1) + ") "
                   + ((collation[0].second == SortDirection::DESCENDING) ? " DESC " : " ASC ");
            for (int i = 1; i < collation.size(); ++i) {
                sql += ", (" + std::to_string(collation[i].first + 1) + ") "
                       + ((collation[i].second == SortDirection::DESCENDING) ? " DESC " : " ASC ");
            }
            this->input_query_ = sql;
            // signal we need to refresh for updated SQL query
            if(output_ != nullptr && output_->order_by_ != this->sort_definition_)  {
                delete output_;
                output_ = nullptr;
            }

    }

    virtual ~SqlInput() {
    }

    bool operator==(const Operator<bool> & rhs) const override {

        if(rhs.getType() != OperatorType::SQL_INPUT) return false;

        auto other_node = dynamic_cast<const SqlInput &>(rhs);
        if(this->input_query_ != other_node.input_query_) return false;
        if(this->dummy_tagged_ != other_node.dummy_tagged_) return false;
        if(this->input_query_ != other_node.input_query_) return false;
        if(this->tuple_limit_ != other_node.tuple_limit_) return false;
        if(this->original_input_query_ != other_node.original_input_query_) return false;
        if(this->original_collation_ != other_node.original_collation_) return false;
        if(this->db_name_ != other_node.db_name_) return false;

        return this->operatorEquality(rhs);
    }

    void truncateInput(const size_t & limit); // to test on smaller datasets
    std::string input_query_, original_input_query_;
    std::string db_name_;
    SortDefinition  original_collation_;
    bool dummy_tagged_;
    size_t tuple_limit_;


private:
    void runQuery();

protected:
    PlainTable *runSelf() override;
    string getParameters() const override {
        string str = input_query_;
        std::replace(str.begin(), str.end(), '\n', ' ');
        return "\"" + str + "\", tuple_count=" + std::to_string(this->getOutputCardinality());
     }
    OperatorType getType() const override { return OperatorType::SQL_INPUT; }

};

}   // namespace vaultdb

#endif // _SQL_INPUT_H
