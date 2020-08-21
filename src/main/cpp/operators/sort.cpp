#include "sort.h"
#include "querytable/types/type_id.h"
#include "querytable/types/value.h"
#include <emp-tool/circuits/swappable.h>
#include <querytable/query_table.h>
#include <vector>
#include <operators/support/secure_sort_condition.h>
#include <operators/support/plain_sort_condition.h>

int Sort::powerOfLessThanTwo(const int & n) {
  int k = 1;
  while (k > 0 && k < n) {
    k *= 2;
  }
  return k / 2;
}

// swap_tuples attempts to be an oblivious swap.
// Ideally the execution trace of this will be the same
// regardless if the swap occurs.
//

void swapTuples(int t1, int t2, QueryTable *t, types::Value *swap_condition) {
  QueryTuple tup1 = t->getTuple(t1);
  QueryTuple tup2 = t->getTuple(t2);

/*  for (int i = 0; i < t->getSchema()->getFieldCount(); i++) {
    vaultdb::expression::Expression ex(
        swap_condition, tup1->GetMutableField(i)->GetMutableValue(),
        tup2->GetMutableField(i)->GetMutableValue(),
        vaultdb::expression::ExpressionId::SWAP);
    ex.ExecuteMutable();
  }
  vaultdb::expression::Expression ex2(
          swap_condition, tup1->GetMutableDummyTag(), tup2->GetMutableDummyTag(),
          vaultdb::expression::ExpressionId::SWAP);
  ex2.ExecuteMutable();*/

}

void Compare(int t1, int t2, QueryTable *t, SortDefinition &s, bool dir,
             bool dummy_dir) {

  types::Value comparator;
  types::Value v_and;
  bool comparator_init = false;
  bool v_and_init = true;
  if (t->isEncrypted()) {
    emp::Bit b1(comparator_init, emp::BOB);
    emp::Bit b2(v_and_init, emp::BOB);
    comparator = types::Value(b1);
    v_and = types::Value(b2);
  } else {
    comparator = types::Value(comparator_init);
    v_and = types::Value(v_and_init);
  }

  /* TODO(Jennie): Fix this up with the new PredicateClass framework
  vaultdb::expression::ExpressionId compare_dir =
      dir ? vaultdb::expression::ExpressionId::GREATERTHAN
          : vaultdb::expression::ExpressionId::LESSTHAN;

  vaultdb::expression::ExpressionId dummy_compare_dir =
      dummy_dir ? vaultdb::expression::ExpressionId::GREATERTHAN
                : vaultdb::expression::ExpressionId::LESSTHAN;

  // This complicated algorithm is here to enable sorting on multiple
  // attributes, the comparison bit has to be carried.
  for (auto idx : s.columnOrders) {
    const vaultdb::types::Value *val1, *val2;
    if (idx == -1) {
      val1 = t->getTuple(t1)->GetDummyTag();
      val2 = t->getTuple(t2)->getDummyTag();
    } else {
      val1 = t->getTuple(t1)->getField(idx)->GetValue(),
      val2 = t->getTuple(t2)->getField(idx)->GetValue();
    }
    vaultdb::expression::Expression ex(
        val1, val2, (idx == -1) ? dummy_compare_dir : compare_dir);
    auto v = ex.execute();
    vaultdb::expression::Expression ex_cascade(
        &v, &v_and, vaultdb::expression::ExpressionId::AND);
    auto v_cascade = ex_cascade.execute();
    vaultdb::expression::Expression ex2(&comparator, &v_cascade,
                                        vaultdb::expression::ExpressionId::OR);
    comparator = ex2.execute();
    vaultdb::expression::Expression ex_and(
        val1, val2, vaultdb::expression::ExpressionId::EQUAL);
    v_and = ex_and.execute();
  }
  SwapTuples(t1, t2, t, &comparator); */
}

/** The procedure BitonicMerge recursively sorts a bitonic sequence in
 * ascending order, if dir = ASCENDING, and in descending order
 * otherwise. The sequence to be sorted starts at index position lo,
 * the number of elements is cnt.
 **/
