#include "secure_sort.h"
#include "querytable/expression/expression_id.h"
#include "querytable/types/type_id.h"
#include "querytable/types/value.h"
#include <emp-tool/circuits/swappable.h>
#include <querytable/expression/expression.h>
#include <querytable/query_table.h>
#include <vector>

int power_of_two_less_than(int n) {
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
void SwapTuples(int t1, int t2, QueryTable *t, types::Value *swap_condition) {
  auto tup1 = t->GetTuple(t1);
  auto tup2 = t->GetTuple(t2);
  for (int i = 0; i < t->GetSchema()->GetNumFields(); i++) {
    vaultdb::expression::Expression ex(
        swap_condition, tup1->GetMutableField(i)->GetMutableValue(),
        tup2->GetMutableField(i)->GetMutableValue(),
        vaultdb::expression::ExpressionId::SWAP);
    ex.ExecuteMutable();
  }
}

void Compare(int t1, int t2, QueryTable *t, SortDef &s, bool dir) {

  types::Value comparator;
  types::Value v_and;
  bool comparator_init = false;
  bool v_and_init = true;
  if (t->GetIsEncrypted()) {
    emp::Bit b1(comparator_init, emp::BOB);
    emp::Bit b2(v_and_init, emp::BOB);
    comparator = types::Value(types::TypeId::ENCRYPTED_BOOLEAN, b1);
    v_and = types::Value(types::TypeId::ENCRYPTED_BOOLEAN, b2);
  } else {
    comparator = types::Value(types::TypeId::BOOLEAN, comparator_init);
    v_and = types::Value(types::TypeId::BOOLEAN, v_and_init);
  }
  vaultdb::expression::ExpressionId compare_dir =
      dir ? vaultdb::expression::ExpressionId::GREATERTHAN
          : vaultdb::expression::ExpressionId::LESSTHAN;
  for (auto idx : s.ordinals) {
    vaultdb::expression::Expression ex(
        t->GetTuple(t1)->GetField(idx)->GetValue(),
        t->GetTuple(t2)->GetField(idx)->GetValue(), compare_dir);
    auto v = ex.execute();
    vaultdb::expression::Expression ex_cascade(
        &v, &v_and, vaultdb::expression::ExpressionId::AND);
    auto v_cascade = ex_cascade.execute();
    vaultdb::expression::Expression ex2(&comparator, &v_cascade,
                                        vaultdb::expression::ExpressionId::OR);
    comparator = ex2.execute();
    vaultdb::expression::Expression ex_and(
        t->GetTuple(t1)->GetField(idx)->GetValue(),
        t->GetTuple(t2)->GetField(idx)->GetValue(),
        vaultdb::expression::ExpressionId::EQUAL);
    v_and = ex_and.execute();
  }
  SwapTuples(t1, t2, t, &comparator);
}

/** The procedure BitonicMerge recursively sorts a bitonic sequence in
 * ascending order, if dir = ASCENDING, and in descending order
 * otherwise. The sequence to be sorted starts at index position lo,
 * the number of elements is cnt.
 **/
void BitonicMerge(int lo, int n, QueryTable *t, SortDef &s, bool dir) {
  if (n > 1) {
    int m = power_of_two_less_than(n);
    for (int i = lo; i < lo + n - m; i++) {
      Compare(i, i + m, t, s, dir);
    }
    BitonicMerge(lo, m, t, s, dir);
    BitonicMerge(lo + m, n - m, t, s, dir);
  }
}

/** Procedure bitonicSort first produces a bitonic sequence by
 * recursively sorting its two halves in opposite directions, and then
 * calls bitonicMerge.
 **/
void BitonicSort(int lo, int cnt, QueryTable *t, SortDef &s, bool dir) {
  if (cnt > 1) {
    int k = cnt / 2;
    BitonicSort(lo, k, t, s, !dir);
    BitonicSort(lo + k, cnt - k, t, s, dir);
    BitonicMerge(lo, cnt, t, s, dir);
  }
}
void Sort(QueryTable *input, SortDef &s) {
  bool dir = s.order == SortOrder::ASCENDING ? true : false;
  BitonicSort(0, input->GetNumTuples(), input, s, dir);
}
