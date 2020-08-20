//
// Created by Jennie Rogers on 8/18/20.
//

#ifndef _SORT_CONDITION_H
#define _SORT_CONDITION_H

#include  <querytable/query_tuple.h>

using namespace  vaultdb;

class SortCondition {

    virtual void compareAndSwap(QueryTuple lhs, QueryTuple rhs) = 0;

};


#endif //VAULTDB_EMP_SORT_CONDITION_H
