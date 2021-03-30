#include <memory>
#include <utility>
#include <util/data_utilities.h>
#include <util/type_utilities.h>
#include "query_table.h"
#include "plain_tuple.h"
#include "secure_tuple.h"

using namespace vaultdb;




template <typename B>
void QueryTable<B>::setSchema(const QuerySchema & s) {
    schema_ = std::make_shared<QuerySchema>(s);
}

template <typename B>
const std::shared_ptr<QuerySchema>  QueryTable<B>::getSchema() const { return schema_; }

template <typename B>
unsigned int QueryTable<B>::getTupleCount() const {
    return tuple_data_.size() / tuple_size_;
}


template <typename B>
QueryTable<B>::QueryTable(const size_t &num_tuples, const QuerySchema &schema, const SortDefinition & sortDefinition)
        :  orderBy(std::move(sortDefinition)), schema_(std::make_shared<QuerySchema>(schema)) {

     tuple_size_ = schema_->size()/8; // bytes for plaintext
    if(std::is_same_v<emp::Bit, B>) {
        tuple_size_ *= 8 * sizeof(emp::block); // bits, one block per bit
    }

    tuple_data_.resize(num_tuples * tuple_size_);
    if(std::is_same_v<emp::Bit, B>) {
        emp::Integer tmp(schema_->size() * num_tuples, 0, emp::PUBLIC);
        memcpy(tuple_data_.data(), tmp.bits.data(), tuple_size_ * num_tuples);
    }
    else {
        std::memset(tuple_data_.data(), 0, tuple_data_.size());
    }

}



/*template <typename B>
QueryTable<B>::QueryTable(const size_t &num_tuples, const int &colCount) {
    schema_ = std::make_shared<QuerySchema>(QuerySchema(colCount));
    tuples_.resize(num_tuples);
    for(size_t i = 0; i < num_tuples; ++i)
        tuples_[i] = QueryTuple<B>(schema_);

} */


template <typename B>
bool QueryTable<B>::isEncrypted() const {
    return std::is_same_v<B, emp::Bit>;
}



template <typename B>
std::unique_ptr<PlainTable> QueryTable<B>::reveal(int empParty) const  {
    assert(isEncrypted());

    QuerySchema dst_schema = QuerySchema::toPlain(*schema_);
    std::unique_ptr<QueryTable<bool>> dst_table = std::unique_ptr<QueryTable<bool>>(new QueryTable<bool>(getTupleCount(), dst_schema));

    // secret share the entire serialized table
    size_t tuple_bits = dst_schema.size();


    emp::Integer dst_bytes(tuple_bits, 0L, empParty);
    memcpy(dst_bytes.bits.data(), tuple_data_.data(), tuple_bits * sizeof(emp::block));


    bool *bools = new bool[tuple_bits];
    std::string revealed_bits = dst_bytes.reveal<std::string>(empParty);
    int i = 0;
    for(char b : revealed_bits) {
        bools[i] = (b == '1');
       ++i;
    }

    vector<int8_t> decodedBytesVector = Utilities::boolsToBytes(bools, tuple_bits);
    memcpy(dst_table->tuple_data_.data(), decodedBytesVector.data(), decodedBytesVector.size());

    delete[] bools;
    return dst_table;

}

// iterate over all tuples and produce one long bit array for encrypting/decrypting in emp
// only tested in PUBLIC or XOR mode
template <typename B>
vector<int8_t> QueryTable<B>::serialize() const {
    // copy out our payload
    return vector<int8_t>(tuple_data_);

}

std::ostream &vaultdb::operator<<(std::ostream &os, const PlainTable &table) {


        os <<  *table.getSchema() << " isEncrypted? " << table.isEncrypted() << endl;

        for(uint32_t i = 0; i < table.getTupleCount(); ++i) {
            PlainTuple tuple = table[i];

            os << tuple;
            const bool isDummy = tuple.getDummyTag();
            if(!isDummy)
                os << endl;

        }

        return os;

}

