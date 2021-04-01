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
        std::cout << "Input for bitonic merge over (" << lo << ", " << lo + cnt - 1 << "]: " << *Operator<B>::output << std::endl;
        bitonicMerge(Operator<B>::output, sortDefinition, lo, cnt, invertDir);

    }
}


/** The procedure BitonicMerge recursively sorts a bitonic sequence in
 * ascending order, if dir = ASCENDING, and in descending order
 * otherwise. The sequence to be sorted starts at index position lo,
 * the number of elements is cnt.
 **/
/*
 * 	if (n > 1) {
		int m = greatestPowerOfTwoLessThan(n);
		for (int i = lo; i < lo + n - m; i++)
			cmp_swap(key, data, i, i + m, acc);
		bitonic_merge(key, data, lo, m, acc);
		bitonic_merge(key, data, lo + m, n - m, acc);
	}
 */
template<typename B>
void Sort<B>::bitonicMerge( std::shared_ptr<QueryTable<B> > & table, const SortDefinition & sort_def, const int &lo, const int &n, const bool &invertDir) {

    if (n > 1) {
        int m = powerOfLessThanTwo(n);
        for (int i = lo; i < lo + n - m; i++) {
            QueryTuple<B> lhs = table->getTuple(i);
            QueryTuple<B> rhs =  table->getTuple(i+m);

            B to_swap = swapTuples(lhs, rhs, sort_def, invertDir);
            QueryTuple<B>::compare_swap(to_swap,lhs, rhs);


            bool swap_revealed = FieldUtilities::extract_bool(to_swap);
            std::cout << "Testing C&S on indexes " << i << ", " << i+m <<  std::endl;
            if(swap_revealed)
                std::cout << "     Swapped: " << lhs.getField(0).reveal() << ", " << rhs.getField(0).reveal() <<  std::endl;


        }
        bitonicMerge(table, sort_def, lo, m,  invertDir);
        bitonicMerge(table, sort_def, lo + m, n - m, invertDir);
    }
}

template<typename B>
void Sort<B>::compareAndSwap(const int &lhsIdx, const int &rhsIdx, const bool &invertDir) {
    QueryTuple<B> lhs = Operator<B>::output->getTuple(lhsIdx);
    QueryTuple<B> rhs =  Operator<B>::output->getTuple(rhsIdx);
    B toSwap = swapTuples(lhs, rhs, sortDefinition, invertDir);

    QueryTuple<B>::compare_swap(toSwap,lhs, rhs);

}

template<typename B>
Sort<B>::~Sort() {

}

template<typename B>
B Sort<B>::swapTuples(const QueryTuple<B> & lhs, const QueryTuple<B> & rhs, const SortDefinition  & sort_definition, const bool & invertDir)  {
    B swap(false);
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

        B colSwapFlag = (lhsField < rhsField) == B(asc);

        // find first one where not eq, use this to init flag
        swap =  Field<B>::If(swapInit, Field<B>(swap), Field<B>(colSwapFlag)).template getValue<B>(); // once we know there's a swap once, we keep it
        swapInit = swapInit | (lhsField != rhsField);  // have we found the first  column where they are not equal?
    }

    return swap;
}


template class vaultdb::Sort<bool>;
template class vaultdb::Sort<emp::Bit>;

