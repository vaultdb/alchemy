#include <memory>
#include <utility>
#include <util/data_utilities.h>
#include <util/field_utilities.h>
#include <util/logger.h>
#include "plain_tuple.h"
#include <operators/sort.h>
#include <util/system_configuration.h>
#include "secure_tuple.h"
#include "column_table.h"
#include "packed_column_table.h"


using namespace vaultdb;

// iterate over all tuples and produce one long bit array for encrypting/decrypting in emp
// only tested in PUBLIC or XOR mode
template <typename B>
vector<int8_t> QueryTable<B>::serialize() const {

    int dst_size = tuple_size_bytes_ * tuple_cnt_;
    vector<int8_t> dst(dst_size);

    int8_t *write_ptr = dst.data();
    int write_size;
    for(int i = 0; i < schema_.getFieldCount(); ++i) {
        write_size = field_sizes_bytes_.at(i) * tuple_cnt_;
        memcpy(write_ptr, column_data_.at(i).data(), write_size);
        write_ptr += write_size;
    }

    // dummy tag
    write_size =  field_sizes_bytes_.at(-1) * tuple_cnt_;
    memcpy(write_ptr, column_data_.at(-1).data(), write_size);
    return dst;
}




template <typename B>
QueryTable<B> & QueryTable<B>::operator=(const QueryTable<B> & s) {
    auto src = (QueryTable<B> *) &s;

    if(&s == this)
        return *this;

    setSchema(src->getSchema());

    column_data_.clear();

    for(auto pos : src->column_data_) {
        column_data_[pos.first] = pos.second;
    }

    return *this;
}





// up to two-way secret share - both Alice and Bob providing private inputs
template<typename B>
SecureTable *QueryTable<B>::secretShare()  {
    assert(!isEncrypted());
    SystemConfiguration & conf = SystemConfiguration::getInstance();
    return conf.emp_manager_->secretShare((PlainTable *) this);
}





// use this for acting as a data sharing party in the PDF
// generates alice and bob's shares and returns the pair
template<typename B>
SecretShares QueryTable<B>::generateSecretShares() const {
    assert(!isEncrypted());

    vector<int8_t> serialized = serialize();
    int8_t *secrets = serialized.data();
    size_t share_size = serialized.size();

    vector<int8_t> alice_shares, bob_shares;
    alice_shares.resize(share_size);
    bob_shares.resize(share_size);
    int8_t *alice = alice_shares.data();
    int8_t *bob = bob_shares.data();

    emp::PRG prg; // initializes with a random seed
    prg.random_data(alice, share_size);

    for(size_t i = 0; i < share_size; ++i) {
        bob[i] = alice[i] ^ secrets[i];
    }

    return SecretShares(alice_shares, bob_shares);
}




template <typename B>
QueryTable<B> *QueryTable<B>::deserialize(const QuerySchema &schema, const vector<int8_t> & table_bytes) {
    const int8_t *read_ptr = table_bytes.data();

    uint32_t row_size = (std::is_same_v<B, bool>) ? schema.size() / 8 : schema.size() * sizeof(emp::Bit);
    uint32_t tuple_cnt =  table_bytes.size() /  row_size;
    auto result = QueryTable<B>::getTable(tuple_cnt, schema);
    int write_size;

    for(int i = 0; i < schema.getFieldCount(); ++i) {
        write_size = result->field_sizes_bytes_[i] * tuple_cnt;
        result->column_data_[i].resize(write_size);
        memcpy(result->column_data_[i].data(), read_ptr, write_size);
        read_ptr += write_size;
    }

    // dummy tag
    write_size = result->field_sizes_bytes_[-1] * tuple_cnt;
    result->column_data_.at(-1).resize(write_size);
    memcpy(result->column_data_.at(-1).data(), read_ptr, write_size);

    return result;

}

