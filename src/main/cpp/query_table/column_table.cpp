#include <memory>
#include <utility>
#include <util/data_utilities.h>
#include <util/type_utilities.h>
#include <util/field_utilities.h>
#include <util/logger.h>
#include "column_table.h"
#include "plain_tuple.h"
#include "secure_tuple.h"
#include <operators/sort.h>
#include <util/system_configuration.h>



using namespace vaultdb;


template <typename B>
ColumnTable<B>::ColumnTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition & sort_def)
        : QueryTable<B>(tuple_cnt, schema, sort_def) {

    if(tuple_cnt == 0)
        return;

    int write_size;
    for(auto col_entry : this->field_sizes_bytes_) {
        vector<int8_t> fields;
        write_size = tuple_cnt * col_entry.second;
        fields.resize(write_size);
        memset(fields.data(), 0, write_size);
        column_data_[col_entry.first] = fields;
    }

    // initialize dummy tags to true
    B dummy_tag = B(true);
    B *dummy_tags = (B *) getFieldPtr(0, -1);

    for(int i = 0; i < tuple_cnt; ++i) {
        *dummy_tags = dummy_tag;
        ++dummy_tags;
    }
}

template <typename B>
ColumnTable<B>::ColumnTable(const QueryTable<B> &s) : QueryTable<B>(s) {
    // only support copy constructor on tables with same storage
    assert(s.storageModel() == StorageModel::COLUMN_STORE);
    ColumnTable<B> src = ((ColumnTable<B>) s);

    for(auto col_entry : src.column_data_) {
        this->column_data_[col_entry.first] = col_entry.second;
    }

}





// iterate over all tuples and produce one long bit array for encrypting/decrypting in emp
// only tested in PUBLIC or XOR mode
template <typename B>
vector<int8_t> ColumnTable<B>::serialize() const {
    // copy out our payload
    int dst_size = this->tuple_size_bytes_ * this->tuple_cnt_ + 1;
    vector<int8_t> dst(dst_size);

    int8_t *write_ptr = dst.data();

    *write_ptr = (int8_t) StorageModel::COLUMN_STORE;
    ++write_ptr;

    int write_size;
    for(int i = 0; i < this->schema_.getFieldCount(); ++i) {
        write_size = this->field_sizes_bytes_.at(i) * this->tuple_cnt_;
        memcpy(write_ptr, this->column_data_.at(i).data(), write_size);
        write_ptr += write_size;
    }

    // dummy tag
    memcpy(write_ptr, this->column_data_.at(-1).data(), this->field_sizes_bytes_.at(-1) * this->tuple_cnt_);

    return dst;
}




template <typename B>
QueryTable<B> & ColumnTable<B>::operator=(const QueryTable<B> & s) {
    assert(s.storageModel() == this->storageModel());
    auto src = (ColumnTable<B> *) &s;

    if(&s == this)
        return *this;

    this->setSchema(src->getSchema());

    column_data_.clear();

    for(auto pos : src->column_data_) {
        column_data_[pos.first] = pos.second;
    }

    return *this;
}








// up to two-way secret share - both Alice and Bob providing private inputs
template<typename B>
SecureTable *ColumnTable<B>::secretShare()  {
    SystemConfiguration & conf = SystemConfiguration::getInstance();
    return conf.emp_manager_->secretShare((PlainTable *) this);


}





// use this for acting as a data sharing party in the PDF
// generates alice and bob's shares and returns the pair
template<typename B>
SecretShares ColumnTable<B>::generateSecretShares() const {
    vector<int8_t> serialized = this->serialize();
    int8_t *secrets = serialized.data();
    size_t sharesSize = serialized.size();

    vector<int8_t> aliceShares, bobShares;
    aliceShares.resize(sharesSize);
    bobShares.resize(sharesSize);
    int8_t *alice = aliceShares.data();
    int8_t *bob = bobShares.data();

    emp::PRG prg; // initializes with a random seed


    prg.random_data(alice, sharesSize);

    for(size_t i = 0; i < sharesSize; ++i) {
        bob[i] = alice[i] ^ secrets[i];
    }


    return SecretShares(aliceShares, bobShares);
}




