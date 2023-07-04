#include <memory>
#include <utility>
#include <util/data_utilities.h>
#include <util/type_utilities.h>
#include <util/field_utilities.h>
#include <util/logger.h>
#include "row_table.h"
#include "plain_tuple.h"
#include "secure_tuple.h"
#include <operators/sort.h>
#include <util/system_configuration.h>


using namespace vaultdb;


template <typename B>
RowTable<B>::RowTable(const size_t &tuple_cnt, const QuerySchema &schema, const SortDefinition & sort_def)
         : QueryTable<B>(tuple_cnt, schema, sort_def) {

    if(tuple_cnt == 0)
        return;

    tuple_data_.resize(tuple_cnt * RowTable<B>::tuple_size_);

      

    std::memset(tuple_data_.data(), 0, tuple_data_.size());
    for(int i = 0; i < tuple_cnt; ++i) {
        setDummyTag(i, true);
    }

}

template <typename B>
RowTable<B>::RowTable(const QueryTable<B> &src) : QueryTable<B>(src) {
    // only support copy constructor on tables with same storage
    assert(src.storageModel() == StorageModel::ROW_STORE);
    tuple_data_ = ((RowTable<B> *) &src)->tuple_data_;

}






template <typename B>
PlainTable *RowTable<B>::reveal(const int & party)   {
    assert(this->storageModel() == StorageModel::ROW_STORE);

    if(!this->isEncrypted()) {
        return (RowTable<bool> *) this;
    }


    auto table = (RowTable<Bit> *) this;
    SortDefinition collation = table->getSortOrder();
    QuerySchema dst_schema = QuerySchema::toPlain(table->getSchema());

    // if it is not sorted so that the dummies are at the end, sort it now.
    if(collation.empty() || collation[0].first != -1) {
        SortDefinition tmp{ColumnSort(-1, SortDirection::ASCENDING)};
        for(int i = 0; i < collation.size(); ++i) {
            tmp.emplace_back(collation[i]);
        }
        Sort sort(this->clone(), tmp);
        table = (RowTable<Bit> *)  sort.run()->clone();
    }

    auto dst_table = new RowTable<bool>(this->tuple_cnt_, dst_schema, collation);
    int write_cursor = 0;


    for(uint32_t i = 0; i < table->tuple_cnt_; ++i)  {
        bool dummy_tag = table->getDummyTag(i).reveal();
        if(!dummy_tag) { // if real tuple (not a dummy), reveal it
            PlainTuple dst_tuple = table->revealRow(i, dst_schema, party);
            dst_table->putTuple(write_cursor, dst_tuple);
            ++write_cursor;
        }
        else {
             break;
        }
    }

    dst_table->resize(write_cursor);
    if(table != (RowTable<Bit> *) this) // extra sort
        delete table;

    return dst_table;

}

template<typename B>
PlainTable *RowTable<B>::revealInsecure(const int &party) {
    assert(this->storageModel() == StorageModel::ROW_STORE);

    if(!this->isEncrypted()) {
        return (RowTable<bool> *) this;
    }


    auto table = (RowTable<Bit> *) this;
    QuerySchema dst_schema = QuerySchema::toPlain(this->schema_);

    auto dst_table = new RowTable<bool>(this->tuple_cnt_, dst_schema, this->getSortOrder());

    for(uint32_t i = 0; i < this->tuple_cnt_; ++i)  {
            const SecureTuple tuple(&this->schema_, this->getFieldPtr(i, 0));
            PlainTuple dst_tuple = tuple.revealInsecure(&dst_schema, party);
            dst_table->putTuple(i, dst_tuple);
    }
    return dst_table;

}



// iterate over all tuples and produce one long bit array for encrypting/decrypting in emp
// only tested in PUBLIC or XOR mode
template <typename B>
vector<int8_t> RowTable<B>::serialize() const {
    // copy out the payload with 1 byte in front to signify it is a row store
    vector<int8_t> dst(tuple_data_.size() + 1);
    int8_t *write_ptr = dst.data();
    *write_ptr = (int8_t) StorageModel::ROW_STORE;
    ++write_ptr;
    memcpy(write_ptr, tuple_data_.data(), tuple_data_.size());

    return dst;
}




