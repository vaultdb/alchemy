#include <util/field_utilities.h>
#include "sort_condition.h"

using namespace vaultdb;

template<typename T>
const Field *SortCondition<T>::getValue(QueryTuple &aTuple, const ColumnSort &aColumnSort) {
    return aColumnSort.first == -1 ? aTuple.getDummyTag() : aTuple.getField(aColumnSort.first);
}

template<typename T>
void SortCondition<T>::compareAndSwap(QueryTuple &lhs, QueryTuple &rhs) {

    T swap(false);

    T swapInit = swap; // false



    for (size_t i = 0; i < sortDefinition.size(); ++i) {
        const Field *lhsField = SortCondition::getValue(lhs, sortDefinition[i]);
        const Field *rhsField = SortCondition::getValue(rhs, sortDefinition[i]);


        T *eq = static_cast<T *>(FieldUtilities::equal(lhsField, rhsField));
        T neq = !(*eq);

        SortDirection direction = sortDefinition[i].second;

        // is a swap needed?
        // if (lhs > rhs AND DESCENDING) OR (lhs < rhs AND ASCENDING)
        T colSwapFlag = neq; // true if we are not equal

        if (direction == vaultdb::SortDirection::ASCENDING) {
            Field *gt = FieldUtilities::gt(rhsField, lhsField);
            colSwapFlag = static_cast<T &>(*gt);
            delete gt;
            //colSwapFlag = (rhsField > lhsValue).getEmpBit();
        } else if (direction == vaultdb::SortDirection::DESCENDING) {
            Field *gt = FieldUtilities::gt(lhsField, rhsField);
            colSwapFlag = static_cast<T &>(*gt);
            // colSwapFlag = (lhsField > rhsValue).getEmpBit();
            delete gt;
        } else {
            throw;
        }


        // find first one where not eq, use this to init flag
        swap = swap.select(swapInit, colSwapFlag);
        //         If(swapInit, swap, colSwapFlag); // once we know there's a swap once, we keep it
        swapInit = swapInit | neq;  // have we found the most significant column where they are not equal?

        delete eq;
    } // end check for swap

    QueryTuple::compareAndSwap(&lhs, &rhs, swap);
}

template class vaultdb::SortCondition<BoolField>;
template class vaultdb::SortCondition<SecureBoolField>;

    //;
    /*
    bool swap = false;

    for(size_t i = 0; i < sortDefinition.size(); ++i) {
        const Field *lhsValue = SortCondition::getValue(lhs, sortDefinition[i]);
        const Field *rhsValue = SortCondition::getValue(rhs, sortDefinition[i]);

        T eq = FieldUtilities::equal(lhsValue, rhsValue);
        T geq = FieldUtilities::geq(lhsValue, rhsValue);
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
    }*/


