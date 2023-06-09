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
    QuerySchema dst_schema = QuerySchema::toPlain(this->schema_);

    auto dst_table = new RowTable<bool>(this->tuple_cnt_, dst_schema, this->getSortOrder());
    int write_cursor = 0;


    for(uint32_t i = 0; i < this->tuple_cnt_; ++i)  {
        bool dummy_tag = table->getDummyTag(i).reveal();
        if(!dummy_tag) { // if real tuple (not a dummy), reveal it
            const SecureTuple tuple(&this->schema_, ((RowTable<emp::Bit> *) this)->tuple_data_.data() + i * table->tuple_size_);
            PlainTuple dst_tuple = tuple.reveal(&dst_schema, party);
            dst_table->putTuple(write_cursor, dst_tuple);
            ++write_cursor;
        }
    }

    dst_table->resize(write_cursor);
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
SecureTable *RowTable<B>::secretShare(emp::NetIO *netio, const int & party)  {


    size_t alice_tuple_cnt =  this->getTupleCount();
    size_t bob_tuple_cnt = alice_tuple_cnt;

    if (party == ALICE) {
        netio->send_data(&alice_tuple_cnt, 4);
        netio->flush();
        netio->recv_data(&bob_tuple_cnt, 4);
        netio->flush();
    } else if (party == BOB) {
        netio->recv_data(&alice_tuple_cnt, 4);
        netio->flush();
        netio->send_data(&bob_tuple_cnt, 4);
        netio->flush();
    }

    QuerySchema dst_schema = QuerySchema::toSecure(this->schema_);

    auto dst_table = new RowTable<Bit>(alice_tuple_cnt + bob_tuple_cnt, dst_schema, this->getSortOrder());

    // preserve sort order - reverse input order for latter half to do bitonic merge
    if(!this->getSortOrder().empty()) {
        if (party == emp::ALICE) {
            if(alice_tuple_cnt > 0) secret_share_send(emp::ALICE, (PlainTable *)  this, dst_table, 0, true);
            if(bob_tuple_cnt > 0) secret_share_recv(bob_tuple_cnt, emp::BOB, dst_table, alice_tuple_cnt, false);

        } else { // bob
            if(alice_tuple_cnt > 0) secret_share_recv(alice_tuple_cnt, emp::ALICE, dst_table, 0, true);
            if(bob_tuple_cnt > 0)  secret_share_send(emp::BOB, (PlainTable *)  this, dst_table, alice_tuple_cnt, false);
        }
        Sort<emp::Bit>::bitonicMerge(dst_table, dst_table->getSortOrder(), 0, dst_table->getTupleCount(), true);
    }
    else { // concatenate Alice and Bob
        if (party == emp::ALICE) {
            if(alice_tuple_cnt > 0) secret_share_send(emp::ALICE, (PlainTable *)  this, dst_table, 0, false);
            if(bob_tuple_cnt > 0)  secret_share_recv(bob_tuple_cnt, emp::BOB, dst_table, alice_tuple_cnt, false);
        } else { // bob
            if(alice_tuple_cnt > 0) secret_share_recv(alice_tuple_cnt, emp::ALICE, dst_table, 0, false);
            if(bob_tuple_cnt > 0)  secret_share_send(emp::BOB, (PlainTable *)  this, dst_table, alice_tuple_cnt, false);
        }
    }
    netio->flush();

    return dst_table;

}




// ZK secret share - only alice provides inputs
template<typename B>
SecureTable *RowTable<B>::secretShare(BoolIO<NetIO> *ios[], const size_t & thread_count, const int & party)  {


    size_t alice_tuple_cnt =  this->getTupleCount();


    // reset before we send the counts
    for(size_t i = 0; i < thread_count; ++i) {
        ios[i]->flush();
    }

    NetIO *netio = ios[0]->io;

    if (party == ALICE) {
        netio->send_data(&alice_tuple_cnt, 4);
        netio->flush();
    } else if (party == BOB) {
        netio->recv_data(&alice_tuple_cnt, 4);
        netio->flush();
    }

    assert(alice_tuple_cnt > 0);

    QuerySchema dst_schema = QuerySchema::toSecure(this->getSchema());

    SecureTable *dst_table = new RowTable<Bit>(alice_tuple_cnt, dst_schema, this->getSortOrder());


    if (party == emp::ALICE) {
        secret_share_send(emp::ALICE,  (PlainTable *) this, dst_table, 0, false);
    } else { // bob
        secret_share_recv(alice_tuple_cnt, emp::ALICE, dst_table, 0, false);
    }

    for(size_t i = 0; i < thread_count; ++i) {
        ios[i]->flush();
    }

    return dst_table;

}


template<typename B>
SecureTable *RowTable<B>::secret_share_send_table(const PlainTable *input, emp::NetIO *io, const int &sharing_party) {
    QuerySchema dst_schema = QuerySchema::toSecure(input->getSchema());
    int32_t tuple_cnt = input->getTupleCount();
    io->send_data(&tuple_cnt, 4);
    io->flush();

    SecureTable  *shared =  new RowTable<Bit>(input->getTupleCount(), dst_schema, input->getSortOrder());
    secret_share_send(sharing_party, input, shared, 0, false);
    return shared;
}

template<typename B>
SecureTable *
RowTable<B>::secret_share_recv_table(const QuerySchema &src_schema, // plaintext schema
                                       const SortDefinition &sort_definition, emp::NetIO *io, const int &sharing_party)
{
    int32_t tuple_cnt;
    io->recv_data(&tuple_cnt, 4);
    io->flush();

    QuerySchema dst_schema = QuerySchema::toSecure(src_schema);
    SecureTable *shared =  new RowTable<Bit>(tuple_cnt, dst_schema, sort_definition);
    secret_share_recv(tuple_cnt, sharing_party, shared, 0, false);
    return shared;
}


// use this for acting as a data sharing party in the PDF
// generates alice and bob's shares and returns the pair
template<typename B>
SecretShares RowTable<B>::generateSecretShares() const {
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
void
RowTable<B>::secret_share_send(const int &party,const PlainTable *src_table, SecureTable *dst_table, const int &write_offset,
                                 const bool &reverse_read_order)  {

    int cursor = (int) write_offset;

    if(reverse_read_order) {
        for(int i = src_table->getTupleCount() - 1; i >= 0; --i) {
            FieldUtilities::secret_share_send(src_table, i, dst_table,  cursor, party);
            ++cursor;
        }

        return;

    }

    // else
    for(size_t i = 0; i < src_table->getTupleCount(); ++i) {
        FieldUtilities::secret_share_send(src_table, i, dst_table,  cursor, party);
        ++cursor;
    }

}

template<typename B>
void RowTable<B>::secret_share_recv(const size_t &tuple_count, const int &dst_party,
                                      SecureTable *dst_table, const size_t &write_offset,
                                      const bool &reverse_read_order)  {

    int32_t cursor = (int32_t) write_offset;

    if(reverse_read_order) {

        for(int32_t i = tuple_count - 1; i >= 0; --i) {
            FieldUtilities::secret_share_recv(dst_table, cursor, dst_party);
            ++cursor;
        }



        return;
    }

    // else
    for(size_t i = 0; i < tuple_count; ++i) {
        FieldUtilities::secret_share_recv(dst_table, cursor, dst_party);
        ++cursor;
    }



}

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
    memcpy(dst_ptr, src_ptr, src->tuple_size_ - this->field_sizes_bytes_.at(-1));
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

template class vaultdb::RowTable<bool>;
template class vaultdb::RowTable<emp::Bit>;
