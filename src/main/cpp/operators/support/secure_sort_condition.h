//
// Created by Jennie Rogers on 8/20/20.
//

#ifndef _SECURE_SORT_CONDITION_H
#define _SECURE_SORT_CONDITION_H


#include "sort_condition.h"

class SecureSortCondition  : public SortCondition {

public:
    void compareAndSwap(QueryTuple  & lhs, QueryTuple & rhs) override;


};


#endif //VAULTDB_EMP_SECURE_SORT_CONDITION_H
