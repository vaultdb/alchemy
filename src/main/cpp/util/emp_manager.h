//
// Created by Jennie Rogers on 7/25/20.
//

#ifndef EMP_MANAGER_H
#define EMP_MANAGER_H


#include <vaultdb.h>
#include <emp-sh2pc/emp-sh2pc.h>

// singleton for managing emp state, e.g., netio
class EmpManager {
    static EmpManager *instance;
    emp::NetIO *netio_;
    EmpParty party_;

    // Private constructor so that no objects can be created.
    EmpManager() {
        netio_ = nullptr;
    }

    ~EmpManager() {
        delete netio_;
    }

public:
    static EmpManager *getInstance() {
        if (!instance)
            instance = new EmpManager();
        return instance;
    }

    void configureEmpManager(const char *aliceHost, int port, EmpParty party) {
        netio_ =  new emp::NetIO(aliceHost, port);
        party_ = party;
       emp::setup_semi_honest(netio_, (int) party_);
    }



    QueryTable *secretShareTable(QueryTable *srcTable);
    void encryptField(QueryTable *src, QueryTable *dst, int srcTupleIdx, int dstTupleIdx, int fieldIdx, int party);

};


#endif //_EMP_UTILITIES_H
