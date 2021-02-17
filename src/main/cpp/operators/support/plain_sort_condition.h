#ifndef _PLAIN_SORT_CONDITION_H
#define _PLAIN_SORT_CONDITION_H


#include "sort_condition.h"

class PlainSortCondition : public SortCondition {
public:

    PlainSortCondition(const SortDefinition & aSortDefinition) : SortCondition(aSortDefinition) {};
    ~PlainSortCondition() {}
    void compareAndSwap(QueryTuple  & lhs, QueryTuple & rhs) override;


};


#endif //VAULTDB_EMP_PLAIN_SORT_CONDITION_H
