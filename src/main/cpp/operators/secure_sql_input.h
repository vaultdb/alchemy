//
// Created by Jennie Rogers on 8/15/20.
//

#ifndef SECURE_SQL_INPUT_H
#define SECURE_SQL_INPUT_H


#include "sql_input.h"


// reads SQL input and secret shares it
// alice and bob need to run at the same time for this
class SecureSqlInput  : public SqlInput {

    // depends on EmpManager being configured in the calling method
    std::shared_ptr<QueryTable> runSelf() override;


public:
    SecureSqlInput(std::string db, std::string sql, bool dummyTag) : SqlInput(db, sql, dummyTag) {}
    ~SecureSqlInput() {};
};


#endif // ENCRYPTED_SQL_INPUT_H
