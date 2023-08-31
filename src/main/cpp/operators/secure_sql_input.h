#ifndef SECURE_SQL_INPUT_H
#define SECURE_SQL_INPUT_H


#include "sql_input.h"
#include <regex>

// reads SQL input and secret shares it
// alice and bob need to run at the same time for this
namespace  vaultdb {
    class SecureSqlInput : public Operator<emp::Bit> {

        StorageModel storage_model_;
        string original_input_query_; // no modifications for limit or sort
        SortDefinition original_collation_;

    protected:
        SecureTable *runSelf() override;

        OperatorType getType() const override {     return OperatorType::SECURE_SQL_INPUT;  }
        string getParameters() const override {
            // N.B., this is the tuple count provided by each party
            // after secret sharing we expect more than this via the other party
            return "\"" + input_query_ + "\", tuple_count=" + std::to_string(this->getOutputCardinality());
         }



    public:
        string input_query_;
        string db_name_;
        bool has_dummy_tag_;
        int input_party_ = 0;
        size_t input_tuple_limit_;
        PlainTable *plain_input_ = nullptr;


        SecureSqlInput(const string & db, const string & sql, const bool & dummy_tag, const SortDefinition & def, const size_t & input_tuple_cnt = 0); // truncate tuples with last term
        SecureSqlInput(const string & db, const string & sql, const bool & dummy_tag, const int & input_party, const size_t & input_tuple_cnt, const SortDefinition & def);

        SecureSqlInput(const string &db, const string & sql, const bool &dummy_tag, const size_t & input_tuple_cnt = 0);
        SecureSqlInput(const SecureSqlInput & src) : Operator<Bit>(src), input_query_(src.input_query_), db_name_(src.db_name_), has_dummy_tag_(src.has_dummy_tag_), input_party_(src.input_party_), input_tuple_limit_(src.input_tuple_limit_), original_input_query_(src.original_input_query_), original_collation_(src.original_collation_) {
            if(src.plain_input_ != nullptr)  {
                plain_input_ = src.plain_input_->clone();
            }
        }

        SecureOperator *clone() const override {
            return new SecureSqlInput(*this);
        }

        string getInputQuery() const { return input_query_; }
        void setInputQuery(const string & sql) { input_query_ = sql; }
        int getInputParty() const { return input_party_; }

        void updateCollation() override {
            auto collation = this->sort_definition_;
            // take current sql and wrap it in ORDER BY
            if(this->sort_definition_.empty())  {
                plain_input_->setSortOrder(this->sort_definition_); // if sort is empty it does not really matter how it is stored now
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
          if(plain_input_ != nullptr && plain_input_->getSortOrder() != this->sort_definition_) {
            delete plain_input_;
             plain_input_ = nullptr;
          }

        }

         ~SecureSqlInput() {
             if(plain_input_ != nullptr) {
                 delete plain_input_;
             }
         }


    private:
        void runQuery();
    };

}

#endif 
