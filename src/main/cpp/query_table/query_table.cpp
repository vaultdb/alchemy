#include <memory>
#include <utility>
#include <util/data_utilities.h>
#include <util/type_utilities.h>
#include "query_table.h"

using namespace vaultdb;

template <typename B>
QueryTuple<B> QueryTable<B>::getTuple(int idx) const {
    return tuples_[idx];
}



template <typename B>
void QueryTable<B>::setSchema(const QuerySchema & s) {
    schema_ = s;
}

template <typename B>
const QuerySchema & QueryTable<B>::getSchema() const { return schema_; }

template <typename B>
unsigned int QueryTable<B>::getTupleCount() const {
    return tuples_.size();
}


template <typename B>
QueryTable<B>::QueryTable(const size_t &num_tuples, const QuerySchema &schema, const SortDefinition & sortDefinition)
        :  orderBy(std::move(sortDefinition)), schema_(schema) {

   //std::cout << "Instantiating a query table at: " << std::endl
   //           << Utilities::getStackTrace();

    tuples_.resize(num_tuples);
    for(size_t i = 0; i < num_tuples; ++i)
        tuples_[i] = QueryTuple<B>(schema.getFieldCount());


}



template <typename B>
QueryTable<B>::QueryTable(const size_t &num_tuples, const int &colCount)
    : schema_(QuerySchema(colCount)) {

    tuples_.resize(num_tuples);
    for(size_t i = 0; i < num_tuples; ++i)
        tuples_[i] = QueryTuple<B>(colCount);

}

template <typename B>
bool QueryTable<B>::isEncrypted() const {
    FieldType firstColType = schema_.getField(0).getType();

    // if encrypted version of this column is the same as its original value
    return TypeUtilities::isEncrypted(firstColType);
}



template <typename B>
std::unique_ptr<PlainTable> QueryTable<B>::reveal(int empParty) const  {
    uint32_t tupleCount = getTupleCount();

    if(!this->isEncrypted())
        throw; // NYI, copy this out from unencrypted source

    QuerySchema dstSchema = QuerySchema::toPlain(getSchema());

    std::unique_ptr<PlainTable > dstTable(new PlainTable(tupleCount, dstSchema, getSortOrder()));


    for(uint32_t i = 0; i < tupleCount; ++i)  {
        PlainTuple dstTuple = tuples_[i].reveal(empParty);
        dstTable->putTuple(i, dstTuple);

    }

    return dstTable;


}

// iterate over all tuples and produce one long bit array for encrypting/decrypting in emp
// only works in PUBLIC or XOR mode
template <typename B>
vector<int8_t> QueryTable<B>::serialize() const {
    // dst size is in bits
    size_t tupleWidth =  schema_.size() / 8;
    size_t dstSize = getTupleCount() * tupleWidth;
    vector<int8_t> dst;
    dst.resize(dstSize);
    int8_t *cursor = dst.data();

    for(uint32_t i = 0; i < getTupleCount(); ++i) {
        QueryTuple<B> src = tuples_[i];
        src.serialize(cursor, schema_);
        cursor += tupleWidth;
    }

    return dst;
}

std::ostream &vaultdb::operator<<(std::ostream &os, const PlainTable &table) {


        os <<  table.getSchema() << " isEncrypted? " << table.isEncrypted() << endl;

        for(uint32_t i = 0; i < table.getTupleCount(); ++i) {
            os << table[i];
            bool isDummy = table.getTuple(i).getDummyTag();
            if(!isDummy)
                os << endl;

        }

        return os;

}

std::ostream &vaultdb::operator<<(std::ostream &os, const SecureTable &table) {


    os <<  table.getSchema() << " isEncrypted? " << table.isEncrypted() << endl;

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
    os <<  getSchema() << " isEncrypted? " << isEncrypted() << endl;

    for(uint32_t i = 0; i < getTupleCount(); ++i) {
        os << tuples_[i].toString(showDummies) << endl;

    }

    return os.str();

}

template <typename B>
QueryTable<B> & QueryTable<B>::operator=(const QueryTable<B> & src) {
    if(&src == this)
        return *this;

    setSchema(src.getSchema());

    tuples_.resize(getTupleCount());


    for(uint32_t i = 0; i < getTupleCount(); ++i) {
        tuples_[i] = src.tuples_[i];
    }

    return *this;
}

template <typename B>
void QueryTable<B>::putTuple(const int &idx, const QueryTuple<B> & tuple) {
    tuples_[idx]  = tuple;
}


template <typename B>
QueryTable<B>::QueryTable(const QueryTable<B> &src) : orderBy(src.getSortOrder()), schema_(src.getSchema()) {
    /* std::cout << "Instantiating a query table at: " << std::endl
               << Utilities::getStackTrace();*/
    tuples_.resize(src.getTupleCount());

    for(uint32_t i = 0; i < src.getTupleCount(); ++i) {
        tuples_[i] = src.tuples_[i];
    }

}


template <typename B>
QueryTuple<B> *QueryTable<B>::getTuplePtr(const int &idx) const {
    return ((QueryTuple<B> *) tuples_.data()) + idx;
}

