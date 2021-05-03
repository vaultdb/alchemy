#ifndef SECURE_SQL_INPUT_H
#define SECURE_SQL_INPUT_H


#include "sql_input.h"


// reads SQL input and secret shares it
// alice and bob need to run at the same time for this
namespace  vaultdb {
    class SecureSqlInput : public Operator<emp::Bit> {


        NetIO *netio_;
        int srcParty;

        std::string inputQuery;
        std::string dbName;
        bool hasDummyTag;


    protected:
        std::shared_ptr<SecureTable> runSelf() override;


    public:
        SecureSqlInput(std::string db, std::string sql, bool dummyTag, emp::NetIO *netio, int aSrcParty) :     netio_(netio), srcParty(aSrcParty),
            inputQuery(sql), dbName(db), hasDummyTag(dummyTag) {}

        SecureSqlInput(const string &db, const string & sql, const bool &dummyTag, const SortDefinition &sortDefinition, NetIO *netio, const int &party) :
                        Operator(sortDefinition), netio_(netio), srcParty(party),  inputQuery(sql), dbName(db), hasDummyTag(dummyTag) {}
         ~SecureSqlInput() = default;

    };

}

#endif // ENCRYPTED_SQL_INPUT_H