template <typename B>
QueryTable<B> & RowTable<B>::operator=(const QueryTable<B> & src) {
    assert(src.storageModel() == this->storageModel());

    if(&src == this)
        return *this;

    this->setSchema(src.getSchema());

    tuple_data_.resize(((RowTable<B> *) &src)->tuple_data_.size());
    memcpy(tuple_data_.data(), ((RowTable<B> *) &src)->tuple_data_.data(), tuple_data_.size());
    return *this;
}








// up to two-way secret share - both Alice and Bob providing private inputs
template<typename B>
SecureTable *RowTable<B>::secretShare()  {
    SystemConfiguration & conf = SystemConfiguration::getInstance();
    return conf.emp_manager_->secretShare((PlainTable *) this);
}





// use this for acting as a data sharing party in the PDF
// generates alice and bob's shares and returns the pair
template<typename B>
SecretShares RowTable<B>::generateSecretShares() const {
    vector<int8_t> serialized = this->serialize();
    int8_t *secrets = serialized.data();
    size_t share_size_bytes = serialized.size();

    vector<int8_t> alice_shares, bob_shares;
    alice_shares.resize(share_size_bytes);
    bob_shares.resize(share_size_bytes);
    int8_t *alice = alice_shares.data();
    int8_t *bob = bob_shares.data();

    emp::PRG prg; // initializes with a random seed

    prg.random_data(alice, share_size_bytes);

    for(size_t i = 0; i < share_size_bytes; ++i) {
        bob[i] = alice[i] ^ secrets[i];
    }


    return SecretShares(alice_shares, bob_shares);
}



template <typename B>
RowTable<B> *RowTable<B>::deserialize(const QuerySchema &schema, const vector<int8_t> & table_bytes) {

    const int8_t *read_ptr = table_bytes.data();
    assert(*read_ptr == (int8_t) StorageModel::ROW_STORE); // confirm this was encoded for row store
    ++read_ptr;


    uint32_t table_size = table_bytes.size() - 1; // in bytes
    uint32_t row_size = (std::is_same_v<B, bool>) ? schema.size() / 8 : schema.size() * TypeUtilities::getEmpBitSize(); // in bytes
    uint32_t tuple_cnt = table_size / row_size;
    assert(table_size % row_size == 0);

    auto result = new RowTable<B>(tuple_cnt, schema);
    memcpy(result->tuple_data_.data(), read_ptr, table_size);

    return result;
}

// only for use in SH2PC mode
template <typename B>
RowTable<B> *RowTable<B>::deserialize(const QuerySchema &schema, const vector<Bit> & table_bits) {

    // reveal first byte to confirm that we are reading a row store instance
    const Bit *read_ptr = table_bits.data();
    Integer i(8, 0, PUBLIC);
    memcpy(i.bits.data(), read_ptr, 8*TypeUtilities::getEmpBitSize());
    int storage_model = i.reveal<int>(PUBLIC);
    assert(storage_model == (int) StorageModel::ROW_STORE); // confirm this was encoded for row store
    read_ptr += 8;

    uint32_t table_size_bits = table_bits.size() - 8;
    uint32_t row_size_bits = schema.size();
    uint32_t tuple_cnt = table_size_bits / row_size_bits;

    assert(table_size_bits % row_size_bits == 0);

    auto result = new RowTable<B>(tuple_cnt, schema);
    memcpy(result->tuple_data_.data(), read_ptr, table_size_bits * TypeUtilities::getEmpBitSize());

    return result;

}

template<typename B>
void RowTable<B>::resize(const size_t &cnt) {
    this->tuple_data_.resize(cnt * this->tuple_size_);
    this->tuple_cnt_ = cnt;
}




template<typename B>
PlainTuple RowTable<B>::getPlainTuple(size_t idx) const {
    assert(!this->isEncrypted()); // B == bool
    int8_t *tuple_pos =  ((int8_t *) this->tuple_data_.data()) + this->tuple_size_ * idx;
    return PlainTuple(const_cast<QuerySchema *>(&this->schema_), tuple_pos);
}


//template<typename B>
//QueryTuple<Bit> RowTable<B>::getSecureTuple(size_t idx) const  {
//    assert(this->isEncrypted());
//    int8_t *tuple_pos =  ((int8_t *) this->tuple_data_.data()) + this->tuple_size_ * idx;
//    return SecureTuple(const_cast<QuerySchema *>(&this->schema_), tuple_pos);
//}





