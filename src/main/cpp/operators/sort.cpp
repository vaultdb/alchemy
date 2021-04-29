#include "sort.h"

#include "plain_tuple.h"
#include "secure_tuple.h"
#include <util/field_utilities.h>


using namespace vaultdb;

template<typename B>
int Sort<B>::powerOfLessThanTwo(const int & n) {
    int k = 1;
    while (k < n)
        k = k << 1;
    return k >> 1;
}

template<typename B>
Sort<B>::Sort(Operator<B> *child, const SortDefinition &aSortDefinition, const int & limit) : Operator<B>(child), sortDefinition(aSortDefinition), limit_(limit) {

    for(ColumnSort s : sortDefinition) {
        if(s.second == SortDirection::INVALID)
            throw; // invalid sort definition
    }

    if(limit_ > 0)
        assert(sortDefinition[0].first == -1); // Need to sort on dummy tag to make resizing not delete real tuples


}

template<typename B>
Sort<B>::Sort(shared_ptr<QueryTable<B> > child, const SortDefinition &aSortDefinition, const int & limit) : Operator<B>(child), sortDefinition(aSortDefinition), limit_(limit) {

    for(ColumnSort s : sortDefinition) {
        if(s.second == SortDirection::INVALID)
            throw; // invalid sort definition
    }

    if(limit_ > 0)
        assert(sortDefinition[0].first == -1); // Need to sort on dummy tag to make resizing not delete real tuples

}

template<typename B>
std::shared_ptr<QueryTable<B> > Sort<B>::runSelf() {
    std::shared_ptr<QueryTable<B> > input = Operator<B>::children_[0]->getOutput();

    // deep copy new output
    Operator<B>::output = std::shared_ptr<QueryTable<B> >(new QueryTable<B>(*input));
    bitonicSort(0,  Operator<B>::output->getTupleCount(), true);

    Operator<B>::output->setSortOrder(sortDefinition);

    if(limit_ > 0) {
        Operator<B>::output->resize(limit_);
    }
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
        bitonicMerge(Operator<B>::output, sortDefinition, lo, cnt, invertDir);

    }
}


/** The procedure BitonicMerge recursively sorts a bitonic sequence in
 * ascending order, if dir = ASCENDING, and in descending order
 * otherwise. The sequence to be sorted starts at index position lo,
 * the number of elements is cnt.
 **/
template<typename B>
void Sort<B>::bitonicMerge( std::shared_ptr<QueryTable<B> > & table, const SortDefinition & sort_def, const int &lo, const int &n, const bool &invertDir) {

    if (n > 1) {
        int m = powerOfLessThanTwo(n);
        for (int i = lo; i < lo + n - m; i++) {
            QueryTuple<B> lhs = table->getTuple(i);
            QueryTuple<B> rhs =  table->getTuple(i+m);

            B to_swap = swapTuples(lhs, rhs, sort_def, invertDir);
            QueryTuple<B>::compareSwap(to_swap, lhs, rhs);

        }
        bitonicMerge(table, sort_def, lo, m,  invertDir);
        bitonicMerge(table, sort_def, lo + m, n - m, invertDir);
    }
}


template<typename B>
Sort<B>::~Sort() {

}

template<typename B>
B Sort<B>::swapTuples(const QueryTuple<B> & lhs, const QueryTuple<B> & rhs, const SortDefinition  & sort_definition, const bool & invertDir)  {
    B swap = false;
    B swapInit = swap;


    Field<B> lhsDummyTag = Field<B>(lhs.getDummyTag());
    Field<B> rhsDummyTag = Field<B>(rhs.getDummyTag());

    for (size_t i = 0; i < sort_definition.size(); ++i) {

        const Field<B> lhsField = sort_definition[i].first == -1 ? lhsDummyTag
                                                                 : lhs.getField(sort_definition[i].first);
        const Field<B> rhsField = sort_definition[i].first == -1 ? rhsDummyTag
                                                                 : rhs.getField(sort_definition[i].first);

        bool asc = (sort_definition[i].second == SortDirection::ASCENDING);
        if (invertDir)
            asc = !asc;

        B neq = lhsField != rhsField;
        B lt = lhsField < rhsField;
        B colSwapFlag = (lt == asc);

        // find first one where not eq, use this to init flag
        swap = FieldUtilities::select(swapInit, swap, colSwapFlag);
        swapInit = swapInit | neq;

    }

    return swap;
}



template class vaultdb::Sort<bool>;
template class vaultdb::Sort<emp::Bit>;

