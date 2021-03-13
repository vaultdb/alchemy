#ifndef SECURE_SQL_INPUT_H
#define SECURE_SQL_INPUT_H


#include "sql_input.h"


// reads SQL input and secret shares it
// alice and bob need to run at the same time for this
namespace  vaultdb {
    class SecureSqlInput : public SqlInput {


        NetIO *netio_;
        int srcParty;


    protected:
        std::shared_ptr<QueryTable> runSelf() override;


    public:
        SecureSqlInput(std::string db, std::string sql, bool dummyTag, emp::NetIO *netio, int aSrcParty) : SqlInput(db, sql, dummyTag),
                                                                                                           netio_(netio), srcParty(aSrcParty) {}

        SecureSqlInput(const string &dbName, const string & sql, const bool &hasDummyTag, const SortDefinition &sortedOn, NetIO *netio, const int &party) : SqlInput(dbName, sql, hasDummyTag, sortedOn),
                        netio_(netio), srcParty(party) {}
         ~SecureSqlInput() = default;
    };

}

#endif // ENCRYPTED_SQL_INPUT_H
