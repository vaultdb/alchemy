//
// Created by Jennie Rogers on 8/15/20.
//

#ifndef SECURE_SQL_INPUT_H
#define SECURE_SQL_INPUT_H


#include "sql_input.h"


// reads SQL input and secret shares it
// alice and bob need to run at the same time for this
class SecureSqlInput  : public SqlInput {


    NetIO *netio_;
    int dstParty;
    // depends on EmpManager being configured in the calling method
    std::shared_ptr<QueryTable> runSelf() override;


public:
    SecureSqlInput(std::string db, std::string sql, bool dummyTag, emp::NetIO *netio, int aDstParty) : SqlInput(db, sql, dummyTag), dstParty(aDstParty) {
        netio_ = netio;
    }

    ~SecureSqlInput() {};
};


#endif // ENCRYPTED_SQL_INPUT_H