/*void BitonicMerge(int lo, int n, QueryTable *t, SortDefinition &s, bool dir,
                  bool dummy_dir) {
  if (n > 1) {
    int m = powerOfLessThanTwo(n);
    for (int i = lo; i < lo + n - m; i++) {
      Compare(i, i + m, t, s, dir, dummy_dir);
    }
    BitonicMerge(lo, m, t, s, dir, dummy_dir);
    BitonicMerge(lo + m, n - m, t, s, dir, dummy_dir);
  }
}
*/

/** Procedure bitonicSort first produces a bitonic sequence by
 * recursively sorting its two halves in opposite directions, and then
 * calls bitonicMerge.
 **/


/*void BitonicSort(int lo, int cnt, QueryTable *t, SortDefinition &s, bool dir,
                 bool dummy_dir) {
  if (cnt > 1) {
    int k = cnt / 2;
    BitonicSort(lo, k, t, s, !dir, !dummy_dir);
    BitonicSort(lo + k, cnt - k, t, s, dir, dummy_dir);
    BitonicMerge(lo, cnt, t, s, dir, dummy_dir);
  }
}*/

void Sort(QueryTable *input, SortDefinition &s) {
/*  bool dir = s.order == SortDirection::ASCENDING ? true : false;
  bool dummy_dir = s.dummyOrder == SortDirection::ASCENDING ? true : false;
  BitonicSort(0, input->getTupleCount(), input, s, dir, dummy_dir);*/
}

Sort::Sort(const SortDefinition &aSortDefinition, std::shared_ptr<Operator> &child) : Operator(child), sortDefinition(aSortDefinition) {


}

std::shared_ptr<QueryTable> Sort::runSelf() {
    std::shared_ptr<QueryTable> input = children[0]->getOutput();
    SortDefinition reverseSortDefinition = getReverseSortDefinition(sortDefinition);

    if(input->isEncrypted()) {
        sortCondition = std::unique_ptr<SortCondition>(new SecureSortCondition(sortDefinition));
        reverseSortCondition = std::unique_ptr<SortCondition>(new SecureSortCondition(reverseSortDefinition));
    }
    else {
        sortCondition = std::unique_ptr<SortCondition>(new PlainSortCondition(sortDefinition));
        reverseSortCondition = std::unique_ptr<SortCondition>(new PlainSortCondition(reverseSortDefinition));
    }

    // deep copy new output
    output = std::shared_ptr<QueryTable>(new QueryTable(*input));
    bitonicSort(0,  output->getTupleCount(), true);
    return output;
}

void Sort::bitonicSort(const int &lo, const int &cnt, bool invertDir) {
    if (cnt > 1) {
        int k = cnt / 2;
        bitonicSort(lo, k, !invertDir);
        bitonicSort(lo + k, cnt - k, invertDir);
        bitonicMerge(lo, cnt, invertDir);
    }
}

void Sort::bitonicMerge(const int &lo, const int &n, bool invertDir) {

    if (n > 1) {
        int m = powerOfLessThanTwo(n);
        for (int i = lo; i < lo + n - m; i++) {
            compareAndSwap(i, i + m, invertDir);
        }
        bitonicMerge(lo, m, invertDir);
        bitonicMerge(lo + m, n - m, invertDir);
    }
}

void Sort::compareAndSwap(const int &lhsIdx, const int &rhsIdx, bool invertDir) {
    QueryTuple lhs = output->getTuple(lhsIdx);
    QueryTuple rhs = output->getTuple(rhsIdx);

    if(!invertDir) {
        sortCondition->compareAndSwap(lhs, rhs);
    }
    else {
        reverseSortCondition->compareAndSwap(lhs, rhs);
    }

    output->putTuple(lhsIdx, lhs);
    output->putTuple(rhsIdx, rhs);




}

SortDefinition Sort::getReverseSortDefinition(const SortDefinition & aSortDef) {
    SortDefinition reverseSortDefinition;

    for(ColumnSort cs : aSortDef.columnOrders) {
        ColumnSort reversed = cs;
        reversed.second = (reversed.second == SortDirection::ASCENDING) ?  SortDirection::DESCENDING : SortDirection::ASCENDING;
        reverseSortDefinition.columnOrders.push_back(reversed);
    }

    return reverseSortDefinition;

}