// only works with no wire packing
template <typename B>
QueryTable<B> *QueryTable<B>::deserialize(const QuerySchema &schema, const vector<Bit> & table_bits) {
    const Bit *read_ptr = table_bits.data();
    assert(!SystemConfiguration::getInstance().wire_packing_enabled_);
    bool is_plain = std::is_same_v<B, bool>;
    assert(!is_plain);

    uint32_t tuple_cnt =  table_bits.size() /  schema.size();
    auto result = QueryTable<B>::getTable(tuple_cnt, schema);
    int write_size;

    for(int i = 0; i < schema.getFieldCount(); ++i) {
        write_size = result->field_sizes_bytes_[i] * tuple_cnt;
        memcpy(result->column_data_[i].data(), read_ptr, write_size);
        read_ptr += (write_size / sizeof(emp::Bit));
    }

    // dummy tag
    write_size = result->field_sizes_bytes_[-1] * tuple_cnt;
    memcpy(result->column_data_.at(-1).data(), read_ptr, write_size);
    return result;
}







template<typename B>
PlainTuple QueryTable<B>::getPlainTuple(size_t idx) const {
    assert(!isEncrypted());

    PlainTuple p(const_cast<QuerySchema *>(&schema_));
    PlainTable *t = (PlainTable *) this;
    for(auto pos : field_sizes_bytes_) {
        Field f = t->getField(idx, pos.first);
        p.setField(pos.first, f);
    }
    return p;
}



//
//template<>
//void QueryTable<bool>::cloneRow(const bool &write, const int &dst_row, const int &dst_col, const QueryTable<B> *src, const int &src_row) {
//    if(write) {
//        vector<int8_t> bytes;
//    }
//}

//template<typename B>
//void QueryTable<B>::cloneRow(const Bit &write, const int &dst_row, const int &dst_col, const QueryTable<B> *src, const int &src_row) {
//
//    assert(tuple_size_bytes_ >= src->tuple_size_bytes_);
//
//    if(SystemConfiguration::getInstance().wire_packing_enabled_) {
//        auto s = (QueryTable<Bit> *) src;
//        for(int i = 0; i < src->getSchema().getFieldCount(); ++i) {
//            SecureField src_field = s->getField(src_row, i);
//            SecureField dst_field = ((QueryTable<Bit> *) this)->getField(dst_row, dst_col + i);
//            dst_field = Field<Bit>::If(write, src_field, dst_field);
//            ((QueryTable<Bit> *) this)->setField(dst_row, dst_col + i, dst_field);
//        }
//
//        return;
//    }
//
//    //clone everything except dummy tag  - handle that separately to push to end
//    int src_size = src->tuple_size_bytes_ - src->field_sizes_bytes_.at(-1);
//
//    int cursor = 0; // bytes
//    int write_idx = dst_col; // field indexes
//    vector<int8_t> row = src->unpackRowBytes(src_row);
//
//    // re-pack row
//    while(cursor < src_size) {
//        Bit *write_pos = (Bit *) getFieldPtr(dst_row, write_idx);
//        Bit *read_pos = (Bit *) (row.data() + cursor);
//
//        int bytes_remaining = src_size - cursor;
//        int dst_len = field_sizes_bytes_.at(write_idx);
//        int to_read = (dst_len < bytes_remaining) ? dst_len : bytes_remaining;
//
//        int to_read_bits = to_read / sizeof(emp::Bit);
//        for(int i = 0; i <  to_read_bits; ++i) {
//            *write_pos = emp::If(write, *read_pos, *write_pos);
//            ++write_pos;
//            ++read_pos;
//        }
//        cursor += to_read;
//        ++write_idx;
//    }
//
//
//}
//



// copy entire table to offset in QueryTable
//template<typename B>
//void QueryTable<B>::cloneTable(const int & dst_row, QueryTable<B> *s) {
//    assert(s->getSchema() == getSchema());
//    assert((s->getTupleCount() + dst_row) <= tuple_cnt_);
//
//    QueryTable<B> *src = (QueryTable<B> *) s;
//    int8_t *write_pos, *read_pos;
//
//    // copy out entire cols
//    for(auto pos : field_sizes_bytes_) {
//        int col_id = pos.first;
//        int field_size = pos.second;
//        write_pos = getFieldPtr(dst_row, col_id);
//        read_pos = src->column_data_.at(col_id).data();
//        memcpy(write_pos, read_pos, field_size * src->getTupleCount());
//    }
//
//}

