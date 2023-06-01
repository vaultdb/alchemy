#include <memory>
#include <utility>
#include <util/data_utilities.h>
#include <util/type_utilities.h>
#include <util/field_utilities.h>
#include <util/logger.h>
#include "query_table.h"
#include "plain_tuple.h"
#include "secure_tuple.h"

#include <operators/sort.h>


using namespace vaultdb;







template <typename B>
QueryTable<B>::QueryTable(const size_t &num_tuples, const QuerySchema &schema, const SortDefinition & sortDefinition)
        :  order_by_(std::move(sortDefinition)), schema_(std::make_shared<QuerySchema>(schema)), tuple_cnt_(num_tuples) {

    setSchema(schema);


    if(num_tuples == 0)
        return;

    tuple_data_.resize(num_tuples * tuple_size_);

      

    std::memset(tuple_data_.data(), 0, tuple_data_.size());
    for(int i = 0; i < num_tuples; ++i) {
        getTuple(i).setDummyTag(true);
    }


}

template <typename B>
QueryTable<B>::QueryTable(const QueryTable<B> &src) : order_by_(src.getSortOrder()), tuple_size_(src.tuple_size_), tuple_cnt_(src.tuple_cnt_) {
    tuple_data_ = src.tuple_data_;
    setSchema(*src.schema_);

}



template <typename B>
bool QueryTable<B>::isEncrypted() const {
    return std::is_same_v<B, emp::Bit>;
}



template <typename B>
std::unique_ptr<PlainTable> QueryTable<B>::reveal(int empParty) const  {
   return revealTable(this, empParty);

}



// iterate over all tuples and produce one long bit array for encrypting/decrypting in emp
// only tested in PUBLIC or XOR mode
template <typename B>
vector<int8_t> QueryTable<B>::serialize() const {
    // copy out our payload
    return vector<int8_t>(tuple_data_);

}

std::ostream &vaultdb::operator<<(std::ostream &os, const PlainTable &table) {
        os << *(table.getSchema()) << " isEncrypted? " << table.isEncrypted() <<  " order by: " << DataUtilities::printSortDefinition(table.getSortOrder()) << endl;

        PlainTable *ref = const_cast<PlainTable *>(&table);

        for(auto pos = ref->begin(); pos != ref->end(); ++pos) {
            os << *pos;
            const bool is_dummy = pos->getDummyTag();
            if(!is_dummy)
                os << endl;

        }

        return os;

}

std::ostream &vaultdb::operator<<(std::ostream &os, const SecureTable &table) {


    os << table.getSchema() << " isEncrypted? " << table.isEncrypted() <<  " order by: " << DataUtilities::printSortDefinition(table.getSortOrder()) << endl;

    for(uint32_t i = 0; i < table.getTupleCount(); ++i) {
        os << table[i] << endl;
    }

    return os;

}


template <typename B>
string QueryTable<B>::toString(const bool & showDummies) const {

    std::ostringstream os;

    if(!showDummies) {
        os << *this;
        return os.str();
    }

    // show dummies case
    os << *getSchema() << " isEncrypted? " << isEncrypted() <<  " order by: " << DataUtilities::printSortDefinition(getSortOrder()) << std::endl;


    for(uint32_t i = 0; i < getTupleCount(); ++i) {
        PlainTuple tuple = getPlainTuple(i);
        os << tuple.toString(showDummies) << std::endl;
    }

    return os.str();

}

template <typename B>
QueryTable<B> & QueryTable<B>::operator=(const QueryTable<B> & src) {
    if(&src == this)
        return *this;

    setSchema(*src.getSchema());

    tuple_data_.resize(src.tuple_data_.size());
    memcpy(tuple_data_.data(), src.tuple_data_.data(), tuple_data_.size());
    return *this;
}

