#include <memory>
#include <utility>
#include <util/data_utilities.h>
#include <util/type_utilities.h>
#include "query_table.h"

using namespace vaultdb;

QueryTuple QueryTable::getTuple(int idx) const {
    return tuples_[idx];
}

void QueryTable::setSchema(const QuerySchema & s) {
    schema_ = s;
}


const QuerySchema & QueryTable::getSchema() const { return schema_; }

unsigned int QueryTable::getTupleCount() const {
    return tuples_.size();
}



QueryTable::QueryTable(const size_t &num_tuples, const QuerySchema &schema, const SortDefinition & sortDefinition)
        :  orderBy(std::move(sortDefinition)), schema_(schema) {

   /* std::cout << "Instantiating a query table at: " << std::endl
              << Utilities::getStackTrace();*/
    tuples_.reserve(num_tuples);

    for(size_t i = 0; i < num_tuples; ++i) {
        tuples_.emplace_back(QueryTuple(schema.getFieldCount()));
    }
}



QueryTable::QueryTable(const size_t &num_tuples, const int &colCount)
    : schema_(QuerySchema(colCount)) {

    /* std::cout << "Instantiating a query table at: " << std::endl
               << Utilities::getStackTrace();*/
    tuples_.reserve(num_tuples);

    for(size_t i = 0; i < num_tuples; ++i) {
        tuples_.emplace_back(QueryTuple(colCount));
    }

}

bool QueryTable::isEncrypted() const {
    FieldType firstColType = schema_.getField(0).getType();

    // if encrypted version of this column is the same as its original value
    return TypeUtilities::isEncrypted(firstColType);
}



std::unique_ptr<QueryTable> QueryTable::reveal(int empParty) const  {
    uint32_t tupleCount = getTupleCount();

    if(!this->isEncrypted())
        return std::make_unique<QueryTable>(*this);

    QuerySchema dstSchema = QuerySchema::toPlain(getSchema());

    std::unique_ptr<QueryTable> dstTable(new QueryTable(tupleCount, dstSchema, getSortOrder()));

    QueryTuple srcTuple; // initialized below

    for(uint32_t i = 0; i < tupleCount; ++i)  {
        srcTuple = getTuple(i);

        QueryTuple dstTuple = srcTuple.reveal(empParty);
        dstTable->putTuple(i, dstTuple);

    }

    return dstTable;


}

// iterate over all tuples and produce one long bit array for encrypting/decrypting in emp
// only works in PUBLIC or XOR mode
vector<int8_t> QueryTable::serialize() const {
    // dst size is in bits
    size_t tupleWidth =  schema_.size() / 8; // 496 / 8  = 62
    size_t dstSize = getTupleCount() * tupleWidth;
    vector<int8_t> dst;
    dst.resize(dstSize);
    int8_t *cursor = dst.data();

    for(uint32_t i = 0; i < getTupleCount(); ++i) {
        getTuplePtr(i)->serialize(cursor, schema_);
        cursor += tupleWidth;
    }

    return dst;
}

std::ostream &vaultdb::operator<<(std::ostream &os, const QueryTable &table) {


    os <<  table.getSchema() << " isEncrypted? " << table.isEncrypted() << endl;

    for(uint32_t i = 0; i < table.getTupleCount(); ++i) {
        os << table.getTuple(i);


        bool isEncrypted = table.isEncrypted();
        if(isEncrypted) {
            os << endl;
        }
        else {
            bool isDummy = (table.getTuple(i).getDummyTag())->getValue<bool>();
            if(!isDummy)
                os << endl;
        }


    }

    return os;
}

string QueryTable::toString(const bool & showDummies) const {

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


QueryTable & QueryTable::operator=(const QueryTable & src) {
    if(&src == this)
        return *this;

    setSchema(src.getSchema());

    tuples_.resize(getTupleCount());


    for(uint32_t i = 0; i < getTupleCount(); ++i) {
        tuples_[i] = src.tuples_[i];
    }

    return *this;
}

void QueryTable::putTuple(const int &idx, const QueryTuple & tuple) {
    tuples_[idx]  = tuple;
}


QueryTable::QueryTable(const QueryTable &src) : orderBy(src.getSortOrder()), schema_(src.getSchema()) {
    /* std::cout << "Instantiating a query table at: " << std::endl
               << Utilities::getStackTrace();*/
    tuples_.resize(src.getTupleCount());

    for(uint32_t i = 0; i < src.getTupleCount(); ++i) {
        tuples_[i] = src.tuples_[i];
    }

}


QueryTuple *QueryTable::getTuplePtr(const int &idx) const {
    return ((QueryTuple *) tuples_.data()) + idx;
}

bool QueryTable::operator==(const QueryTable &other) const {



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
        QueryTuple *thisTuple = getTuplePtr(i);
        QueryTuple *otherTuple = other.getTuplePtr(i);

        if(*thisTuple != *otherTuple) {
            std::cout << "Comparing on idx " << i << " with "  << thisTuple->toString(true) << "\n          !=            " << otherTuple->toString(true) << endl;
            std::cout << "    Failed to match!" << std::endl;
           return false;
        }

    }


    return true;
}

