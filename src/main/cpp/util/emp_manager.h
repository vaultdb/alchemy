//
// Created by Jennie Rogers on 7/25/20.
//

#ifndef EMP_MANAGER_H
#define EMP_MANAGER_H


#include <vaultdb.h>
#include <emp-sh2pc/emp-sh2pc.h>

// singleton for managing emp state, e.g., netio
class EmpManager {
    static EmpManager *instance_;
    emp::NetIO *netio_;
    int empParty_;

    // Private constructor so that no objects can be created.
    EmpManager() {
        netio_ = nullptr;
        instance_ = nullptr;
    }

public:
    static EmpManager *getInstance() {
        if (!instance_)
            instance_ = new EmpManager();  // null initially, only valid once we call configure below
        return instance_;
    }

    void configureEmpManager(const char *aliceHost, int port, int empParty);




    std::shared_ptr<QueryTable> secretShareTable(const QueryTable *srcTable);
    QueryTuple secretShareTuple(QueryTuple *srcTuple,  const QuerySchema *schema, int party);
    QueryField secretShareField(const QueryField *srcField, int ordinal, types::TypeId type, size_t length, int party);
    types::Value secretShareValue(const types::Value &srcValue, types::TypeId type, size_t length, int party);

    std::string toString() const {
        return std::string("emp manager: ") +   std::to_string((uint64_t) this) +   std::string(" with netio: ") + std::to_string((uint64_t) netio_);
    }


    void flush();

    ~EmpManager();

    void close();

    emp::Integer encryptVarchar(std::string input, size_t stringBitCount, int party);
};


#endif //_EMP_UTILITIES_H
