#ifndef SECURE_SQL_INPUT_H
#define SECURE_SQL_INPUT_H


#include "sql_input.h"


// reads SQL input and secret shares it
// alice and bob need to run at the same time for this
class SecureSqlInput  : public SqlInput {


    NetIO *netio_;
    int srcParty;
    // depends on EmpManager being configured in the calling method
    std::shared_ptr<QueryTable> runSelf() override;


public:
    SecureSqlInput(std::string db, std::string sql, bool dummyTag, emp::NetIO *netio, int aSrcParty) : SqlInput(db, sql, dummyTag), srcParty(aSrcParty) {
        netio_ = netio;
    }

    ~SecureSqlInput() {};
};


#endif // ENCRYPTED_SQL_INPUT_H
