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
            instance = new EmpManager();  // null initially, only valid once we call configure below
        return instance;
    }

    void configureEmpManager(const char *aliceHost, int port, EmpParty party) {
        std::cout << "Instantiating emp with host=" << aliceHost << " port=" << port << " party=" << (int) party << std::endl;
        netio_ =  new emp::NetIO(party == EmpParty::ALICE ? nullptr:aliceHost, port);
        party_ = party;
       emp::setup_semi_honest(netio_, (int) party_);
       std::cout << "Initialized netio!" << std::endl;
    }



    std::unique_ptr<QueryTable> secretShareTable(QueryTable *srcTable);
    void secretShareTuple(QueryTuple *srcTuple, QueryTuple *dstTuple, const QuerySchema *schema, int party); // TODO: refactor this in query table to regularize access patterns wrt Table/Field
    QueryField *secretShareField(const QueryField *srcField, int ordinal, types::TypeId type, int party);
    types::Value secretShareValue(const types::Value *srcValue, types::TypeId type, int party);

    void secretShareField(QueryTable *src, QueryTable *dst, int srcTupleIdx, int dstTupleIdx, int fieldIdx, int party);


    std::unique_ptr<QueryTable> revealTable(const QueryTable *srcTable, int party);
    void revealField(const QueryTable *src, QueryTable *dst, int tupleIdx, int fieldIdx, int party);



};


#endif //_EMP_UTILITIES_H
