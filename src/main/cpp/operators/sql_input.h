//
// Created by Jennie Rogers on 8/15/20.
//

#ifndef _SQL_INPUT_H
#define _SQL_INPUT_H


#include "operator.h"

// reads SQL input and stores in a plaintext array
class SqlInput  : public Operator {

protected:
    std::string inputQuery;
    std::string dbName;
    bool hasDummyTag;

public:
    // bool denotes whether the last col of the SQL statement should be interpreted as a dummy tag
    SqlInput(std::string db, std::string sql, bool dummyTag) : inputQuery(sql), dbName(db), hasDummyTag(dummyTag)  {}

    std::shared_ptr<QueryTable> runSelf() override;

};


#endif // _SQL_INPUT_H