//template<>
//void QueryTable<bool>::compareSwap(const bool &swap, const int &lhs_row, const int &rhs_row) {
//    if(swap) {
//        // iterating on column_data to cover dummy tag at -1
//        for(auto pos : column_data_) {
//            int col_id = pos.first;
//            int8_t *l = getFieldPtr(lhs_row, col_id);
//            int8_t *r = getFieldPtr(rhs_row, col_id);
//
//            // swap in place
//            for(int i = 0; i < field_sizes_bytes_[col_id]; ++i) {
//                *l = *l ^ *r;
//                *r = *r ^ *l;
//                *l = *l ^ *r;
//
//                ++l;
//                ++r;
//            }
//
//        }
//    }
//}
//
//template<>
//void QueryTable<Bit>::compareSwap(const Bit &swap, const int &lhs_row, const int &rhs_row) {
//
//
//    if(SystemConfiguration::getInstance().wire_packing_enabled_) {
//        assert(SystemConfiguration::getInstance().emp_mode_ == EmpMode::OUTSOURCED);
//
//        Integer lhs = unpackRow(lhs_row);
//        Integer rhs = unpackRow(rhs_row);
//
//        emp::swap(swap, lhs, rhs);
//
//        packRow(lhs_row, lhs);
//        packRow(rhs_row, rhs);
//
//        return;
//    }
//
//    int col_cnt = schema_.getFieldCount();
//    // iterating on column_data to cover dummy tag at -1
//    // need to do this piecewise to avoid overhead we found in profiling from iterating over table
//    for(int col_id = 0; col_id < col_cnt; ++col_id) {
//
//        int field_len = schema_.fields_.at(col_id).size();
//
//        Bit *l = (Bit *) getFieldPtr(lhs_row, col_id);
//        Bit *r = (Bit *) getFieldPtr(rhs_row, col_id);
//
//        // swap column bits in place
//        // based on emp
//        for(int i = 0; i < field_len; ++i) {
//            Bit o = emp::If(swap, *l, *r);
//            o ^= *r;
//            *l ^= o;
//            *r ^= o;
//
//            ++l;
//            ++r;
//        }
//
//    }
//    // dummy tag
//    Bit *l = (Bit *)  getFieldPtr(lhs_row, -1);
//    Bit *r = (Bit *) getFieldPtr(rhs_row, -1);
//    Bit o = emp::If(swap, *l, *r);
//    o ^= *r;
//    *l ^= o;
//    *r ^= o;
//
//
//}
//

template <typename B>
bool QueryTable<B>::operator==(const QueryTable<B> &other) const {

    assert(!isEncrypted()); // reveal this for tables in the clear
    assert(!other.isEncrypted());

    if(schema_ != other.getSchema()) {

        std::cout << "Failed to match on schema: \n" << schema_ << "\n  == \n" << other.getSchema() << std::endl;
        return false;
    }

    if(order_by_ != other.order_by_) {
        std::cout << "Failed to match on sort order expected="  << DataUtilities::printSortDefinition(order_by_)
                  << "observed=" << DataUtilities::printSortDefinition(other.order_by_) <<  std::endl;
        return false;
    }

    if(tuple_cnt_ != other.tuple_cnt_) {
        std::cout  << "Failed to match on tuple count " << tuple_cnt_ << " vs " << other.tuple_cnt_ << std::endl;
        return false;
    }


    auto *lhs =  (QueryTable<bool> *) this;
    auto *rhs = (QueryTable<bool> *) &other;



    for(uint32_t i = 0; i < tuple_cnt_; ++i) {
        bool lhs_dummy_tag = lhs->getDummyTag(i);
        bool rhs_dummy_tag = rhs->getDummyTag(i);

        if(lhs_dummy_tag != rhs_dummy_tag) {
            std::cout  << "Comparing on idx " << i << " failed to match on dummy tag!" << std::endl;
            return false;
        }
        else if(!lhs->getDummyTag(i)) { // they match
                for(int j = 0; j < schema_.getFieldCount(); ++j) {
                    if(lhs->getField(i, j) != rhs->getField(i, j)) {
                        PlainTuple this_tuple = getPlainTuple(i);
                        PlainTuple other_tuple = other.getPlainTuple(i);
                        std::cout << "Comparing on idx " << i << " with " << this_tuple.toString(true)
                                  << "\n          !=            " << other_tuple.toString(true) << endl;
                        std::cout << "    Failed to match!" << std::endl;
                        return false;

                    }
                }
            }
        }


    return true;

}



