#include "sort_condition.h"

const Field *SortCondition::getValue(QueryTuple &aTuple, const ColumnSort &aColumnSort) {
    return aColumnSort.first == -1 ? aTuple.getDummyTag() : aTuple.getField(aColumnSort.first);
}
