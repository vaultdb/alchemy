#include <util/field_utilities.h>
#include "plain_sort_condition.h"

void PlainSortCondition::compareAndSwap(QueryTuple &lhs, QueryTuple &rhs) {

    bool swap = false;

    for(size_t i = 0; i < sortDefinition.size(); ++i) {
        const Field *lhsValue = SortCondition::getValue(lhs, sortDefinition[i]);
        const Field *rhsValue = SortCondition::getValue(rhs, sortDefinition[i]);

        bool eq = FieldUtilities::equal(lhsValue, rhsValue);
        bool geq = FieldUtilities::geq(lhsValue, rhsValue);
        SortDirection direction = sortDefinition[i].second;



        // is a swap needed?
        // if (lhs > rhs AND descending) OR (lhs < rhs AND ASCENDING)
        if((geq && !eq && direction == SortDirection::DESCENDING)  ||
           (!geq && direction == SortDirection::ASCENDING)){
            swap = true;
            break;
        }
        else if (!eq) {
            break; // no switch needed, they are already in the right order
        }


    } // end check for swap


    if(swap) {
        QueryTuple tmp = lhs;
        lhs = rhs;
        rhs = tmp;
    }

}