template <typename B>
string QueryTable<B>::toString(const bool & show_dummies) const {

    assert(!isEncrypted());
    std::ostringstream os;
    os << schema_ << " isEncrypted? " << isEncrypted() <<  " order by: " << DataUtilities::printSortDefinition(order_by_) << std::endl;

    PlainTable *plain_table = (PlainTable *) this;
    for(uint32_t i = 0; i < tuple_cnt_; ++i) {
            os <<  getPlainTuple(i).toString(show_dummies);

            if(!plain_table->getDummyTag(i)  || show_dummies)
                os << std::endl;
    }

    return os.str();

}


template<typename B>
std::string QueryTable<B>::toString(const size_t &limit, const bool &show_dummies) const {
    std::ostringstream os;
    size_t tuples_printed = 0;
    size_t cursor = 0;

    assert(!isEncrypted());

    os << schema_ <<  " order by: " << DataUtilities::printSortDefinition(order_by_) << std::endl;
    while((cursor < tuple_cnt_) && (tuples_printed < limit)) {
        PlainTuple tuple = getPlainTuple(cursor);
        if(show_dummies  // print unconditionally
           || !tuple.getDummyTag()) {
            os << cursor << ": " << tuple.toString(show_dummies) << std::endl;
            ++tuples_printed;
        }
        ++cursor;
    }

    return os.str();

}






template<typename B>
PlainTable *QueryTable<B>::reveal(const int &party) {
    {
        if(!isEncrypted()) { return (QueryTable<bool> *) this;  }
        auto table = (QueryTable<Bit> *) this;
        QuerySchema dst_schema = QuerySchema::toPlain(schema_);
        SortDefinition collation = table->order_by_;

        // if it is not sorted so that the dummies are at the end, sort it now.
        if(collation.empty() || collation[0].first != -1) {
            SortDefinition tmp{ColumnSort(-1, SortDirection::ASCENDING)};
            for(int i = 0; i < collation.size(); ++i) {
                tmp.emplace_back(collation[i]);
            }
            Sort sort(((QueryTable<Bit> *) this)->clone(), tmp);
            sort.setOperatorId(-2);
            table = sort.getOutput()->clone();
        }

        // count # of real (not dummy) rows
        int row_cnt = 0;
        while((row_cnt < table->tuple_cnt_) &&
              !table->getDummyTag(row_cnt).reveal()) {
            ++row_cnt;
        }

        auto dst_table = new ColumnTable<bool>(row_cnt, dst_schema, collation);

        for(int i = 0; i < row_cnt; ++i)  {
            PlainTuple dst_tuple = table->revealRow(i, dst_schema, party);
            dst_table->putTuple(i, dst_tuple);
        }

        if(table != (QueryTable<Bit> *) this) // extra sort
            delete table;

        return dst_table;
    }
}


template<typename B>
PlainTable *QueryTable<B>::revealInsecure(const int &party) const {

    if(!isEncrypted()) {
        return (QueryTable<bool> *) this;
    }

    QuerySchema dst_schema = QuerySchema::toPlain(schema_);

    auto dst_table = PlainTable::getTable(tuple_cnt_, dst_schema, order_by_);

    for(uint32_t i = 0; i < tuple_cnt_; ++i)  {
        PlainTuple t = revealRow(i, dst_schema, party);
        dst_table->putTuple(i, t);
    }
    return dst_table;

}



template<typename B>
QueryTable<B> *QueryTable<B>::getTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition &sort_def) {
    if(std::is_same_v<B, Bit> && SystemConfiguration::getInstance().wire_packing_enabled_) {
        return (QueryTable<B> *) new PackedColumnTable(tuple_cnt, schema, sort_def);
    }
    else return new ColumnTable<B>(tuple_cnt, schema, sort_def);
}

template class vaultdb::QueryTable<bool>;
template class vaultdb::QueryTable<emp::Bit>;
