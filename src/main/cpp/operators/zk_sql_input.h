#ifndef ZK_SQL_INPUT_H
#define ZK_SQL_INPUT_H


#include "sql_input.h"
#include <emp-zk/emp-zk.h>


// reads SQL input and secret shares it
// alice and bob need to run at the same time for this
namespace  vaultdb {
    class ZkSqlInput : public Operator<emp::Bit> {


        int src_party_;

        string input_query_;
        string db_name_;
        bool has_dummy_tag_;

    protected:
        SecureTable *runSelf() override;

        string getOperatorTypeString() const override { return "ZkSqlInput";  }
        OperatorType getOperatorType() const override { return OperatorType::ZK_SQL_INPUT; }
        string getParameters() const override {
            return "\"" + input_query_ + "\", tuple_count=" + std::to_string(plain_input_->getTupleCount());
        }


    public:
        ZkSqlInput(string db, string sql, bool dummy_tag, const size_t &input_tuple_cnt = 0); // truncate tuples with last term
        ZkSqlInput(const string &db, const string & sql, const bool &dummy_tag, const SortDefinition &sortDefinition, const size_t & input_tuple_cnt = 0);
        SecureOperator *clone() const override {
            return new ZkSqlInput(this->db_name_, this->input_query_, this->has_dummy_tag_, this->sort_definition_,
                                  this->input_tuple_limit_);
        }
         ~ZkSqlInput() {
             if(plain_input_) {
                 delete plain_input_;
                 plain_input_ = nullptr;
             }
         };


    private:
        void runQuery();
        PlainTable *plain_input_;
        size_t input_tuple_limit_;
    };

}

#endif // ENCRYPTED_SQL_INPUT_H