uint32_t QueryTable::getTrueTupleCount() const {
    if(isEncrypted())
        return getTupleCount(); // encrypted -- can't reveal true count

    uint32_t count = 0;

    for (auto pos = begin(tuples_); pos != end(tuples_); ++pos) {
        bool dummyTag = ((*pos).getDummyTag())->getValue<bool>();

        if (!dummyTag) {
            ++count;
        }
    }


    return count;
}


std::shared_ptr<QueryTable> QueryTable::secretShare(emp::NetIO *netio, const int & party) const {

    size_t aliceSize = this->getTupleCount();
    size_t bobSize = aliceSize;
    int colCount = this->getSchema().getFieldCount();
    QueryTuple dstTuple(colCount, true);

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

    std::shared_ptr<QueryTable> dstTable(new QueryTable(aliceSize + bobSize, colCount));

    dstTable->setSchema(QuerySchema::toSecure(getSchema()));
    dstTable->setSortOrder(getSortOrder());


    // read alice in order
    for (size_t i = 0; i < aliceSize; ++i) {
        const QueryTuple *srcTuple = (party == ALICE) ? &(tuples_[i]) : nullptr;
        dstTuple = QueryTuple::secretShare(srcTuple, schema_, party, emp::ALICE);
        dstTable->putTuple(i, dstTuple);

    }

    int writeIdx = aliceSize;
    // write bob last --> first to make bitonic sequence
    int readTuple = bobSize; // last tuple

    for (size_t i = 0; i < bobSize; ++i) {
        --readTuple;
        const QueryTuple *srcTuple = (party == BOB) ? &(tuples_[readTuple]) : nullptr;

        //if(party == BOB)
        //    std::cout << "Secret sharing: " << *srcTuple << std::endl;
        dstTuple = QueryTuple::secretShare(srcTuple, schema_,  party, emp::BOB);
        //std::string revealed = dstTuple.reveal(PUBLIC).toString(false);
        //std::cout << "Encrypted: " << dstTuple.reveal(PUBLIC) << std::endl;

        // if(empParty_ == BOB)  assert(revealed == srcTuple->toString());

        dstTable->putTuple(writeIdx, dstTuple);
        ++writeIdx;
    }

    netio->flush();

    return dstTable;

}
/*
// use this for acting as a data sharing party in the PDF
// generates alice and bob's shares and returns the pair
SecretShares QueryTable::generateSecretShares() const {
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
 */

void QueryTable::setSortOrder(const SortDefinition &sortOrder) {
    orderBy = sortOrder;

}

SortDefinition QueryTable::getSortOrder() const {
    return orderBy;
}

std::shared_ptr<QueryTable> QueryTable::deserialize(const QuerySchema &schema, const vector<int8_t> & tableBits) {
    auto *cursor = const_cast<int8_t *>(tableBits.data());
    uint32_t tableSize = tableBits.size(); // in bytes
    uint32_t tupleSize = schema.size() / 8; // in bytes
    uint32_t tupleCount = tableSize / tupleSize;
    SortDefinition emptySortDefinition;

    std::cout << "Deserializing " << tupleCount << " tuples." << std::endl;
    std::shared_ptr<QueryTable> result(new QueryTable(tupleCount, schema, emptySortDefinition));

    for(uint32_t i = 0; i < tupleCount; ++i) {
        QueryTuple aTuple = QueryTuple::deserialize(schema, cursor);
        result->putTuple(i, aTuple);
        cursor += tupleSize;
    }

    return result;


}

std::shared_ptr<QueryTable>
QueryTable::deserialize(const QuerySchema &schema, vector<Bit> &tableBits) {
    Bit *cursor =  tableBits.data();
    uint32_t tableSize = tableBits.size(); // in bits
    uint32_t tupleSize = schema.size(); // in bits
    uint32_t tupleCount = tableSize / tupleSize;
    SortDefinition emptySortDefinition;


    QuerySchema encryptedSchema = QuerySchema::toSecure(schema);
    std::shared_ptr<QueryTable> result(new QueryTable(tupleCount, encryptedSchema, emptySortDefinition));


    for(uint32_t i = 0; i < tupleCount; ++i) {
        QueryTuple aTuple = QueryTuple::deserialize(encryptedSchema, (int8_t *) cursor);
        result->putTuple(i, aTuple);
        cursor += tupleSize;
    }

    return result;

}

void QueryTable::resize(const size_t &tupleCount) {
    tuples_.resize(tupleCount);
}

