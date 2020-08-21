//
// Created by Jennie Rogers on 8/18/20.
//

#ifndef _SORT_CONDITION_H
#define _SORT_CONDITION_H

#include  <querytable/query_tuple.h>
#include "defs.h"

using namespace  vaultdb;

class SortCondition {

protected:
    SortDefinition sortDefinition;
    types::Value getValue(QueryTuple & aTuple, const ColumnSort & aColumnSort);

public:
    SortCondition() {}; // placeholder
    SortCondition(const SortDefinition & aSortDefinition) :  sortDefinition(aSortDefinition) {};

    virtual void compareAndSwap(QueryTuple  & lhs, QueryTuple & rhs) = 0;

};


#endif //VAULTDB_EMP_SORT_CONDITION_H
