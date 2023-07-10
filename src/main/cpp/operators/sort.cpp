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
Sort<B>::Sort(Operator<B> *child, const SortDefinition &sort_def, const int & limit)
   : Operator<B>(child, sort_def), limit_(limit) {
    this->output_cardinality_ = child->getOutputCardinality();

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
Sort<B>::Sort(QueryTable<B> *child, const SortDefinition &sort_def, const int & limit)
  : Operator<B>(child, sort_def), limit_(limit) {
    this->output_cardinality_ = child->getTupleCount();

    for(ColumnSort s : Operator<B>::sort_definition_) {
        if(s.second == SortDirection::INVALID)
            throw; // invalid sort definition
    }

    if(limit_ > 0)
        assert(Operator<B>::sort_definition_[0].first == -1); // Need to sort on dummy tag to make resizing not delete real tuples

}

template<typename B>
QueryTable<B> *Sort<B>::runSelf() {
    QueryTable<B> *input = Operator<B>::getChild()->getOutput();;

    this->start_time_ = clock_start();
    this->start_gate_cnt_ = this->system_conf_.andGateCount();

    cout << "Sorting on " << DataUtilities::printSortDefinition(this->sort_definition_) << endl;
    // deep copy new output
    Operator<B>::output_ = input->clone();
    int counter = 0;
    int n = this->output_->getTupleCount();
    bitonicSort(0, this->output_->getTupleCount(), true, counter);
    auto end_gates = this->system_conf_.andGateCount();
    auto total_gates = end_gates - this->start_gate_cnt_;
//    cout << "Sorted on " << this->output_->getTupleCount() << " rows, have " << counter << " compare & swap ops in " << total_gates << " gates."   << endl;

    float rounds = log2(n);
    float stages = (rounds * (rounds + 1))/2.0;

    float comparisons_per_stage = n / 2.0;
    float total_comparisons = stages * comparisons_per_stage;
    cout << "Estimated comparisons: " << total_comparisons << ", actual comparisons: " << counter << endl;

    Operator<B>::output_->setSortOrder(Operator<B>::sort_definition_);

    // implement LIMIT
    if(limit_ > 0) {
        size_t cutoff = limit_;

        // can't have more tuples than are initialized
        if(cutoff > Operator<B>::output_->getTupleCount())
            cutoff = Operator<B>::output_->getTupleCount();

        // if in plaintext, truncate to true length or limit_, whichever one is lower
        if(std::is_same_v<B, bool>) {
            int first_dummy = -1;
            uint32_t cursor = 0;
            while(first_dummy < 0 && cursor < Operator<B>::output_->getTupleCount()) {
                if(Operator<B>::output_->getPlainTuple(cursor).getDummyTag()) {
                    first_dummy = cursor; // break
                }
                ++cursor;
            }

            if(first_dummy > 0 && first_dummy < limit_)
                cutoff = first_dummy;
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
void Sort<B>::bitonicSort(const int &lo, const int &cnt, const bool &dir,  int & counter) {
    if (cnt > 1) {
        int m = cnt / 2;
        bitonicSort(lo, m, !dir,   counter);
        bitonicSort(lo + m, cnt - m, dir, counter);
        bitonicMerge(this->output_, this->sort_definition_, lo, cnt, dir, counter);
    }

}


/** The procedure BitonicMerge recursively sorts a bitonic sequence in
 * ascending order, if dir = ASCENDING, and in descending order
 * otherwise. The sequence to be sorted starts at index position lo,
 * the number of elements is cnt.
 **/
template<typename B>
void Sort<B>::bitonicMerge( QueryTable<B> *table, const SortDefinition & sort_def, const int &lo, const int &n, const bool &dir,  int & counter) {

    if (n > 1) {
        int m = powerOfTwoLessThan(n);
        for (int i = lo; i < lo + n - m; ++i) {
            B to_swap = swapTuples(table, i, i+m, sort_def, dir);
            table->compareSwap(to_swap, i, i+m);
            ++counter;
        }

        bitonicMerge(table, sort_def, lo, m,  dir, counter);
        bitonicMerge(table, sort_def, lo + m, n - m, dir, counter);
    }
}




template<typename B>
B Sort<B>::swapTuples(const QueryTable<B> *table, const int &lhs_idx, const int &rhs_idx,
                      const SortDefinition &sort_definition, const bool &dir) {
    B swap = false;
    B not_init = true;


    for (size_t i = 0; i < sort_definition.size(); ++i) {
//        auto start_ands = SystemConfiguration::getInstance().andGateCount();
        bool asc = (sort_definition[i].second == SortDirection::ASCENDING);
        if(dir)
            asc = !asc;


        const Field<B> lhs_field = table->getPackedField(lhs_idx,sort_definition[i].first);
        const Field<B> rhs_field = table->getPackedField(rhs_idx,sort_definition[i].first);

        B eq = (lhs_field == rhs_field);

//        auto eq_gates = SystemConfiguration::getInstance().andGateCount() - start_ands;

        B to_swap =  ((lhs_field < rhs_field) == asc);

//        auto lt_gates = SystemConfiguration::getInstance().andGateCount() - start_ands - eq_gates;

        swap = FieldUtilities::select(not_init, to_swap, swap);
        not_init = not_init & eq;
//        auto bookkeeping_gates = SystemConfiguration::getInstance().andGateCount() - start_ands - eq_gates - lt_gates;
//       if(lhs_idx== 0 && rhs_idx == 1) {
//            cout << "For sort comparison over " << table->getSchema().getField(sort_definition[i].first)
//            << "  have " << eq_gates + bookkeeping_gates  + lt_gates << " AND gates, eq="
//            << eq_gates << " lt=" << lt_gates << ", bookkeeping: " << bookkeeping_gates <<  endl;
//        }
    }

//    if(lhs_idx== 0 && rhs_idx == 1) {
//        cout << endl;
//    }

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
