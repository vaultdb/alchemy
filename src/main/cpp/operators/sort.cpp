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
        assert(Operator<B>::sort_definition_[0].first == -1); // Need to sort on dummy tag to make resizing not delete real tuples

}

template<typename B>
QueryTable<B> *Sort<B>::runSelf() {
    QueryTable<B> *input = Operator<B>::getChild()->getOutput();;

    this->start_time_ = clock_start();
    this->start_gate_cnt_ = this->system_conf_.andGateCount();


    if(SystemConfiguration::getInstance().storageModel() == StorageModel::ROW_STORE) {
        this->output_ = normalizeTable(input, this->sort_definition_);
        int counter = 0;
        bitonicSortNormalized(0, this->output_->getTupleCount(), true, counter);

        auto tmp = denormalizeTable(this->output_, this->sort_definition_);
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
            B to_swap = swapTuplesNormalized(table, i, i+m, dir, sort_key_size_bits_);
            table->compareSwap(to_swap, i, i+m);
            if(FieldUtilities::extract_bool(to_swap))
                cout << "After swap: \n" << *(table->revealInsecure()) << endl;
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

  //    assert(table->storageModel() == StorageModel::ROW_STORE);
    QuerySchema dst_tmp = QuerySchema::toPlain(table->getSchema());

    RowTable<Bit> *row_table = (RowTable<Bit> *) table;

    vector<Bit> placeholder(sort_key_width_bits+1); // +1 for sign bit in case it is packed
    // placeholder to avoid initializing public value for Integer
    Integer lhs_key(placeholder);
    Integer rhs_key(placeholder);

    PlainTuple l_tmp = table->revealRow(lhs_idx, dst_tmp);
    PlainTuple r_tmp = table->revealRow(rhs_idx, dst_tmp);
    cout << "Comparing rows: (" << lhs_idx << ", " << rhs_idx << ", " << (dir ? "ASC" : "DESC")
         << "): "<< l_tmp << ", " << r_tmp << ", sort key width=" << sort_key_width_bits <<  endl;



    Bit *lhs = (Bit *) (row_table->tuple_data_.data() + row_table->tuple_size_ * lhs_idx);
    Bit *rhs = (Bit *) (row_table->tuple_data_.data() + row_table->tuple_size_ * rhs_idx);

    memcpy(lhs_key.bits.data(), lhs, sort_key_width_bits * TypeUtilities::getEmpBitSize());
    memcpy(rhs_key.bits.data(), rhs, sort_key_width_bits * TypeUtilities::getEmpBitSize());

    // set MSB to 1 for all to avoid losing MSBs that are zero
    lhs_key[sort_key_width_bits] = 1;
    rhs_key[sort_key_width_bits] = 1;
    //reverse 1 bit at a time
    // if this works, TODO: put in project instead of 1 comparison at a time
//    for(int i = 0; i < sort_key_width_bits; ++i) {
//        lhs_key.bits[i] = lhs[sort_key_width_bits - i - 1];
//        rhs_key.bits[i] = rhs[sort_key_width_bits - i - 1];
//    }

    cout << "LHS starting bits: ";
    for(int i = 0; i < sort_key_width_bits; ++i) {
        cout << lhs[i].reveal();
    }
    cout << endl;
    cout << "RHS starting bits: ";
    for(int i = 0; i < sort_key_width_bits; ++i) {
        cout << rhs[i].reveal();
    }
    cout << endl;

    Bit key_cmp = (lhs_key > rhs_key);
    Bit res = ((lhs_key > rhs_key) == dir);
    if((l_tmp != r_tmp)) {
        bool r2 = res.reveal();
        cout << ">To swap? " << r2 << endl;
        cout << ">(lhs > rhs): " << key_cmp.reveal() << " with dir " << dir << endl;
        cout << ">lhs orderyear: " << FieldUtilities::printField(table->getPackedField(lhs_idx, 0)) << ", orderkey: " << FieldUtilities::printField(table->getPackedField(lhs_idx, 1)) << endl;
        cout << ">rhs orderyear: " << FieldUtilities::printField(table->getPackedField(rhs_idx, 0)) << ", orderkey: " << FieldUtilities::printField(table->getPackedField(rhs_idx, 1)) << endl;
        cout << ">order year comparison (lhs > rhs): " << (table->getPackedField(lhs_idx, 1) > table->getPackedField(rhs_idx, 1)).reveal() << endl;
        cout << ">LHS: " << FieldUtilities::printInt(lhs_key) << endl;
        cout << ">RHS: " << FieldUtilities::printInt(rhs_key) << endl;

    }

    return res;

}

// potential bug:
// Comparing rows: (0, 2, ASC): (8192, 1996, 1996, 1), (8190, 1993, 1993, 3), sort key width=16
//LHS starting bits: 1111111111111001
//RHS starting bits: 1011111111111100
//>To swap? 0
//>(lhs > rhs): 0 with dir 1
//>lhs orderyear: 8191: 11111111111110, (-1, 31, 0, 0), orderkey: 4: 0010, (4, 0, 0, 0)
//>rhs orderyear: 8189: 10111111111110, (-3, 31, 0, 0), orderkey: 1: 1000, (1, 0, 0, 0)
//>LHS: 40959: 1111111111111001, (-1, -97, 0, 0)
//>RHS: 16381: 1011111111111100, (-3, 63, 0, 0)

template<typename B>
bool Sort<B>::swapTuplesNormalized(const QueryTable<bool> *table, const int &lhs_idx, const int &rhs_idx,
                                   const bool &dir, const int &sort_key_width_bits) {
    assert(table->storageModel() == StorageModel::ROW_STORE);
    RowTable<bool> *row_table = (RowTable<bool> *) table;

    int8_t *lhs = row_table->tuple_data_.data() + row_table->tuple_size_ * lhs_idx;
    int8_t *rhs = row_table->tuple_data_.data() + row_table->tuple_size_ * rhs_idx;
    int res = std::memcmp(lhs, rhs, sort_key_width_bits/8);

    // < 0: lhs < rhs
    // >= 0: lhs >= rhs
    return ((res > 0) == dir);
}


template<typename B>
string Sort<B>::getOperatorType() const {
    return "Sort";
}

template<typename B>
string Sort<B>::getParameters() const {
    return DataUtilities::printSortDefinition(this->sort_definition_);
}


// project sort key to the front of table and normalize all fields
template<typename B>
QueryTable<B> *Sort<B>::normalizeTable(QueryTable<B> *src, SortDefinition &sort_def) {
    ExpressionMapBuilder<B> builder(src->getSchema());
    int write_cursor = 0;
    sort_key_size_bits_ = 0;

    vector<int> sort_cols;
    if(std::is_same_v<B, bool>) {
        for (auto key: sort_def) {
            builder.addMapping(key.first, write_cursor);
            if (key.first != -1) {
                sort_cols.emplace_back(key.first);
                sort_key_map_[write_cursor] = key.first;
            }

            sort_key_size_bits_ += src->getSchema().getField(key.first).size();
            ++write_cursor;
        }
    } else { // reverse order of sort keys for Bits
        for(int i = sort_def.size()-1; i >= 0; --i) {
            auto key = sort_def[i];

            builder.addMapping(key.first, write_cursor);
            if (key.first != -1) {
                sort_cols.emplace_back(key.first);
                sort_key_map_[write_cursor] = key.first;
            }

            sort_key_size_bits_ += src->getSchema().getField(key.first).size();
            ++write_cursor;
        }
        std::reverse(sort_def.begin(), sort_def.end());
    }

    for(int i = 0; i < src->getSchema().getFieldCount(); ++i) {
        if(std::find(sort_cols.begin(), sort_cols.end(),i) == sort_cols.end()) {
            builder.addMapping(i, write_cursor);
            sort_key_map_[write_cursor] = i;
            ++write_cursor;
        }
    }

    // temporarily put original row after normalized one for debugging
    // TODO: remove this for loop
    for(int i = 0; i < src->getSchema().getFieldCount(); ++i) {
            builder.addMapping(i, write_cursor);
            ++write_cursor;
    }

    Project<B> projection(src->clone(), builder.getExprs());
    projection.setOperatorId(-2);

    auto projected = projection.run();
    auto dst = projected->clone();
    projected_schema_ = dst->getSchema();
    // normalize the fields for the sort key
    QuerySchema normed_schema = projected_schema_; // convert floats to int32s

    for(int i = 0; i < sort_def.size(); ++i) {
        QueryFieldDesc f = dst->getSchema().getField(i);
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
    // TEMP VARS
    RowTable<B> *rows = (RowTable<B> *) dst;
    QuerySchema plain_schema = QuerySchema::toPlain(dst->getSchema());

    // normalize the fields for the sort key
    for(int i = 0; i < dst->getTupleCount(); ++i) {
        cout << rows->revealRow(i, plain_schema, PUBLIC) << endl;
        Bit *tmp = (Bit *) (rows->tuple_data_.data() + rows->tuple_size_ * i);
//        cout << "Row starts at offset: " << rows->tuple_size_ * i << endl; // 16 bytes / Bit, 17 Bits (2*(3+13)+1 (dt)) per row = 528 bytes / row
//        cout << "Before starting bits: ";
//        for(int j = 0; j < this->sort_key_size_bits_; ++j) {
//            cout << tmp[j].reveal();
//        }
//        cout << endl;

        for(int j = 0; j < sort_def.size(); ++j)  {
            Field<B> s = projected->getPackedField(i, j);
            Field<B> d = NormalizeFields::normalize(s, sort_def[j].second, normed_schema.getField(j).bitPacked());
//            cout << "Received packed field: " << FieldUtilities::printField(d) << endl;
            dst->setPackedField(i, j, d);

//            cout << "During starting bits " << j << ": ";
//            for(int j = 0; j < this->sort_key_size_bits_; ++j) {
//                cout << tmp[j].reveal();
//            }
//            cout << endl;
        }

//        cout << "After starting bits:  ";
//        for(int j = 0; j < this->sort_key_size_bits_; ++j) {
//            cout << tmp[j].reveal();
//        }
//        cout << endl << endl;

    }
    cout << "Normalized schema: " << normed_schema << endl;
    cout << "Normalized table: " << *(dst->revealInsecure()) << endl;

    return dst;
}

// say we have (2, 3, 4) and we want to sort in DESC order
// ordinarily they are 010, 011, 100) to get DESC we want to flip the bits
// e.g.,               101, 100, 011
// now ASC sort produces 011, 100, 101 - OK

template<typename B>
QueryTable<B> *Sort<B>::denormalizeTable(QueryTable<B> *src,  SortDefinition &sort_def) {
    // denormalize the fields for the sort key
    auto dst = src->clone();
    cout << "Normalized sorted table: " << *(dst->revealInsecure()) << endl;
    QuerySchema dst_schema = projected_schema_;
    dst->setSchema(dst_schema);
    for(int i = 0; i < src->getTupleCount(); ++i) {
        for(int j = 0; j < sort_def.size(); ++j)  {
            Field<B> s = src->getPackedField(i, j);
            Field<B> d = NormalizeFields::denormalize(s, dst_schema.getField(j).getType(), sort_def[j].second, dst_schema.getField(j).bitPacked());
            dst->setPackedField(i, j, d);
        }
    }

    cout << "Denormalized sorted table: " << *(dst->revealInsecure()) << endl;
    if(std::is_same_v<B, Bit>) {
        std::reverse(sort_def.begin(), sort_def.end());
    }

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
