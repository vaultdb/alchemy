#include "sort_condition.h"

types::Value SortCondition::getValue(QueryTuple &aTuple, const ColumnSort &aColumnSort) {
    return aColumnSort.first == -1 ? aTuple.getDummyTag() : aTuple.getField(aColumnSort.first).getValue();
}
