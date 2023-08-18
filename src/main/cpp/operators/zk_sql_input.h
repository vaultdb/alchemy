#ifndef ZK_SQL_INPUT_H
#define ZK_SQL_INPUT_H


#include "sql_input.h"
#include <emp-zk/emp-zk.h>


// reads SQL input and secret shares it
// alice and bob need to run at the same time for this
namespace  vaultdb {
    class ZkSqlInput : public Operator<emp::Bit> {


    protected:
        SecureTable *runSelf() override;

        OperatorType getType() const override { return OperatorType::ZK_SQL_INPUT; }
        string getParameters() const override {
            return "\"" + input_query_ + "\", tuple_count=" + std::to_string(plain_input_->getTupleCount());
        }

        string input_query_;
        string db_name_;
        bool has_dummy_tag_;

        PlainTable *plain_input_;
        size_t input_tuple_limit_;


    public:
        ZkSqlInput(string db, string sql, bool dummy_tag, const size_t &input_tuple_cnt = 0); // truncate tuples with last term
        ZkSqlInput(const string &db, const string & sql, const bool &dummy_tag, const SortDefinition &sortDefinition, const size_t & input_tuple_cnt = 0);
        ZkSqlInput(const ZkSqlInput & src) : Operator<Bit>(src), input_query_(src.input_query_), db_name_(src.db_name_), has_dummy_tag_(src.has_dummy_tag_), input_tuple_limit_(src.input_tuple_limit_) {
            if(src.plain_input_) {
                plain_input_ = src.plain_input_->clone();
            }
        }

        SecureOperator *clone() const override {
            return new ZkSqlInput(*this);
        }

        void updateCollation() override {
            // no-op
        }

         ~ZkSqlInput() {
             if(plain_input_) {
                 delete plain_input_;
                 plain_input_ = nullptr;
             }
         }




    private:
        void runQuery();
    };

}

#endif // ZK_SQL_INPUT_H