template <typename B>
void QueryTable<B>::putTuple(const int &idx, const QueryTuple<B> & tuple) {
    assert(*tuple.getSchema() == *schema_);

    size_t tuple_offset = idx * tuple_size_;
    memcpy(tuple_data_.data() + tuple_offset, tuple.getData(), tuple_size_);


}





template <typename B>
bool QueryTable<B>::operator==(const QueryTable<B> &other) const {

    assert(!isEncrypted()); // reveal this for tables in the clear

    if(*getSchema() != *other.getSchema()) {

        std::cout << "Failed to match on schema: \n" << *getSchema() << "\n  == \n" << *other.getSchema() << std::endl;
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


    size_t tuple_offset = 0;

    for(uint32_t i = 0; i < getTupleCount(); ++i) {
        PlainTuple this_tuple(schema_, (int8_t *) (tuple_data_.data() + tuple_offset));
        PlainTuple other_tuple(schema_, (int8_t *) (other.tuple_data_.data() + tuple_offset));

        if(this_tuple != other_tuple) {
            std::cout  << "Comparing on idx " << i << " with " << this_tuple.toString(true) << "\n          !=            " << other_tuple.toString(true) << endl;
            std::cout  << "    Failed to match!" << std::endl;
           return false;
        }

        tuple_offset += tuple_size_;
    }


    return true;
}


// up to two-way secret share - both Alice and Bob providing private inputs
template<typename B>
std::shared_ptr<SecureTable> QueryTable<B>::secretShare(const PlainTable *input, emp::NetIO *netio, const int & party)  {


    size_t alice_tuple_cnt =  input->getTupleCount();
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

    QuerySchema dst_schema = QuerySchema::toSecure(*(input->getSchema()));

    std::shared_ptr<SecureTable> dst_table(new SecureTable(alice_tuple_cnt + bob_tuple_cnt, dst_schema, input->getSortOrder()));

    // preserve sort order - reverse input order for latter half to do bitonic merge
    if(!input->getSortOrder().empty()) {
        if (party == emp::ALICE) {
            if(alice_tuple_cnt > 0) secret_share_send(emp::ALICE, input, dst_table.get(), 0, true);
            if(bob_tuple_cnt > 0) secret_share_recv(bob_tuple_cnt, emp::BOB, dst_table.get(), alice_tuple_cnt, false);

        } else { // bob
            if(alice_tuple_cnt > 0) secret_share_recv(alice_tuple_cnt, emp::ALICE, dst_table.get(), 0, true);
            if(bob_tuple_cnt > 0)  secret_share_send(emp::BOB, input, dst_table.get(), alice_tuple_cnt, false);
        }
        Sort<emp::Bit>::bitonicMerge(dst_table, dst_table->getSortOrder(), 0, dst_table->getTupleCount(), true);
    }
    else { // concatenate Alice and Bob
        if (party == emp::ALICE) {
            if(alice_tuple_cnt > 0) secret_share_send(emp::ALICE, input, dst_table.get(), 0, false);
            if(bob_tuple_cnt > 0)  secret_share_recv(bob_tuple_cnt, emp::BOB, dst_table.get(), alice_tuple_cnt, false);
        } else { // bob
            if(alice_tuple_cnt > 0) secret_share_recv(alice_tuple_cnt, emp::ALICE, dst_table.get(), 0, false);
            if(bob_tuple_cnt > 0)  secret_share_send(emp::BOB, input, dst_table.get(), alice_tuple_cnt, false);
        }
    }
    netio->flush();

    return dst_table;

}




// ZK secret share - only alice provides inputs
template<typename B>
std::shared_ptr<SecureTable> QueryTable<B>::secretShare(const PlainTable *input, BoolIO<NetIO> *ios[], const size_t & thread_count, const int & party)  {


    size_t alice_tuple_cnt =  input->getTupleCount();


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

    QuerySchema dst_schema = QuerySchema::toSecure(*input->getSchema());

    std::shared_ptr<SecureTable> dst_table(new SecureTable(alice_tuple_cnt, dst_schema, input->getSortOrder()));


    if (party == emp::ALICE) {
        secret_share_send(emp::ALICE, input, dst_table.get(), 0, false);
    } else { // bob
        secret_share_recv(alice_tuple_cnt, emp::ALICE, dst_table.get(), 0, false);
    }

    for(size_t i = 0; i < thread_count; ++i) {
        ios[i]->flush();
    }

    return dst_table;

}


template<typename B>
shared_ptr<SecureTable>
QueryTable<B>::secret_share_send_table(const PlainTable *input, emp::NetIO *io, const int &sharing_party) {
    QuerySchema dst_schema = QuerySchema::toSecure(*input->getSchema());
    int32_t tuple_cnt = input->getTupleCount();
    io->send_data(&tuple_cnt, 4);
    io->flush();

    std::shared_ptr<SecureTable> shared = std::make_shared<SecureTable>(input->getTupleCount(), dst_schema, input->getSortOrder());
    secret_share_send(sharing_party, input, shared.get(), 0, false);
    return shared;
}

template<typename B>
shared_ptr<SecureTable>
QueryTable<B>::secret_share_recv_table(const QuerySchema &src_schema, // plaintext schema
                                       const SortDefinition &sort_definition, emp::NetIO *io, const int &sharing_party)
{
    int32_t tuple_cnt;
    io->recv_data(&tuple_cnt, 4);
    io->flush();

    QuerySchema dst_schema = QuerySchema::toSecure(src_schema);
    std::shared_ptr<SecureTable> shared = std::make_shared<SecureTable>(tuple_cnt, dst_schema, sort_definition);
    secret_share_recv(tuple_cnt, sharing_party, shared.get(), 0, false);
    return shared;
}


// use this for acting as a data sharing party in the PDF
// generates alice and bob's shares and returns the pair
template<typename B>
SecretShares QueryTable<B>::generateSecretShares() const {
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
void QueryTable<B>::setSortOrder(const SortDefinition &sortOrder) {
    order_by_ = sortOrder;

}

template <typename B>
SortDefinition QueryTable<B>::getSortOrder() const {
    return order_by_;
}

template <typename B>
std::shared_ptr<PlainTable> QueryTable<B>::deserialize(const QuerySchema &schema, const vector<int8_t> & tableBits) {

    uint32_t tableSize = tableBits.size(); // in bytes
    uint32_t tupleSize = schema.size() / 8; // in bytes
    uint32_t tupleCount = tableSize / tupleSize;

    std::shared_ptr<PlainTable > result(new PlainTable(tupleCount, schema));
    result->tuple_data_ = tableBits;

    return result;


}

template<typename B>
std::shared_ptr<SecureTable>
QueryTable<B>::deserialize(const QuerySchema &schema, vector<Bit> &table_bits) {
    QuerySchema encrypted_schema = QuerySchema::toSecure(schema);
    uint32_t table_size = table_bits.size(); // in bits
    uint32_t tuple_size = encrypted_schema.size(); // in bits
    assert(table_size % tuple_size == 0);
    uint32_t tuple_cnt = table_size / tuple_size;

    std::shared_ptr<SecureTable> result(new SecureTable(tuple_cnt, encrypted_schema));
    assert(result->tuple_data_.size() / TypeUtilities::getEmpBitSize() == table_bits.size());

    memcpy(result->tuple_data_.data(), table_bits.data(), table_size * TypeUtilities::getEmpBitSize());

    return result;

}

template<typename B>
void QueryTable<B>::resize(const size_t &cnt) {
    tuple_data_.resize(cnt * tuple_size_);
    tuple_cnt_ = cnt;
}



template<typename B>
size_t QueryTable<B>::getTrueTupleCount() const {
    assert(!isEncrypted());
    size_t count = 0;

    for(size_t i = 0; i < tuple_cnt_; ++i) {
        PlainTuple p = getPlainTuple(i);
        if(!p.getDummyTag())
            ++count;
    }

    return count;
}

template<typename B>
PlainTuple QueryTable<B>::getPlainTuple(size_t idx) const {
    assert(!isEncrypted()); // B == bool
    size_t tuple_offset = tuple_size_ * idx;
    std::shared_ptr<QuerySchema> q = schema_;
    int8_t *tuple_pos =  (int8_t *) tuple_data_.data() + tuple_offset;
    return vaultdb::PlainTuple(q,tuple_pos);
}

template<typename B>
QueryTuple<B> QueryTable<B>::operator[](const int &idx) {
    return this->getTuple(idx);
}

template<typename B>
const QueryTuple<B> QueryTable<B>::operator[](const int &idx) const {
    int8_t *read_ptr = (int8_t *) (tuple_data_.data() + tuple_size_ * idx);

    const QueryTuple<B> tuple(schema_, read_ptr);
    return tuple;
}

template<typename B>
std::unique_ptr<PlainTable> QueryTable<B>::revealTable(const SecureTable *table, const int & party) {
    uint32_t tuple_cnt = table->getTupleCount();


    shared_ptr<QuerySchema> dst_schema = std::make_shared<QuerySchema>(QuerySchema::toPlain(*(table->getSchema())));

    std::unique_ptr<PlainTable > dst_table(new PlainTable(tuple_cnt, dst_schema, table->getSortOrder()));


    for(uint32_t i = 0; i < tuple_cnt; ++i)  {
        const SecureTuple tuple = table->getImmutableTuple(i);
        if(!tuple.getDummyTag().reveal()) { // if real tuple (not a dummy), reveal it
            PlainTuple dst_tuple = tuple.reveal(dst_schema, party);
            dst_table->putTuple(i, dst_tuple);
        }
        else {
            (*dst_table)[i].setDummyTag(true);
        }
    }

    return dst_table;
}


template<typename B>
void
QueryTable<B>::secret_share_send(const int &party,const PlainTable *src_table, SecureTable *dst_table, const int &write_offset,
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
void QueryTable<B>::secret_share_recv(const size_t &tuple_count, const int &dst_party,
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
std::string QueryTable<B>::toString(const size_t &limit, const bool &show_dummies) const {
    std::ostringstream os;
    size_t tuples_printed = 0;
    size_t cursor = 0;

    assert(!isEncrypted());

    os << *getSchema() <<  " order by: " << DataUtilities::printSortDefinition(getSortOrder()) << std::endl;
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
void QueryTable<B>::setSchema(const QuerySchema &schema) {
    schema_ = std::make_shared<QuerySchema>(schema);

    if(std::is_same_v<emp::Bit, B>) {
        tuple_size_ = schema_->size() * TypeUtilities::getEmpBitSize(); // bits, one block per bit
        // covers dummy tag as -1
        for(auto pos : schema_->offsets_) {
            field_offsets_bytes_[pos.first] = pos.second * TypeUtilities::getEmpBitSize();
            field_sizes_bytes_[pos.first] = schema_->getField(pos.first).size() * TypeUtilities::getEmpBitSize();
        }


        return;

    }

    // plaintext case
    tuple_size_ = schema_->size()/8; // bytes for plaintext

    for(auto pos : schema_->offsets_) {
        field_offsets_bytes_[pos.first] = pos.second / 8;
        field_sizes_bytes_[pos.first] = schema_->getField(pos.first).size() /8;
    }


}



#include "query_table.h"

using namespace vaultdb;

template<typename B>
void QueryTable<B>::assignField(const int &dst_row, const int &dst_col, const QueryTable<B> *src, const int &src_row,
                                const int &src_col) {

    int8_t *src_field = (int8_t *) (src->tuple_data_.data() + src->tuple_size_ * src_row + src->field_offsets_bytes_.at(src_col));
    int8_t *dst_field = (int8_t *) (tuple_data_.data() + tuple_size_ * dst_row + field_offsets_bytes_.at(dst_col));
    memcpy(dst_field, src_field, field_sizes_bytes_.at(dst_col));


}


template<typename B>
void
QueryTable<B>::cloneFields(const int &dst_row, const int &dst_col, const QueryTable<B> *src, const int &src_row, const int & src_col,
                           const int &col_cnt) {

    int8_t *src_ptr = (int8_t *) (src->tuple_data_.data() + src->tuple_size_ * src_row + src->field_offsets_bytes_.at(src_col));
    int8_t *dst_ptr = (int8_t *) (tuple_data_.data() + tuple_size_ * dst_row + field_offsets_bytes_.at(dst_col));

    int write_size = 0;
    int cursor = dst_col;
    for(int i = 0; i < col_cnt;  ++i) {
        write_size += field_sizes_bytes_[cursor];
        ++cursor;
    }

    memcpy(dst_ptr, src_ptr, write_size);
}


template<typename B>
void QueryTable<B>::cloneFields(bool write, const int &dst_row, const int &dst_col, const QueryTable<B> *src,
                                const int &src_row, const int &src_col, const int &col_cnt) {

    if(write)
        cloneFields(dst_row, dst_col, src, src_row, src_col, col_cnt);


}


template<typename B>
void QueryTable<B>::cloneFields(Bit write, const int &dst_row, const int &dst_col, const QueryTable<B> *src,
                                const int &src_row, const int &src_col, const int &col_cnt) {

    Bit *read_ptr = (Bit *) (src->tuple_data_.data() + src->tuple_size_ * src_row + src->field_offsets_bytes_.at(src_col));
    Bit *write_ptr = (Bit *) (tuple_data_.data() + tuple_size_ * dst_row + field_offsets_bytes_.at(dst_col));

    int write_bit_cnt = 0;
    int cursor = dst_col;
    for(int i = 0; i < col_cnt;  ++i) {
        write_bit_cnt += field_sizes_bytes_[cursor];
        ++cursor;
    }

    for(size_t i = 0; i < write_bit_cnt; ++i) {
        *write_ptr = emp::If(write, *read_ptr, *write_ptr);
        ++read_ptr;
        ++write_ptr;
    }



}


template<typename B>
void QueryTable<B>::cloneRow(const int &dst_row, const int &dst_col, const QueryTable<B> *src, const int &src_row) {

    int8_t *src_ptr = (int8_t *) (src->tuple_data_.data() + src->tuple_size_ * src_row);
    int8_t *dst_ptr = (int8_t *) (tuple_data_.data() + tuple_size_ * dst_row + field_offsets_bytes_.at(dst_col));
    memcpy(dst_ptr, src_ptr, src->tuple_size_ - field_sizes_bytes_.at(-1));
}

template<typename B>
void QueryTable<B>::cloneRow(const bool &write, const int &dst_row, const int &dst_col, const QueryTable<B> *src,
                             const int &src_row) {
    if(write)
        cloneRow(dst_row, dst_col, src, src_row);

}

template<typename B>
void QueryTable<B>::cloneRow(const Bit &write, const int &dst_row, const int &dst_col, const QueryTable<B> *src,
                             const int &src_row) {

    Bit *read_ptr = (Bit *) (src->tuple_data_.data() + src->tuple_size_ * src_row );
    Bit *write_ptr = (Bit *) (tuple_data_.data() + tuple_size_ * dst_row + field_offsets_bytes_.at(dst_col));

    size_t write_size = src->schema_->size() - 1;
    for(size_t i = 0; i < write_size; ++i) {
        *write_ptr = emp::If(write, *read_ptr, *write_ptr);
        ++read_ptr;
        ++write_ptr;
    }

}



template class vaultdb::QueryTable<bool>;
template class vaultdb::QueryTable<emp::Bit>;