template<typename B>
void RowTable<B>::assignField(const int &dst_row, const int &dst_col, const QueryTable<B> *s, const int &src_row,
                                const int &src_col) {

    assert(s->storageModel() == StorageModel::ROW_STORE);
    RowTable<B> *src = (RowTable<B> *) s;

    int8_t *src_field = (int8_t *) (src->tuple_data_.data() + src->tuple_size_ * src_row + src->field_offsets_bytes_.at(src_col));
    int8_t *dst_field = (int8_t *) (tuple_data_.data() + this->tuple_size_ * dst_row + this->field_offsets_bytes_.at(dst_col));
    memcpy(dst_field, src_field, this->field_sizes_bytes_.at(dst_col));


}


template<typename B>
void
RowTable<B>::cloneFields(const int &dst_row, const int &dst_col, const QueryTable<B> *s, const int &src_row, const int & src_col,
                           const int &col_cnt) {

    assert(s->storageModel() == StorageModel::ROW_STORE);
    RowTable<B> *src = (RowTable<B> *) s;


    int8_t *src_ptr = (int8_t *) (src->tuple_data_.data() + src->tuple_size_ * src_row + src->field_offsets_bytes_.at(src_col));
    int8_t *dst_ptr = (int8_t *) (tuple_data_.data() + this->tuple_size_ * dst_row + this->field_offsets_bytes_.at(dst_col));

    int write_size = 0;
    int cursor = dst_col;
    for(int i = 0; i < col_cnt;  ++i) {
        write_size += this->field_sizes_bytes_[cursor];
        ++cursor;
    }

    memcpy(dst_ptr, src_ptr, write_size);
}


//template<typename B>
//void RowTable<B>::cloneFields(bool write, const int &dst_row, const int &dst_col, const QueryTable<B> *src,
//                                const int &src_row, const int &src_col, const int &col_cnt) {
//
//    if(write)
//        cloneFields(dst_row, dst_col, src, src_row, src_col, col_cnt);
//
//
//}
//
//
//template<typename B>
//void RowTable<B>::cloneFields(Bit write, const int &dst_row, const int &dst_col, const QueryTable<B> *s,
//                                const int &src_row, const int &src_col, const int &col_cnt) {
//
//    assert(s->storageModel() == StorageModel::ROW_STORE);
//    RowTable<B> *src = (RowTable<B> *) s;
//
//    Bit *read_ptr = (Bit *) (src->tuple_data_.data() + src->tuple_size_ * src_row + src->field_offsets_bytes_.at(src_col));
//    Bit *write_ptr = (Bit *) (tuple_data_.data() + this->tuple_size_ * dst_row + this->field_offsets_bytes_.at(dst_col));
//
//    int write_bit_cnt = 0;
//    int cursor = dst_col;
//    for(int i = 0; i < col_cnt;  ++i) {
//        write_bit_cnt += this->field_sizes_bytes_[cursor];
//        ++cursor;
//    }
//
//    for(size_t i = 0; i < write_bit_cnt; ++i) {
//        *write_ptr = emp::If(write, *read_ptr, *write_ptr);
//        ++read_ptr;
//        ++write_ptr;
//    }
//
//
//
//}


template<typename B>
void RowTable<B>::cloneRow(const int &dst_row, const int &dst_col, const QueryTable<B> *s, const int &src_row) {

    assert(s->storageModel() == StorageModel::ROW_STORE);
    RowTable<B> *src = (RowTable<B> *) s;

    int8_t *src_ptr = (int8_t *) (src->tuple_data_.data() + src->tuple_size_ * src_row);
    int8_t *dst_ptr = (int8_t *) (tuple_data_.data() + this->tuple_size_ * dst_row + this->field_offsets_bytes_.at(dst_col));
    int write_sz = src->tuple_size_ - src->field_sizes_bytes_.at(-1);
    memcpy(dst_ptr, src_ptr, write_sz);
}

template<typename B>
void RowTable<B>::cloneRow(const bool &write, const int &dst_row, const int &dst_col, const QueryTable<B> *src,
                             const int &src_row) {
    if(write)
        cloneRow(dst_row, dst_col, src, src_row);

}

