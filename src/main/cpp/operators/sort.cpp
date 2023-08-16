#include "sort.h"
#include "project.h"
#include <query_table/plain_tuple.h>
// keep this file to ensure overloaded methods are visible
#include <query_table/secure_tuple.h>
#include <util/field_utilities.h>
#include <util/data_utilities.h>
#include <query_table/row_table.h>
#include <operators/support/normalize_fields.h>


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
        assert(Operator<B>::sort_definition_[0].first == -1); // Need to sort on dummy tag to ensure resizing does not delete real tuples

}

template<typename B>
QueryTable<B> *Sort<B>::runSelf() {
    QueryTable<B> *input = this->getChild()->getOutput();;

    this->start_time_ = clock_start();
    this->start_gate_cnt_ = this->system_conf_.andGateCount();

    if(SystemConfiguration::getInstance().storageModel() == StorageModel::ROW_STORE) {
        this->output_ = normalizeTable(input);
        this->output_->setSortOrder(this->sort_definition_);

        int counter = 0;
        bitonicSortNormalized(0, this->output_->getTupleCount(), true, counter);

        auto tmp = denormalizeTable(this->output_);
        delete this->output_;
        this->output_ = tmp;
        this->output_->setSortOrder(this->sort_definition_);
    }
    else {
        this->output_ = input->clone();
        int counter = 0;
        bitonicSort(0, this->output_->getTupleCount(), true, counter);
    }

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
        this->output_->resize(cutoff);
    }


    return this->output_;
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

