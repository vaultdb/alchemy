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


    protected:
        shared_ptr<SecureTable> runSelf() override;


    public:
        SecureSqlInput(string db, string sql, bool dummyTag, emp::NetIO *netio, int aSrcParty);

        SecureSqlInput(const string &db, const string & sql, const bool &dummyTag, const SortDefinition &sortDefinition, NetIO *netio, const int &party);
         ~SecureSqlInput() = default;

    private:
        void runQuery();
        shared_ptr<PlainTable> plain_input_;
    };

}

#endif // ENCRYPTED_SQL_INPUT_H
