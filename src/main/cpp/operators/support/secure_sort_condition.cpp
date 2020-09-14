//
// Created by Jennie Rogers on 8/20/20.
//

#include <util/data_utilities.h>
#include "secure_sort_condition.h"

void SecureSortCondition::compareAndSwap(QueryTuple &lhs, QueryTuple &rhs) {

    emp::Bit swap(false);

    emp::Bit swapInit = swap; // false

    emp::Bit trueBit(true);
    emp::Bit falseBit(false);


    for(int i = 0; i < sortDefinition.size(); ++i) {
        int sortColIdx = sortDefinition[i].first;
        types::Value lhsValue = SortCondition::getValue(lhs, sortDefinition[i]);
        types::Value rhsValue = SortCondition::getValue(rhs, sortDefinition[i]);


        types::Value gtValue = lhsValue > rhsValue;
        emp::Bit gt = gtValue.getEmpBit();

        types::Value eqValue = (lhsValue == rhsValue);
        emp::Bit eq = eqValue.getEmpBit();

        SortDirection direction = sortDefinition[i].second;

        // is a swap needed?
        // if (lhs > rhs AND descending) OR (lhs < rhs AND ASCENDING)
        emp::Bit colSwapFlag;
        if(direction == vaultdb::SortDirection::ASCENDING) {
            colSwapFlag = !gt;
        }
        else if(direction == vaultdb::SortDirection::DESCENDING) {
            colSwapFlag = gt;
        }
        else {
            throw;
        }


        // find first one where not eq, use this to init flag

        swap = If(swapInit, swap, colSwapFlag); // once we know there's a swap once, we keep it
        swapInit = swapInit  | If(!eq, trueBit, falseBit);  // have we found the most significant column where they are not equal?
       // std::cout << "   Comparing " << lhs.reveal(emp::PUBLIC) <<  " to " << rhs.reveal(emp::PUBLIC) << " on col: " <<  sortColIdx << " gt? " << gt.reveal() << " toSwap? " << swap.reveal() << std::endl;


    } // end check for swap


    QueryTuple::compareAndSwap(lhs, rhs, swap);
}
/*     if((gt && direction == SortDirection::DESCENDING)  ||
                (!gt && direction == SortDirection::ASCENDING)){
                swap = true;
                break;
            }
            else if (!eq) {
                break; // no switch needed, they are already in the right order
            }
*/