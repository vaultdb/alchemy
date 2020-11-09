#ifndef _SECURE_REPLACE_TUPLE_H
#define _SECURE_REPLACE_TUPLE_H


#include "replace_tuple.h"

class SecureReplaceTuple : public ReplaceTuple{
public:
    explicit SecureReplaceTuple(std::shared_ptr<QueryTable> table);

    // override
    void conditionalWrite(const uint32_t &writeIdx, const QueryTuple &inputTuple, const types::Value &toWrite) override;

};


#endif //VAULTDB_EMP_SECURE_REPLACE_TUPLE_H
