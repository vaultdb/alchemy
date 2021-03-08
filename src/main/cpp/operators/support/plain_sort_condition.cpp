#include "plain_sort_condition.h"

void PlainSortCondition::compareAndSwap(QueryTuple &lhs, QueryTuple &rhs) {

    bool swap = false;

    Field *eqValue;

    for(size_t i = 0; i < sortDefinition.size(); ++i) {
        const Field *lhsValue = SortCondition::getValue(lhs, sortDefinition[i]);
        const Field *rhsValue = SortCondition::getValue(rhs, sortDefinition[i]);

        eqValue = &(*lhsValue == *rhsValue);
        bool eq =  ((BoolField *) eqValue)->primitive();

        delete lhsValue;
        delete rhsValue;
        delete eqValue;

        SortDirection direction = sortDefinition[i].second;



        // is a swap needed?
        // if (lhs > rhs AND descending) OR (lhs < rhs AND ASCENDING)
        if((( lhsValue > rhsValue) && direction == SortDirection::DESCENDING)  ||
                (( rhsValue > lhsValue)&& direction == SortDirection::ASCENDING)){
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


