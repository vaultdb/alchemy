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

        string getOperatorType() const override;

        string getParameters() const override;


    public:
        ZkSqlInput(string db, string sql, bool dummy_tag, const size_t &input_tuple_cnt = 0); // truncate tuples with last term
        ZkSqlInput(const string &db, const string & sql, const bool &dummy_tag, const SortDefinition &sortDefinition, const size_t & input_tuple_cnt = 0);
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
