//
// Created by Jennie Rogers on 8/15/20.
//

#ifndef ENCRYPTED_SQL_INPUT_H
#define ENCRYPTED_SQL_INPUT_H


#include "sql_input.h"


// reads SQL input and secret shares it
// alice and bob need to run at the same time for this
class EncryptedSqlInput  : public SqlInput {
    EncryptedSqlInput(std::string db, std::string sql, bool dummyTag) : SqlInput(db, sql, dummyTag) {}

    // depends on EmpManager being configured in the calling method
    std::shared_ptr<QueryTable> runSelf() override;

};


#endif // ENCRYPTED_SQL_INPUT_H