std::ostream &vaultdb::operator<<(std::ostream &os, const SecureTable &table) {


    os <<  table.getSchema() << " isEncrypted? " << table.isEncrypted() << endl;

    for(uint32_t i = 0; i < table.getTupleCount(); ++i) {
        SecureTuple tuple = table[i];
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
    os <<  *getSchema() << " isEncrypted? " << isEncrypted() << std::endl;


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
QueryTable<B>::QueryTable(const QueryTable<B> &src) : orderBy(src.getSortOrder()), tuple_size_(src.tuple_size_) {
    schema_ = std::make_shared<QuerySchema>(*src.getSchema());
    tuple_data_ = src.tuple_data_;
}



template <typename B>
bool QueryTable<B>::operator==(const QueryTable<B> &other) const {

    std::cout << "Equality predicate on QueryTable: " << std::endl;

    assert(!isEncrypted()); // reveal this for tables in the clear

    if(*getSchema() != *other.getSchema()) {
        std::cout << "Failed to match on schema: \n" << *getSchema()  << "\n  == \n" << *other.getSchema() << std::endl;
        return false;
    }

    if(this->getSortOrder() != other.getSortOrder()) {
        std::cout << "Failed to match on sort order expected="  << DataUtilities::printSortDefinition(this->getSortOrder())
                  << "observed=" << DataUtilities::printSortDefinition(other.getSortOrder()) <<  std::endl;
        return false;
    }

    if(this->getTupleCount() != other.getTupleCount()) {
        std::cout << "Failed to match on tuple count " << this->getTupleCount() << " vs " << other.getTupleCount() << std::endl;
        return false;
    }


    size_t tuple_offset = 0;
    std::shared_ptr<QuerySchema> q = schema_;

    for(uint32_t i = 0; i < getTupleCount(); ++i) {
        PlainTuple thisTuple(q, (int8_t *) (tuple_data_.data() + tuple_offset));
        PlainTuple otherTuple(q, (int8_t *) (other.tuple_data_.data() + tuple_offset));

        if(thisTuple != otherTuple) {
            std::cout << "Comparing on idx " << i << " with "  << thisTuple.toString(true) << "\n          !=            " << otherTuple.toString(true) << endl;
            std::cout << "    Failed to match!" << std::endl;
           return false;
        }

        tuple_offset += tuple_size_;
    }


    return true;
}



template<typename B>
std::shared_ptr<SecureTable> QueryTable<B>::secretShare(emp::NetIO *netio, const int & party) const {

    size_t alice_tuple_cnt = this->getTupleCount();
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


    QuerySchema dstSchema = QuerySchema::toSecure(*schema_);

    std::shared_ptr<SecureTable> dstTable(new SecureTable(alice_tuple_cnt + bob_tuple_cnt, dstSchema, getSortOrder()));
    bool *bools = Utilities::bytesToBool((int8_t *) tuple_data_.data(), tuple_data_.size());

    if(party == emp::ALICE) {
        // feed through A data first, then wait for B
        auto dst = (emp::block *) dstTable->tuple_data_.data();
        emp::ProtocolExecution::prot_exec->feed(dst, emp::ALICE, bools, alice_tuple_cnt * tuple_size_  * 8);
        dst += alice_tuple_cnt * tuple_size_  * 8;
        emp::ProtocolExecution::prot_exec->feed(dst, emp::BOB, nullptr, bob_tuple_cnt * tuple_size_  * 8);
    }
    else { // bob
        auto dst = (emp::block *) dstTable->tuple_data_.data();
        emp::ProtocolExecution::prot_exec->feed(dst, emp::ALICE, nullptr, alice_tuple_cnt * tuple_size_  * 8);
        dst += alice_tuple_cnt * tuple_size_  * 8;
        emp::ProtocolExecution::prot_exec->feed(dst, emp::BOB, bools, bob_tuple_cnt * tuple_size_  * 8);
    }

    delete[] bools;

    netio->flush();

    return dstTable;

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
    orderBy = sortOrder;

}

template <typename B>
SortDefinition QueryTable<B>::getSortOrder() const {
    return orderBy;
}

template <typename B>
std::shared_ptr<PlainTable> QueryTable<B>::deserialize(const QuerySchema &schema, const vector<int8_t> & tableBits) {

    uint32_t tableSize = tableBits.size(); // in bytes
    uint32_t tupleSize = schema.size() / 8; // in bytes
    uint32_t tupleCount = tableSize / tupleSize;
    SortDefinition emptySortDefinition;

    std::shared_ptr<PlainTable > result(new PlainTable(tupleCount, schema));
    result->tuple_data_ = tableBits;

    return result;


}

template<typename B>
std::shared_ptr<SecureTable>
QueryTable<B>::deserialize(const QuerySchema &schema, vector<Bit> &tableBits) {
    uint32_t tableSize = tableBits.size(); // in bits
    uint32_t tupleSize = schema.size(); // in bits
    uint32_t tupleCount = tableSize / tupleSize;

    QuerySchema encryptedSchema = QuerySchema::toSecure(schema);
    std::shared_ptr<SecureTable> result(new SecureTable(tupleCount, encryptedSchema));
    memcpy(result->tuple_data_.data(), tableBits.data(), tableSize * sizeof(emp::block));

    return result;

}

template<typename B>
void QueryTable<B>::resize(const size_t &tupleCount) {
    tuple_data_.resize(tupleCount * tuple_size_);
}

template<typename B>
QueryTuple<B> QueryTable<B>::getTuple(int idx)   {
    int8_t *write_ptr = (int8_t *) (tuple_data_.data() + tuple_size_ * idx);
    return QueryTuple<B>(schema_,  write_ptr);
}

template<typename B>
size_t QueryTable<B>::getTrueTupleCount() const {
    assert(!isEncrypted());
    size_t count = 0;

    for(size_t i = 0; i < tuple_data_.size() / tuple_size_; ++i) {
        PlainTuple p = getPlainTuple(i);
        if(!p.isEncrypted())
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


template class vaultdb::QueryTable<bool>;
template class vaultdb::QueryTable<emp::Bit>;
