#include "plain_sort_condition.h"

void PlainSortCondition::compareAndSwap(QueryTuple &lhs, QueryTuple &rhs) {

    bool swap = false;



    for(int i = 0; i < sortDefinition.size(); ++i) {
        types::Value lhsValue = SortCondition::getValue(lhs, sortDefinition[i]);
        types::Value rhsValue = SortCondition::getValue(rhs, sortDefinition[i]);

        types::Value gtValue = lhsValue > rhsValue;
        bool gt = gtValue.getBool();

        types::Value eqValue = lhsValue == rhsValue;
        bool eq = eqValue.getBool();

        SortDirection direction = sortDefinition[i].second;



        // is a swap needed?
        // if (lhs > rhs AND descending) OR (lhs < rhs AND ASCENDING)
        if((( lhsValue > rhsValue).getBool() && direction == SortDirection::DESCENDING)  ||
                (( rhsValue > lhsValue).getBool() && direction == SortDirection::ASCENDING)){
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