template <typename B>
ColumnTable<B> *ColumnTable<B>::deserialize(const QuerySchema &schema, const vector<int8_t> & table_bytes) {
    const int8_t *read_ptr = table_bytes.data();

    // read first byte, confirm it was encoded for ColumnTable
    assert(*read_ptr == (int8_t) StorageModel::COLUMN_STORE);
    ++read_ptr;


    uint32_t row_size = (std::is_same_v<B, bool>) ? schema.size() / 8 : schema.size() * TypeUtilities::getEmpBitSize();
    uint32_t tuple_cnt =  (table_bytes.size() - 1) /  row_size;
    auto result = new ColumnTable<B>(tuple_cnt, schema);
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


template<typename B>
void ColumnTable<B>::resize(const size_t &cnt) {
    this->tuple_cnt_ = cnt;

    for(auto col_entry : column_data_) {
        column_data_[col_entry.first].resize(cnt * this->field_sizes_bytes_[col_entry.first]);
    }
}




template<typename B>
PlainTuple ColumnTable<B>::getPlainTuple(size_t idx) const {
    assert(!this->isEncrypted());

    PlainTuple p(const_cast<QuerySchema *>(&this->schema_));
    PlainTable *t = (PlainTable *) this;
    for(auto pos : this->field_sizes_bytes_) {
        Field f = t->getField(idx, pos.first);
        p.setField(pos.first, f);
    }
    return p;
}





template<typename B>
void ColumnTable<B>::assignField(const int &dst_row, const int &dst_col, const QueryTable<B> *s, const int &src_row,
                              const int &src_col) {

    assert(s->storageModel() == StorageModel::COLUMN_STORE);
    ColumnTable<B> *src = (ColumnTable<B> *) s;

    int read_size = src->field_sizes_bytes_[src_col];
    int8_t *src_field = src->getFieldPtr(src_row, src_col);
    int8_t *dst_field = getFieldPtr(dst_row, dst_col);
    memcpy(dst_field, src_field, read_size);


}

// conditional write
template<typename B>
void ColumnTable<B>::assignField(const emp::Bit & write, const int &dst_row, const int &dst_col, const QueryTable<B> *s, const int &src_row,
                                 const int &src_col) {

    assert(s->storageModel() == StorageModel::COLUMN_STORE);
    assert(this->isEncrypted()); // safe to cast to bits

    ColumnTable<B> *src = (ColumnTable<B> *) s;


    int read_size = src->field_sizes_bytes_[src_col];
    int field_bits = read_size / TypeUtilities::getEmpBitSize();

    Bit *read_ptr = (Bit *) (src->column_data_[src_col].data()  + read_size * src_row);
    Bit *write_ptr = (Bit *) (this->column_data_[dst_col].data() + read_size * dst_row);

    for(int i = 0; i < field_bits; ++i) {
        *write_ptr = emp::If(write, *read_ptr, *write_ptr);
        ++read_ptr;
        ++write_ptr;
    }

}

template<typename B>
void
ColumnTable<B>::cloneFields(const int &dst_row, const int &dst_col, const QueryTable<B> *src, const int &src_row, const int & src_col,
                         const int &col_cnt) {

    assert(src->storageModel() == StorageModel::COLUMN_STORE);

    int write_col = dst_col;
    int read_col = src_col;

    for(int i = 0; i < col_cnt; ++i) {
        this->assignField(dst_row, write_col, src, src_row, read_col);
        ++read_col;
        ++write_col;
    }
}



template<typename B>
void ColumnTable<B>::cloneRow(const int &dst_row, const int &dst_col, const QueryTable<B> *src, const int &src_row) {

    cloneFields(dst_row, dst_col, src, src_row, 0,src->getSchema().getFieldCount());
}

template<typename B>
void ColumnTable<B>::cloneRow(const bool &write, const int &dst_row, const int &dst_col, const QueryTable<B> *src,
                           const int &src_row) {
    if(write)
        cloneRow(dst_row, dst_col, src, src_row);

}

template<typename B>
void ColumnTable<B>::cloneRow(const Bit &write, const int &dst_row, const int &dst_col, const QueryTable<B> *s, const int &src_row) {


    assert(s->storageModel() == StorageModel::COLUMN_STORE);
    ColumnTable<B> *src = (ColumnTable<B> *) s;

    int write_col = dst_col;
    int read_col = 0;

    for(int i = 0; i < src->getSchema().getFieldCount(); ++i) {
        this->assignField(write, dst_row, write_col, src, src_row, read_col);
        ++read_col;
        ++write_col;
    }

}

// copy entire table to offset in QueryTable
template<typename B>
void ColumnTable<B>::cloneTable(const int & dst_row, QueryTable<B> *s) {
    assert(s->storageModel() == StorageModel::COLUMN_STORE);
    assert(s->getSchema() == this->getSchema());
    assert((s->getTupleCount() + dst_row) <= this->tuple_cnt_);

    ColumnTable<B> *src = (ColumnTable<B> *) s;
    int8_t *write_pos, *read_pos;

    // copy out entire cols
    for(auto pos : this->field_sizes_bytes_) {
        int col_id = pos.first;
        int field_size = pos.second;
        write_pos = column_data_.at(col_id).data() + dst_row * field_size;
        read_pos = src->column_data_.at(col_id).data();
        memcpy(write_pos, read_pos, field_size * src->getTupleCount());
    }

}

template<typename B>
void ColumnTable<B>::compareSwap(const bool &swap, const int &lhs_row, const int &rhs_row) {
    if(swap) {
        // iterating on column_data to cover dummy tag at -1
        for(auto pos : column_data_) {
            int col_id = pos.first;
            int8_t *l = this->getFieldPtr(lhs_row, col_id);
            int8_t *r = this->getFieldPtr(rhs_row, col_id);

            // swap in place
            for(int i = 0; i < this->field_sizes_bytes_[col_id]; ++i) {
                *l = *l ^ *r;
                *r = *r ^ *l;
                *l = *l ^ *r;

                ++l;
                ++r;
            }

        }
    }
}

template<typename B>
void ColumnTable<B>::compareSwap(const Bit &swap, const int &lhs_row, const int &rhs_row) {

    assert(this->isEncrypted());
    int col_cnt = this->schema_.getFieldCount();
    // iterating on column_data to cover dummy tag at -1
    // need to do this piecewise to avoid overhead we found in profiling from iterating over table
    for(int col_id = 0; col_id < col_cnt; ++col_id) {

        int field_len = this->schema_.fields_.at(col_id).size();

        Bit *l = (Bit *) (column_data_.at(col_id).data() + lhs_row * this->field_sizes_bytes_.at(col_id));
        Bit *r =  (Bit *) (column_data_.at(col_id).data() + rhs_row * this->field_sizes_bytes_.at(col_id));

        // swap column bits in place
        // based on emp
        for(int i = 0; i < field_len; ++i) {
            Bit o = emp::If(swap, *l, *r);
            o ^= *r;
            *l ^= o;
            *r ^= o;

            ++l;
            ++r;
        }

    }
    // dummy tag
    Bit *l = (Bit *) (column_data_.at(-1).data() + lhs_row * this->field_sizes_bytes_.at(-1));
    Bit *r = (Bit *) (column_data_.at(-1).data() + rhs_row * this->field_sizes_bytes_.at(-1));
    Bit o = emp::If(swap, *l, *r);
    o ^= *r;
    *l ^= o;
    *r ^= o;


}


template <typename B>
bool ColumnTable<B>::operator==(const QueryTable<B> &other) const {

    assert(!this->isEncrypted()); // reveal this for tables in the clear
    assert(!other.isEncrypted());

    if(this->getSchema() != other.getSchema()) {

        std::cout << "Failed to match on schema: \n" << this->schema_ << "\n  == \n" << other.getSchema() << std::endl;
        return false;
    }

    if(this->getSortOrder() != other.getSortOrder()) {
        std::cout << "Failed to match on sort order expected="  << DataUtilities::printSortDefinition(this->getSortOrder())
                  << "observed=" << DataUtilities::printSortDefinition(other.getSortOrder()) <<  std::endl;
        return false;
    }

    if(this->getTupleCount() != other.getTupleCount()) {
        std::cout  << "Failed to match on tuple count " << this->getTupleCount() << " vs " << other.getTupleCount() << std::endl;
        return false;
    }


    auto *lhs =  (ColumnTable<bool> *) this;
    auto *rhs = (ColumnTable<bool> *) &other;



    for(uint32_t i = 0; i < this->getTupleCount(); ++i) {
        bool lhs_dummy_tag = lhs->getDummyTag(i);
        bool rhs_dummy_tag = rhs->getDummyTag(i);

        if(lhs_dummy_tag != rhs_dummy_tag) {
            std::cout  << "Comparing on idx " << i << " failed to match on dummy tag!" << std::endl;
            return false;
        }
        else if(!lhs->getDummyTag(i)) { // they match
                for(int j = 0; j < this->schema_.getFieldCount(); ++j) {
                    if(lhs->getField(i, j) != rhs->getField(i, j)) {
                        PlainTuple this_tuple = this->getPlainTuple(i);
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

template<typename B>
string ColumnTable<B>::getOstringStream() const {
    assert(!this->isEncrypted());
    stringstream ss;
    ss << this->schema_ << " isEncrypted? " << this->isEncrypted() <<  " order by: " << DataUtilities::printSortDefinition(this->getSortOrder()) << std::endl;
    ColumnTable<bool> *plain_table = (ColumnTable<bool> *) this;

    for(int i = 0; i < this->getTupleCount(); ++i) {
        ss <<  this->getPlainTuple(i);

        if(!plain_table->getDummyTag(i))
            ss << std::endl;
    }

    return ss.str();
}


template <typename B>
string ColumnTable<B>::toString(const bool & showDummies) const {

    std::ostringstream os;

    if(!showDummies) {
        return this->getOstringStream();
    }

    // show dummies case
    os << this->schema_ << " isEncrypted? " << this->isEncrypted() <<  " order by: " << DataUtilities::printSortDefinition(this->getSortOrder()) << std::endl;


    for(uint32_t i = 0; i < this->getTupleCount(); ++i) {
        PlainTuple tuple = getPlainTuple(i);
        os << tuple.toString(showDummies) << std::endl;
    }

    return os.str();

}


template<typename B>
std::string ColumnTable<B>::toString(const size_t &limit, const bool &show_dummies) const {
    std::ostringstream os;
    size_t tuples_printed = 0;
    size_t cursor = 0;

    assert(!this->isEncrypted());

    os << this->schema_ <<  " order by: " << DataUtilities::printSortDefinition(this->getSortOrder()) << std::endl;
    while((cursor < this->tuple_cnt_) && (tuples_printed < limit)) {
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








template class vaultdb::ColumnTable<bool>;
template class vaultdb::ColumnTable<emp::Bit>;
