//
// Created by Jennie Rogers on 8/20/20.
//

#include "secure_sort_condition.h"

void SecureSortCondition::compareAndSwap(QueryTuple &lhs, QueryTuple &rhs) {

    emp::Bit swap(false, emp::PUBLIC);


    for(int i = 0; i < sortDefinition.columnOrders.size(); ++i) {
        types::Value lhsValue = SortCondition::getValue(lhs, sortDefinition.columnOrders[i]);
        types::Value rhsValue = SortCondition::getValue(rhs, sortDefinition.columnOrders[i]);

        types::Value gtValue = lhsValue > rhsValue;
        emp::Bit gt = gtValue.getEmpBit();

        types::Value eqValue = (lhsValue == rhsValue);
        emp::Bit eq = eqValue.getEmpBit();

        SortDirection direction = sortDefinition.columnOrders[i].second;
        emp::Bit isAscending(direction == vaultdb::SortDirection::ASCENDING);
        emp::Bit isDescending(direction == vaultdb::SortDirection::DESCENDING);

        // is a swap needed?
        // if (lhs > rhs AND descending) OR (lhs < rhs AND ASCENDING)
        emp::Bit colSwapFlag = ((gt & isDescending) | (!gt & isAscending));
        swap = swap | colSwapFlag; // once we know there's a swap once, we keep it

    } // end check for swap


    QueryTuple::compareAndSwap(lhs, rhs, swap);
}
