#include "sort.h"

#include <query_table/plain_tuple.h>
// keep this file to ensure overloaded methods are visible
#include <query_table/secure_tuple.h>
#include <util/field_utilities.h>
#include <util/data_utilities.h>


using namespace vaultdb;

template<typename B>
int Sort<B>::powerOfTwoLessThan(const int & n) {
    int k = 1;
    while (k > 0 && k < n)
        k = k << 1;
    return k >> 1;
}

template<typename B>
Sort<B>::Sort(Operator<B> *child, const SortDefinition &aSortDefinition, const int & limit) : Operator<B>(child, aSortDefinition), limit_(limit) {

    for(ColumnSort s : Operator<B>::sort_definition_) {
        if(s.second == SortDirection::INVALID)
            throw; // invalid sort definition
    }

    // Need to sort on dummy tag to make resizing not delete real tuples
    // otherwise may produce incorrect results
    if(limit_ > 0)
        assert(Operator<B>::sort_definition_[0].first == -1);

}

template<typename B>
Sort<B>::Sort(shared_ptr<QueryTable<B> > child, const SortDefinition &aSortDefinition, const int & limit) : Operator<B>(child, aSortDefinition), limit_(limit) {

    for(ColumnSort s : Operator<B>::sort_definition_) {
        if(s.second == SortDirection::INVALID)
            throw; // invalid sort definition
    }

    if(limit_ > 0)
        assert(Operator<B>::sort_definition_[0].first == -1); // Need to sort on dummy tag to make resizing not delete real tuples

}

template<typename B>
std::shared_ptr<QueryTable<B> > Sort<B>::runSelf() {
    std::shared_ptr<QueryTable<B> > input = Operator<B>::children_[0]->getOutput();



    // deep copy new output
    Operator<B>::output_ = std::shared_ptr<QueryTable<B> >(new QueryTable<B>(*input));

    bitonicSort(0, Operator<B>::output_->getTupleCount(), true);

    Operator<B>::output_->setSortOrder(Operator<B>::sort_definition_);

    // implement LIMIT
    if(limit_ > 0) {
        size_t cutoff = limit_;

        // can't have more tuples than are initialized
        if(cutoff > Operator<B>::output_->getTupleCount())
            cutoff = Operator<B>::output_->getTupleCount();

        // if in plaintext, truncate to true length or limit_, whichever one is lower
        if(std::is_same_v<B, bool>) {
            int cursor = 0;
            while(!Operator<B>::output_->getPlainTuple(cursor).getDummyTag() && cursor < cutoff)
                ++cursor;

              cutoff = cursor;
        }
        Operator<B>::output_->resize(cutoff);
    }


    return Operator<B>::output_;
}




/** Procedure bitonicSort first produces a bitonic sequence by
 * recursively sorting its two halves in opposite directions, and then
 * calls bitonicMerge.
 * ASC => (dir == TRUE)
 * DESC => (dir == FALSE)
 **/

template<typename B>
void Sort<B>::bitonicSort(const int &lo, const int &cnt, const bool &dir) {
    if (cnt > 1) {
        int m = cnt / 2;
        bitonicSort(lo, m, !dir);
        bitonicSort(lo + m, cnt - m, dir);
        bitonicMerge(Operator<B>::output_, Operator<B>::sort_definition_, lo, cnt, dir);
    }

}


/** The procedure BitonicMerge recursively sorts a bitonic sequence in
 * ascending order, if dir = ASCENDING, and in descending order
 * otherwise. The sequence to be sorted starts at index position lo,
 * the number of elements is cnt.
 **/
template<typename B>
void Sort<B>::bitonicMerge( std::shared_ptr<QueryTable<B> > & table, const SortDefinition & sort_def, const int &lo, const int &n, const bool &dir) {

    if (n > 1) {
        int m = powerOfTwoLessThan(n);
        for (int i = lo; i < lo + n - m; ++i) {
            QueryTuple<B> lhs = table->getTuple(i);
            QueryTuple<B> rhs = table->getTuple(i + m);

            B to_swap = swapTuples(lhs, rhs, sort_def, dir);
            QueryTuple<B>::compareSwap(to_swap, lhs, rhs);

        }

        bitonicMerge(table, sort_def, lo, m,  dir);
        bitonicMerge(table, sort_def, lo + m, n - m, dir);
    }
}


template<typename B>
Sort<B>::~Sort() {

}

template<typename B>
B Sort<B>::swapTuples(const QueryTuple<B> & lhs, const QueryTuple<B> & rhs, const SortDefinition  & sort_definition, const bool & dir)  {
    B swap = false;
    B swap_init = swap;



    for (size_t i = 0; i < sort_definition.size(); ++i) {

        const Field<B> lhs_field = sort_definition[i].first == -1 ? Field<B>(lhs.getDummyTag())
                                                                  : lhs.getField(sort_definition[i].first);
        const Field<B> rhs_field = sort_definition[i].first == -1 ? Field<B>(rhs.getDummyTag())
                                                                  : rhs.getField(sort_definition[i].first);

        // true for ascending, false for descending
        bool asc = (sort_definition[i].second == SortDirection::ASCENDING);

        B to_swap =  (lhs_field < rhs_field) == asc;
        if(dir)  // flip the bit
            to_swap = !to_swap;

        // if  ascending (1) & dir == 1 --> asc (1) (lhs < rhs (1) means swap!)
        // if DESC (0) & dir == 0 --> ASC (1) (lhs < rhs (1) means swap!)
        // if ASC (1) & dir = 0 --> DESC  (0) (lhs < rhs (1)  means no swap)
        // if DESC (0) & dir == 1  --> DESC (0) (lhs < rhs (1)  means no swap)


        // find first one where not eq, use this to init flag
        swap = FieldUtilities::select(swap_init, swap, to_swap);
        swap_init = swap_init | (lhs_field != rhs_field);
        if(std::is_same_v<bool, B>) {
            bool bool_init = FieldUtilities::extract_bool(swap_init);
            if(bool_init) break;
        }
    }


    return swap;
}

template<typename B>
string Sort<B>::getOperatorType() const {
    return "Sort";
}

template<typename B>
string Sort<B>::getParameters() const {
    return DataUtilities::printSortDefinition(Operator<B>::sort_definition_);
}


template class vaultdb::Sort<bool>;
template class vaultdb::Sort<emp::Bit>;