template<typename B>
void RowTable<B>::cloneRow(const Bit &write, const int &dst_row, const int &dst_col, const QueryTable<B> *s, const int &src_row) {

    assert(s->storageModel() == StorageModel::ROW_STORE);
    RowTable<B> *src = (RowTable<B> *) s;

    Bit *read_ptr = (Bit *) (src->tuple_data_.data() + src->tuple_size_ * src_row );
    Bit *write_ptr = (Bit *) (tuple_data_.data() + this->tuple_size_ * dst_row + this->field_offsets_bytes_.at(dst_col));

    size_t write_size = src->schema_.size() - 1;
    for(size_t i = 0; i < write_size; ++i) {
        *write_ptr = emp::If(write, *read_ptr, *write_ptr);
        ++read_ptr;
        ++write_ptr;
    }

}

template<typename B>
void RowTable<B>::cloneTable(const int & dst_row, QueryTable<B> *s) {
    assert(s->storageModel() == StorageModel::ROW_STORE);
    assert(s->getSchema() == this->getSchema());
    assert((s->getTupleCount() + dst_row) <= this->tuple_cnt_);

    RowTable<B> *src = (RowTable<B> *) s;
    int8_t *write_ptr = this->tuple_data_.data() + dst_row * this->tuple_size_;
    int write_size = s->tuple_cnt_ * s->tuple_size_;

    memcpy(write_ptr, src->tuple_data_.data(), write_size);

}

template<typename B>
void RowTable<B>::compareSwap(const bool &swap, const int &lhs_row, const int &rhs_row) {
    if(swap) {
        int8_t *l = tuple_data_.data() + this->tuple_size_ * lhs_row;
        int8_t *r = tuple_data_.data() + this->tuple_size_ * rhs_row;

        // swap in place
        for(int i = 0; i < this->tuple_size_; ++i) {
            *l = *l ^ *r;
            *r = *r ^ *l;
            *l = *l ^ *r;

            ++l;
            ++r;
        }
    }
}

template<typename B>
void RowTable<B>::compareSwap(const Bit &swap, const int &lhs_row, const int &rhs_row) {

    Bit *l = (Bit *) (tuple_data_.data() + this->tuple_size_ * lhs_row );
    Bit *r = (Bit *) (tuple_data_.data() + this->tuple_size_ * rhs_row);


    size_t write_size = this->schema_.size();
    for(size_t i = 0; i < write_size; ++i) {
        Bit o = emp::If(swap, *l, *r);
        o ^= *r;
        *l ^= o;
        *r ^= o;


        ++l;
        ++r;
    }

}


//std::ostream &operator<<(std::ostream &os, RowTable<bool> &table) {
//    os << table.getSchema() << " isEncrypted? " << table.isEncrypted() <<  " order by: " << DataUtilities::printSortDefinition(table.getSortOrder()) << std::endl;
//
//
//    for(int i = 0; i < table.getTupleCount(); ++i) {
//        os <<  table.getPlainTuple(i);
//        if(!table.getDummyTag(i))
//            os << std::endl;
//    }
//
//    return os;
//}
//
//
//std::ostream &operator<<(std::ostream &os, const RowTable<Bit> &table) { throw; } // don't print MPC table

template <typename B>
bool RowTable<B>::operator==(const QueryTable<B> &other) const {

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


    QuerySchema *schema = const_cast<QuerySchema *>(&this->schema_);
    PlainTuple this_tuple(schema), other_tuple(schema);

    for(uint32_t i = 0; i < this->getTupleCount(); ++i) {
        this_tuple = this->getPlainTuple(i);
        other_tuple = other.getPlainTuple(i);

        if(this_tuple != other_tuple) {
            std::cout  << "Comparing on idx " << i << " with " << this_tuple.toString(true) << "\n          !=            " << other_tuple.toString(true) << endl;
            std::cout  << "    Failed to match!" << std::endl;
            return false;
        }

    }


    return true;

}

template<typename B>
string RowTable<B>::getOstringStream() const {
    assert(!this->isEncrypted());
    stringstream ss;
    ss << this->schema_ << " isEncrypted? " << this->isEncrypted() <<  " order by: " << DataUtilities::printSortDefinition(this->getSortOrder()) << std::endl;
    RowTable<bool> *plain_table = (RowTable<bool> *) this;

    for(int i = 0; i < this->getTupleCount(); ++i) {
        ss <<  this->getPlainTuple(i);

        if(!plain_table->getDummyTag(i))
            ss << std::endl;
    }

    return ss.str();
}


template <typename B>
string RowTable<B>::toString(const bool & showDummies) const {

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
std::string RowTable<B>::toString(const size_t &limit, const bool &show_dummies) const {
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









template class vaultdb::RowTable<bool>;
template class vaultdb::RowTable<emp::Bit>;