template<typename B>
void Sort<B>::bitonicSortNormalized(const int &lo, const int &cnt, const bool &dir,  int & counter) {
    if (cnt > 1) {
        int m = cnt / 2;
        bitonicSortNormalized(lo, m, !dir,   counter);
        bitonicSortNormalized(lo + m, cnt - m, dir, counter);
        bitonicMergeNormalized(this->output_, this->sort_definition_, lo, cnt, dir, counter);
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
void Sort<B>::bitonicMergeNormalized( QueryTable<B> *table, const SortDefinition & sort_def, const int &lo, const int &n, const bool &dir,  int & counter) {

    if (n > 1) {
        int m = powerOfTwoLessThan(n);
        for (int i = lo; i < lo + n - m; ++i) {

            B to_swap =   swapTuplesNormalized(table, i, i+m, dir, sort_key_size_bits_);
            table->compareSwap(to_swap, i, i+m);
            ++counter;
        }

        bitonicMergeNormalized(table, sort_def, lo, m,  dir, counter);
        bitonicMergeNormalized(table, sort_def, lo + m, n - m, dir, counter);
    }
}



template<typename B>
B Sort<B>::swapTuples(const QueryTable<B> *table, const int &lhs_idx, const int &rhs_idx,
                      const SortDefinition &sort_definition, const bool &dir) {
    B swap = false;
    B not_init = true;

    for (size_t i = 0; i < sort_definition.size(); ++i) {
        bool asc = (sort_definition[i].second == SortDirection::ASCENDING);
        if(dir)
            asc = !asc;

        const Field<B> lhs_field = table->getPackedField(lhs_idx,sort_definition[i].first);
        const Field<B> rhs_field = table->getPackedField(rhs_idx,sort_definition[i].first);

        B eq = (lhs_field == rhs_field);

        B to_swap =  ((lhs_field < rhs_field) == asc);


        swap = FieldUtilities::select(not_init, to_swap, swap);
        not_init = not_init & eq;
    }


    return swap;

}

template <typename B>
Bit Sort<B>::swapTuplesNormalized(const QueryTable<Bit> *table, const int &lhs_idx, const int &rhs_idx, const bool &dir, const int & sort_key_width_bits) {

    if(SystemConfiguration::getInstance().emp_mode_ == EmpMode::OUTSOURCED) {
        return swapTuplesNormalizedOmpc(table, lhs_idx, rhs_idx, dir, sort_key_width_bits);
    }
    // caution: only works with row store
    RowTable<Bit> *row_table = (RowTable<Bit> *) table;

    vector<Bit> placeholder(sort_key_width_bits+1); // +1 for sign bit in case it is packed
    // placeholder to avoid initializing public value for Integer
    Integer lhs_key(placeholder);
    Integer rhs_key(placeholder);

    Bit *lhs = (Bit *) (row_table->tuple_data_.data() + row_table->tuple_size_bytes_ * lhs_idx);
    Bit *rhs = (Bit *) (row_table->tuple_data_.data() + row_table->tuple_size_bytes_ * rhs_idx);

    memcpy(lhs_key.bits.data(), lhs, sort_key_width_bits * TypeUtilities::getEmpBitSize());
    memcpy(rhs_key.bits.data(), rhs, sort_key_width_bits * TypeUtilities::getEmpBitSize());

    // set MSB to 1 for all to avoid losing MSBs that are zero
    lhs_key[sort_key_width_bits] = 1;
    rhs_key[sort_key_width_bits] = 1;

    return ((lhs_key > rhs_key) == dir);

}

template <typename B>
Bit Sort<B>::swapTuplesNormalizedOmpc(const QueryTable<Bit> *table, const int &lhs_idx, const int &rhs_idx, const bool &dir, const int & sort_key_width_bits) {

//        assert(table->storageModel() == StorageModel::ROW_STORE);

//    cout << "Swap tuples for (" << lhs_idx << ", " << rhs_idx << ")" << endl;
//    cout << "LHS: " << table->revealRow(lhs_idx, tmp).toString(true) << endl;
//    cout << "RHS: " << table->revealRow(rhs_idx, tmp).toString(true) << endl;
    int sort_col_cnt = table->getSortOrder().size();

    Integer lhs_key = FieldUtilities::unpackRow(table, lhs_idx, sort_col_cnt, sort_key_width_bits+1);
    Integer rhs_key = FieldUtilities::unpackRow(table, rhs_idx, sort_col_cnt, sort_key_width_bits+1);

    // set MSB to 1 for all to avoid losing MSBs that are zero
    lhs_key[sort_key_width_bits] = 1;
    rhs_key[sort_key_width_bits] = 1;
//    cout << "LHS key: " << FieldUtilities::printInt(lhs_key) << endl;
//    cout << "RHS key: " << FieldUtilities::printInt(rhs_key) << endl;

    return ((lhs_key > rhs_key) == dir);

}

template<typename B>
bool Sort<B>::swapTuplesNormalized(const QueryTable<bool> *table, const int &lhs_idx, const int &rhs_idx,
                                   const bool &dir, const int &sort_key_width_bits) {
    assert(table->storageModel() == StorageModel::ROW_STORE);
    auto row_table = (RowTable<bool> *) table;


    int8_t *lhs =  (row_table->tuple_data_.data() + row_table->tuple_size_bytes_ * lhs_idx);
    int8_t *rhs =  (row_table->tuple_data_.data() + row_table->tuple_size_bytes_ * rhs_idx);
    int byte_cnt = sort_key_width_bits / 8;

    // TODO: streamline this with memcmp over reversed keys
    bool lhs_gt = false;
    for(int i = byte_cnt - 1; i >= 0; --i) {
        auto l = (unsigned int) lhs[i];
        auto r = (unsigned int) rhs[i];

        if(l == r) continue;
        if(l > r) {
            lhs_gt = true;
            break;
        }
        break;
    }

    return  (lhs_gt == dir);
}


template<typename B>
string Sort<B>::getParameters() const {
    return DataUtilities::printSortDefinition(this->sort_definition_);
}


// project sort key to the front of table and normalize all fields
template<typename B>
QueryTable<B> *Sort<B>::normalizeTable(QueryTable<B> *src) {
    ExpressionMapBuilder<B> builder(src->getSchema());
    int write_cursor = 0;
    sort_key_size_bits_ = 0;
    // if one or more of the sort keys are floats, need to do a projection that creates a new column for sign_bit for each one
    // this is needed so we can reverse the process later

    vector<int> sort_cols;
    for(int i = this->sort_definition_.size()-1; i >= 0; --i) {
            auto key = this->sort_definition_[i];

            builder.addMapping(key.first, write_cursor);

            if (key.first != -1) {
                sort_cols.emplace_back(key.first);
                sort_key_map_[write_cursor] = key.first;
            }

            sort_key_size_bits_ += src->getSchema().getField(key.first).size();
            ++write_cursor;
        }
        std::reverse(this->sort_definition_.begin(), this->sort_definition_.end());

    for(int i = 0; i < src->getSchema().getFieldCount(); ++i) {
        if(std::find(sort_cols.begin(), sort_cols.end(),i) == sort_cols.end()) {
            builder.addMapping(i, write_cursor);
            sort_key_map_[write_cursor] = i;
            ++write_cursor;
        }
    }

    Project<B> projection(src->clone(), builder.getExprs());
    projection.setOperatorId(-2);

    auto projected = projection.run();
    auto dst = projected->clone();
    projected_schema_ = dst->getSchema();

    // normalize the fields for the sort key
    QuerySchema normed_schema = projected_schema_; // convert floats to int32s

    for(int i = 0; i < this->sort_definition_.size(); ++i) {
        QueryFieldDesc f = dst->getSchema().getField(i);
        f.setName(f.getTableName(), f.getName() + "_normed"); // to sidestep bit packing metadata

        if(f.getType() == FieldType::FLOAT) {
            f = QueryFieldDesc(f, FieldType::INT);
            normed_schema.putField(f);
        }
        if(f.getType() == FieldType::SECURE_FLOAT) {
            f = QueryFieldDesc(f, FieldType::SECURE_INT);
            normed_schema.putField(f);
        }
    }

    normed_schema.initializeFieldOffsets();
    dst->setSchema(normed_schema);

    // normalize the fields for the sort key
    for(int i = 0; i < dst->getTupleCount(); ++i) {
        for(int j = 0; j < this->sort_definition_.size(); ++j)  {
            Field<B> s = projected->getPackedField(i, j);
            Field<B> d = NormalizeFields::normalize(s, this->sort_definition_[j].second, normed_schema.getField(j).bitPacked());
            dst->setPackedField(i, j, d);

        }

    }

    return dst;
}


template<typename B>
QueryTable<B> *Sort<B>::denormalizeTable(QueryTable<B> *src) {
    // denormalize the fields for the sort key
    auto dst = src->clone();

    QuerySchema dst_schema = projected_schema_;
    dst->setSchema(dst_schema);
    for(int i = 0; i < src->getTupleCount(); ++i) {
        for(int j = 0; j < this->sort_definition_.size(); ++j)  {
            Field<B> s = src->getPackedField(i, j);
            Field<B> d = NormalizeFields::denormalize(s, dst_schema.getField(j).getType(), this->sort_definition_[j].second, dst_schema.getField(j).bitPacked());
            dst->setPackedField(i, j, d);
        }
    }

    std::reverse(this->sort_definition_.begin(), this->sort_definition_.end());

    ExpressionMapBuilder<B> builder(src->getSchema());
    for(auto pos : sort_key_map_) {
        builder.addMapping(pos.first, pos.second);
    }
    Project<B> projection(dst->clone(), builder.getExprs());
    projection.setOperatorId(-2);
    return projection.run()->clone();

}


template class vaultdb::Sort<bool>;
template class vaultdb::Sort<emp::Bit>;
