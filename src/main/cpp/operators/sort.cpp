#include "sort.h"


using namespace vaultdb;

template<typename B>
int Sort<B>::powerOfLessThanTwo(const int & n) {
    int k = 1;
    while (k > 0 && k < n) {
        k *= 2;
    }
    return k / 2;
}

template<typename B>
Sort<B>::Sort(Operator<B> *child, const SortDefinition &aSortDefinition) : Operator<B>(child), sortDefinition(aSortDefinition) {

    for(ColumnSort s : sortDefinition) {
        if(s.second == SortDirection::INVALID)
            throw; // invalid sort definition
    }

}

template<typename B>
Sort<B>::Sort(shared_ptr<QueryTable<B> > child, const SortDefinition &aSortDefinition) : Operator<B>(child), sortDefinition(aSortDefinition){

    for(ColumnSort s : sortDefinition) {
        if(s.second == SortDirection::INVALID)
            throw; // invalid sort definition
    }

}

template<typename B>
std::shared_ptr<QueryTable<B> > Sort<B>::runSelf() {
    std::shared_ptr<QueryTable<B> > input = Operator<B>::children[0]->getOutput();

    // deep copy new output
    Operator<B>::output = std::shared_ptr<QueryTable<B> >(new QueryTable<B>(*input));
    bitonicSort(0,  Operator<B>::output->getTupleCount(), true);

    Operator<B>::output->setSortOrder(sortDefinition);

    return Operator<B>::output;
}


/** Procedure bitonicSort first produces a bitonic sequence by
 * recursively sorting its two halves in opposite directions, and then
 * calls bitonicMerge.
 **/

template<typename B>
void Sort<B>::bitonicSort(const int &lo, const int &cnt, const bool &invertDir) {
    if (cnt > 1) {
        int k = cnt / 2;
        bitonicSort(lo, k, !invertDir);
        bitonicSort(lo + k, cnt - k, invertDir);
        bitonicMerge(lo, cnt, invertDir);

    }
}


/** The procedure BitonicMerge recursively sorts a bitonic sequence in
 * ascending order, if dir = ASCENDING, and in descending order
 * otherwise. The sequence to be sorted starts at index position lo,
 * the number of elements is cnt.
 **/

template<typename B>
void Sort<B>::bitonicMerge(const int &lo, const int &n, const bool &invertDir) {

    if (n > 1) {
        int m = powerOfLessThanTwo(n);
        for (int i = lo; i < lo + n - m; i++) {
            compareAndSwap(i, i + m, invertDir);
        }
        bitonicMerge(lo, m, invertDir);
        bitonicMerge(lo + m, n - m, invertDir);
    }
}

template<typename B>
void Sort<B>::compareAndSwap(const int &lhsIdx, const int &rhsIdx, const bool &invertDir) {
    B toSwap = swapTuples(lhsIdx, rhsIdx, invertDir);
    QueryTuple<B>::compareAndSwap(toSwap, Operator<B>::output->getTuplePtr(lhsIdx), Operator<B>::output->getTuplePtr(rhsIdx));

}

template<typename B>
Sort<B>::~Sort() {

}

template<typename B>
B Sort<B>::swapTuples(const int &lhsIdx, const int &rhsIdx, const bool &invertDir) {
    B swap(false);
    B swapInit = swap;

    QueryTuple<B> lhs = (*Operator<B>::output)[lhsIdx];
    QueryTuple<B> rhs = (*Operator<B>::output)[rhsIdx];
    std::cout << "Comparing " << lhs.reveal().toString(true) << " to " << rhs.reveal().toString() << std::endl;

    for (size_t i = 0; i < sortDefinition.size(); ++i) {
        const Field<B> *lhsField = sortDefinition[i].first == -1 ? lhs.getDummyTag()
                                                                 : lhs.getField(sortDefinition[i].first);
        const Field<B> *rhsField = sortDefinition[i].first == -1 ? rhs.getDummyTag()
                                                                 : rhs.getField(sortDefinition[i].first);

        bool asc = (sortDefinition[i].second == SortDirection::ASCENDING);
        if (invertDir)
            asc = !asc;

        B colSwapFlag = (*lhsField < *rhsField) == B(asc);

        // find first one where not eq, use this to init flag
        swap = (B) Field<B>::If(swapInit, swap, colSwapFlag); // once we know there's a swap once, we keep it

        B neq = *lhsField != *rhsField;
        std::cout << "Neq: " << neq.reveal().toString() << std::endl;
        swapInit = swapInit | (*lhsField != *rhsField);  // have we found the first  column where they are not equal?
        std::cout << "Fields: " << lhsField->reveal().toString() << ", " << rhsField->reveal().toString() << ", colSwapFlag: " << colSwapFlag.reveal().toString() << ", toSwap: " << swap.reveal().toString()  << ", swapInit: " << swapInit.reveal().toString() <<  std::endl;

    }

    std::cout << "With invertDir=" << invertDir << " toSwap? " << swap.reveal().toString() << std::endl;
    return swap;
}


template class vaultdb::Sort<BoolField>;
template class vaultdb::Sort<SecureBoolField>;

