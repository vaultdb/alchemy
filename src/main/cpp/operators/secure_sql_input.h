#ifndef SECURE_SQL_INPUT_H
#define SECURE_SQL_INPUT_H


#include "sql_input.h"


// reads SQL input and secret shares it
// alice and bob need to run at the same time for this
namespace  vaultdb {
    class SecureSqlInput : public Operator<emp::Bit> {


        NetIO *netio_;
        int src_party_;

        string input_query_;
        string db_name_;
        bool has_dummy_tag_;
        StorageModel storage_model_;


    protected:
        SecureTable *runSelf() override;

        string getOperatorType() const override;

        string getParameters() const override;


    public:
        SecureSqlInput(string db, string sql, bool dummy_tag, const StorageModel & model, emp::NetIO *netio, int party, const size_t & input_tuple_cnt = 0); // truncate tuples with last term

        SecureSqlInput(const string &db, const string & sql, const bool &dummy_tag, const StorageModel & model, const SortDefinition &sort_def, NetIO *netio, const int &party, const size_t & input_tuple_cnt = 0);
         ~SecureSqlInput() {
             if(plain_input_ != nullptr) {
                 delete plain_input_;
             }
         }


    private:
        void runQuery();
        PlainTable *plain_input_ = nullptr;
        size_t input_tuple_limit_;
    };

}

#endif // ENCRYPTED_SQL_INPUT_H
