#include "sort.h"

using namespace vaultdb;

template<typename T>
int Sort<T>::powerOfLessThanTwo(const int & n) {
  int k = 1;
  while (k > 0 && k < n) {
    k *= 2;
  }
  return k / 2;
}

template<typename T>
Sort<T>::Sort(Operator *child, const SortDefinition &aSortDefinition) : Operator(child), sortDefinition(aSortDefinition) {


}

template<typename T>
Sort<T>::Sort(shared_ptr<QueryTable> child, const SortDefinition &aSortDefinition) : Operator(child), sortDefinition(aSortDefinition){

}


template<typename T>
std::shared_ptr<QueryTable> Sort<T>::runSelf() {
    std::shared_ptr<QueryTable> input = children[0]->getOutput();

    sortCondition = SortCondition<T>(sortDefinition);
    SortDefinition reverseSortDefinition = getReverseSortDefinition(sortDefinition);
    reverseSortCondition = SortCondition<T>(reverseSortDefinition);


    // deep copy new output
    output = std::shared_ptr<QueryTable>(new QueryTable(*input));
    bitonicSort(0,  output->getTupleCount(), true);

    output->setSortOrder(sortDefinition);

    return output;
}


/** Procedure bitonicSort first produces a bitonic sequence by
 * recursively sorting its two halves in opposite directions, and then
 * calls bitonicMerge.
 **/

template<typename T>
void Sort<T>::bitonicSort(const int &lo, const int &cnt, bool invertDir) {
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

template<typename T>
void Sort<T>::bitonicMerge(const int &lo, const int &n, bool invertDir) {

    if (n > 1) {
        int m = powerOfLessThanTwo(n);
        for (int i = lo; i < lo + n - m; i++) {
            compareAndSwap(i, i + m, invertDir);
        }
        bitonicMerge(lo, m, invertDir);
        bitonicMerge(lo + m, n - m, invertDir);
    }
}

template<typename T>
void Sort<T>::compareAndSwap(const int &lhsIdx, const int &rhsIdx, bool invertDir) {
    QueryTuple lhs = output->getTuple(lhsIdx);
    QueryTuple rhs = output->getTuple(rhsIdx);

    if(!invertDir) {
        sortCondition.compareAndSwap(lhs, rhs);
    }
    else {
        reverseSortCondition.compareAndSwap(lhs, rhs);
    }

    output->putTuple(lhsIdx, lhs);
    output->putTuple(rhsIdx, rhs);




}

template<typename T>
SortDefinition Sort<T>::getReverseSortDefinition(const SortDefinition & aSortDef) {
    SortDefinition reverseSortDefinition;

    for(ColumnSort cs : aSortDef) {
        ColumnSort reversed = cs;
        reversed.second = (reversed.second == SortDirection::ASCENDING) ?  SortDirection::DESCENDING : SortDirection::ASCENDING;
        reverseSortDefinition.push_back(reversed);
    }

    return reverseSortDefinition;

}

template<typename T>
Sort<T>::~Sort() {

    //if(sortCondition) delete sortCondition;
    //if(reverseSortCondition) delete reverseSortCondition;

}

template class vaultdb::Sort<BoolField>;
template class vaultdb::Sort<SecureBoolField>;




