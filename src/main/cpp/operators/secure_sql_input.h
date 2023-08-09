#ifndef SECURE_SQL_INPUT_H
#define SECURE_SQL_INPUT_H


#include "sql_input.h"


// reads SQL input and secret shares it
// alice and bob need to run at the same time for this
namespace  vaultdb {
    class SecureSqlInput : public Operator<emp::Bit> {

        string input_query_;
        string db_name_;
        bool has_dummy_tag_;
        StorageModel storage_model_;
        int input_party_ = 0;


    protected:
        SecureTable *runSelf() override;

        string getOperatorTypeString() const override {     return "SecureSqlInput";  }
        OperatorType getOperatorType() const override {     return OperatorType::SECURE_SQL_INPUT;  }
        string getParameters() const override {
            // N.B., this is the tuple count provided by each party
            // after secret sharing we expect more than this via the other party
            return "\"" + input_query_ + "\", tuple_count=" + std::to_string(plain_input_->getTupleCount());
         }


    public:
        SecureSqlInput(const string & db, const string & sql, const bool & dummy_tag, const SortDefinition & def, const size_t & input_tuple_cnt = 0); // truncate tuples with last term
        SecureSqlInput(const string & db, const string & sql, const bool & dummy_tag, const int & input_party, const size_t & input_tuple_cnt, const SortDefinition & def);

        SecureSqlInput(const string &db, const string & sql, const bool &dummy_tag, const size_t & input_tuple_cnt = 0);
        SecureOperator *clone() const override {
            return new SecureSqlInput(this->db_name_, this->input_query_, this->has_dummy_tag_, this->input_party_,
                                      this->input_tuple_limit_, this->sort_definition_);
        }

         ~SecureSqlInput() {
             if(plain_input_ != nullptr) {
                 delete plain_input_;
             }
         }


    private:
        void runQuery();
        void runQuery(const int & input_party);
        PlainTable *plain_input_ = nullptr;
        size_t input_tuple_limit_;
    };

}

#endif 
