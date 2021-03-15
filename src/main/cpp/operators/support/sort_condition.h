#ifndef _SORT_CONDITION_H
#define _SORT_CONDITION_H

#include  <query_table/query_tuple.h>
#include "common/defs.h"
#include <query_table/field/field.h>

namespace vaultdb {
    template<typename T>
    class SortCondition {

    protected:
        SortDefinition sortDefinition;

        const Field *getValue(QueryTuple &aTuple, const ColumnSort &aColumnSort);

    public:
        SortCondition() {} // placeholder
        virtual ~SortCondition() {}

        SortCondition(const SortDefinition &aSortDefinition) : sortDefinition(aSortDefinition) {};

        void compareAndSwap(QueryTuple &lhs, QueryTuple &rhs);

    };
}

#endif //VAULTDB_EMP_SORT_CONDITION_H
