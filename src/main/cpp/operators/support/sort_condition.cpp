#include <util/field_utilities.h>
#include "sort_condition.h"

using namespace vaultdb;

template<typename B>
const Field<B> *SortCondition<B>::getValue(QueryTuple<B> &aTuple, const ColumnSort &aColumnSort) {
    return aColumnSort.first == -1 ? aTuple.getDummyTag() : aTuple.getField(aColumnSort.first);
}

template<typename B>
void SortCondition<B>::compareAndSwap(QueryTuple<B> &lhs, QueryTuple<B> &rhs) {

    B swap(false);

    B swapInit = swap; // false



    for (size_t i = 0; i < sortDefinition.size(); ++i) {
        const Field<B> *lhsField = SortCondition::getValue(lhs, sortDefinition[i]);
        const Field<B> *rhsField = SortCondition::getValue(rhs, sortDefinition[i]);

        B neq = *lhsField != *rhsField;


        SortDirection direction = sortDefinition[i].second;

        // is a swap needed?
        // if (lhs > rhs AND DESCENDING) OR (lhs < rhs AND ASCENDING)
        B colSwapFlag = neq; // true if we are not equal

        if (direction == vaultdb::SortDirection::ASCENDING) {
            colSwapFlag = *rhsField > *lhsField;
            //colSwapFlag = (rhsField > lhsValue).getEmpBit();
        } else if (direction == vaultdb::SortDirection::DESCENDING) {
            colSwapFlag = *lhsField > *rhsField;
            // colSwapFlag = (lhsField > rhsValue).getEmpBit();
        } else {
            throw;
        }


        // find first one where not eq, use this to init flag
        swap = static_cast<B>(Field<B>::If(swapInit, swap, colSwapFlag));
        //         If(swapInit, swap, colSwapFlag); // once we know there's a swap once, we keep it
        swapInit = swapInit | neq;  // have we found the most significant column where they are not equal?

    } // end check for swap

    QueryTuple<B>::compareAndSwap(swap, &lhs, &rhs);
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


