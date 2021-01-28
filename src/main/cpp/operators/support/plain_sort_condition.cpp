//
// Created by Jennie Rogers on 8/20/20.
//

#include "plain_sort_condition.h"

void PlainSortCondition::compareAndSwap(QueryTuple &lhs, QueryTuple &rhs) {

    bool swap = false;

    // begin debug
    string dirString = (sortDefinition[0].second == SortDirection::ASCENDING) ? "ASCENDING" : "DESCENDING";
    int lhsPatientId = lhs.getField(0).getValue().reveal().getInt32();
    int rhsPatientId = rhs.getField(0).getValue().reveal().getInt32();
    int lhsSiteId = lhs.getField(8).getValue().reveal().getInt32();
    int rhsSiteId = rhs.getField(8).getValue().reveal().getInt32();

    // should be "no swap" for first case
    if(lhsPatientId == rhsPatientId && lhsPatientId == 5) {
        std::cout << "Begin debug on patid, direction = " << dirString << std::endl;
    }

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



    if(lhsPatientId == rhsPatientId && lhsPatientId == 5) {

        std::cout << "Comparing " << lhs << " to " << rhs << " swapping? " << swap << std::endl;
        std::cout << "Direction is: " << dirString<< std::endl;
        std::cout << "Comparing on (" << lhsPatientId << ", " << lhsSiteId << ") vs (" << rhsPatientId << ", " << rhsSiteId << ")" << std::endl;
    }


    if(swap) {
        QueryTuple tmp = lhs;
        lhs = rhs;
        rhs = tmp;
    }

}