template <typename B>
bool QueryTable<B>::operator==(const QueryTable<B> &other) const {



    if(getSchema() != other.getSchema()) {
        std::cout << "Failed to match on schema: \n" << getSchema()  << "\n  == \n" << other.getSchema() << std::endl;
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


    for(uint32_t i = 0; i < getTupleCount(); ++i) {
        QueryTuple<B> *thisTuple = getTuplePtr(i);
        QueryTuple<B> *otherTuple = other.getTuplePtr(i);

        if(*thisTuple != *otherTuple) {
            std::cout << "Comparing on idx " << i << " with "  << thisTuple->toString(true) << "\n          !=            " << otherTuple->toString(true) << endl;
            std::cout << "    Failed to match!" << std::endl;
           return false;
        }

    }


    return true;
}

template <typename B>
uint32_t QueryTable<B>::getTrueTupleCount() const {
    if(isEncrypted())
        return getTupleCount(); // encrypted -- can't reveal true count

    uint32_t count = 0;

    for (auto pos = begin(tuples_); pos != end(tuples_); ++pos) {
        bool dummyTag = Field<B>(pos->getDummyTag()).template getValue<bool>();

        if (!dummyTag) {
            ++count;
        }
    }


    return count;
}


template<typename B>
std::shared_ptr<SecureTable> QueryTable<B>::secretShare(emp::NetIO *netio, const int & party) const {

    size_t aliceSize = this->getTupleCount();
    size_t bobSize = aliceSize;
    int colCount = this->getSchema().getFieldCount();
    SecureTuple dstTuple;

    if (party == ALICE) {
        netio->send_data(&aliceSize, 4);
        netio->flush();
        netio->recv_data(&bobSize, 4);
        netio->flush();
    } else if (party == BOB) {
        netio->recv_data(&aliceSize, 4);
        netio->flush();
        netio->send_data(&bobSize, 4);
        netio->flush();
    }


    std::shared_ptr<SecureTable> dstTable(new SecureTable(aliceSize + bobSize, colCount));

    dstTable->setSchema(QuerySchema::toSecure(getSchema()));
    dstTable->setSortOrder(getSortOrder());


    // read alice in order
    for (size_t i = 0; i < aliceSize; ++i) {
        const QueryTuple<B>  *srcTuple = (party == ALICE) ? &tuples_[i] : nullptr;
        dstTuple = SecureTuple::secretShare(srcTuple, schema_, party, emp::ALICE);
        dstTable->putTuple(i, dstTuple);

    }


    int writeIdx = aliceSize;
    // write bob last --> first to make bitonic sequence
    int readTuple = bobSize; // last tuple

    for (size_t i = 0; i < bobSize; ++i) {
        --readTuple;
        const QueryTuple<B> *srcTuple = (party == BOB) ? &tuples_[readTuple] : nullptr;

        //if(party == BOB)
        //    std::cout << "Secret sharing: " << *srcTuple << std::endl;
        dstTuple = QueryTuple<B>::secretShare(srcTuple, schema_,  party, emp::BOB);
        //std::string revealed = dstTuple.reveal(PUBLIC).toString(false);
        //std::cout << "Encrypted: " << dstTuple.reveal(PUBLIC) << std::endl;

        // if(empParty_ == BOB)  assert(revealed == srcTuple->toString());

        dstTable->putTuple(writeIdx, dstTuple);
        ++writeIdx;
    }

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
    auto *cursor = const_cast<int8_t *>(tableBits.data());
    uint32_t tableSize = tableBits.size(); // in bytes
    uint32_t tupleSize = schema.size() / 8; // in bytes
    uint32_t tupleCount = tableSize / tupleSize;
    SortDefinition emptySortDefinition;

    std::shared_ptr<PlainTable > result(new PlainTable(tupleCount, schema, emptySortDefinition));

    for(uint32_t i = 0; i < tupleCount; ++i) {
        PlainTuple aTuple = QueryTuple<bool>::deserialize(schema, cursor);
        result->putTuple(i, aTuple);
        cursor += tupleSize;
    }

    return result;


}

template<typename B>
std::shared_ptr<SecureTable >
QueryTable<B>::deserialize(const QuerySchema &schema, vector<Bit> &tableBits) {
    Bit *cursor =  tableBits.data();
    uint32_t tableSize = tableBits.size(); // in bits
    uint32_t tupleSize = schema.size(); // in bits
    uint32_t tupleCount = tableSize / tupleSize;
    SortDefinition emptySortDefinition;

    QuerySchema encryptedSchema = QuerySchema::toSecure(schema);
    std::shared_ptr<SecureTable> result(new SecureTable(tupleCount, encryptedSchema, emptySortDefinition));

    for(uint32_t i = 0; i < tupleCount; ++i) {
        SecureTuple aTuple = QueryTuple<emp::Bit>::deserialize(encryptedSchema, cursor);
        //std::cout << "Deserialized " << aTuple.reveal().toString(true);
        result->putTuple(i, aTuple);
        cursor += tupleSize;
    }

    return result;

}

template<typename B>
void QueryTable<B>::resize(const size_t &tupleCount) {
    tuples_.resize(tupleCount);
}


template class vaultdb::QueryTable<bool>;
template class vaultdb::QueryTable<emp::Bit>;
