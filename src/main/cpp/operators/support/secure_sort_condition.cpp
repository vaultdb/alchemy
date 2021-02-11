#include <util/data_utilities.h>
#include "secure_sort_condition.h"

void SecureSortCondition::compareAndSwap(QueryTuple &lhs, QueryTuple &rhs) {

    emp::Bit swap(false);

    emp::Bit swapInit = swap; // false

    emp::Bit trueBit(true);
    emp::Bit falseBit(false);


    for(int i = 0; i < sortDefinition.size(); ++i) {
        types::Value lhsValue = SortCondition::getValue(lhs, sortDefinition[i]);
        types::Value rhsValue = SortCondition::getValue(rhs, sortDefinition[i]);


        types::Value gtValue = lhsValue > rhsValue;
        emp::Bit gt = gtValue.getEmpBit();

        types::Value eqValue = (lhsValue == rhsValue);
        emp::Bit eq = eqValue.getEmpBit();

        SortDirection direction = sortDefinition[i].second;

        // is a swap needed?
        // if (lhs > rhs AND DESCENDING) OR (lhs < rhs AND ASCENDING)

        emp::Bit colSwapFlag;

        if(direction == vaultdb::SortDirection::ASCENDING) {
            colSwapFlag = (rhsValue > lhsValue).getEmpBit();
        }
        else if(direction == vaultdb::SortDirection::DESCENDING) {
            colSwapFlag = (lhsValue > rhsValue).getEmpBit();
        }
        else {
            throw;
        }


        // find first one where not eq, use this to init flag
        swap = If(swapInit, swap, colSwapFlag); // once we know there's a swap once, we keep it
        swapInit = swapInit  | If(!eq, trueBit, falseBit);  // have we found the most significant column where they are not equal?

    } // end check for swap



    QueryTuple::compareAndSwap(&lhs, &rhs, swap);
}

/* debug code:
 *     // variables to support debug statement
        std::string lhsRevealed = lhsValue.reveal(emp::PUBLIC).getVarchar();
        std::string rhsRevealed = rhsValue.reveal(emp::PUBLIC).getVarchar();
        uint32_t sortColIdx = sortDefinition[i].first;

        std::cout << "Lhs bytes: ";
        for(int i = 0; i < lhsRevealed.size(); ++i)
            std::cout << (int) lhsRevealed[i] << " ";
        std::cout << " for |" << lhsRevealed << "| encrypted bits: " << lhsValue.getEmpInt().reveal<std::string>() << std::endl;

        std::cout << "Rhs bytes: ";
        for(int i = 0; i < rhsRevealed.size(); ++i)
            std::cout << (int) rhsRevealed[i] << " ";
        std::cout <<  " for |" << rhsRevealed  <<  "| encrypted bits: " << rhsValue.getEmpInt().reveal<std::string>() <<  std::endl;

        bool expectedGT  = (lhsRevealed > rhsRevealed);
        bool observedGT =  (lhsValue > rhsValue).getEmpBit().reveal();
        std::cout << "Expected gt: " << expectedGT << " observed: " << observedGT << std::endl;
        std::cout << "Makeup: " << (lhsValue <= rhsValue).getEmpBit().reveal() << " == " << (lhsRevealed[i] <= rhsRevealed[i]) << std::endl;

        assert(expectedGT == observedGT);

        std::cout << "   Comparing " << lhsRevealed <<  " to " << rhsRevealed << " on col: " <<  sortColIdx << " gt? " << (lhsValue > rhsValue).getEmpBit().reveal() << " toSwap? " << swap.reveal() << std::endl;
*/