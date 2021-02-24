#include <util/emp_manager.h>
#include "secret_share/prg.h"
#include <memory>
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



QueryTable::QueryTable(const int &num_tuples, const QuerySchema &schema, const SortDefinition &sortDefinition)
        :  orderBy(sortDefinition), schema_(schema) {

    tuples_.resize(num_tuples);

    for(int i = 0; i < num_tuples; ++i) {
        tuples_[i].setFieldCount(schema.getFieldCount()); // initialize tuples
    }
}



QueryTable::QueryTable(const int &num_tuples, const int &colCount)
    : schema_(QuerySchema(colCount)) {
    tuples_.resize(num_tuples);

    for(int i = 0; i < num_tuples; ++i) {
        tuples_[i].setFieldCount(colCount); // initialize tuples
    }


}

const bool QueryTable::isEncrypted() const {
    types::TypeId firstColType = schema_.getField(0).getType();

    // if encrypted version of this column is the same as its original value
    return TypeUtilities::isEncrypted(firstColType);
}



unique_ptr<QueryTable> QueryTable::reveal(int empParty) const  {
    uint32_t tupleCount = getTupleCount();

    if(!this->isEncrypted())
        return make_unique<QueryTable>(*this);

    QuerySchema dstSchema = QuerySchema::toPlain(getSchema());

    unique_ptr<QueryTable> dstTable(new QueryTable(tupleCount, dstSchema, getSortOrder()));

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

ostream &vaultdb::operator<<(ostream &os, const QueryTable &table) {


    os <<  table.getSchema() << " isEncrypted? " << table.isEncrypted() << endl;

    for(int i = 0; i < table.getTupleCount(); ++i) {
        os << table.getTuple(i);


        bool isEncrypted = table.isEncrypted();
        if(isEncrypted) {
            os << endl;
        }
        else {
            bool isDummy = table.getTuple(i).getDummyTag().getBool();
            if(!isDummy)
                os << endl;
        }


    }

    return os;
}

string QueryTable::toString(const bool & showDummies) const {

    ostringstream os;

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


    tuples_.resize(src.getTupleCount());

    for(uint32_t i = 0; i < src.getTupleCount(); ++i) {
        tuples_[i] = src.tuples_[i];
    }

}

void QueryTable::setTupleDummyTag(const int &tupleIdx, const types::Value & dummyTag) {
    tuples_[tupleIdx].setDummyTag(dummyTag);
}

QueryTuple *QueryTable::getTuplePtr(const int &idx) const {
    return ((QueryTuple *) tuples_.data()) + idx;
}

bool QueryTable::operator==(const QueryTable &other) const {



    if(getSchema() != other.getSchema()) {
       cout << "Failed to match on schema: \n" << getSchema()  << "\n  == \n" << other.getSchema() << endl;
        return false;
    }

    if(this->getSortOrder() != other.getSortOrder()) {
        cout << "Failed to match on sort order expected="  << DataUtilities::printSortDefinition(this->getSortOrder())
                  << "observed=" << DataUtilities::printSortDefinition(other.getSortOrder()) <<  endl;
        return false;
    }
    if(this->getTupleCount() != other.getTupleCount()) {
        cout << "Failed to match on tuple count " << this->getTupleCount() << " vs " << other.getTupleCount() << endl;
        return false;
    }


    for(uint32_t i = 0; i < getTupleCount(); ++i) {
        QueryTuple *thisTuple = getTuplePtr(i);
        QueryTuple *otherTuple = other.getTuplePtr(i);
       //cout << "Comparing "  << thisTuple->toString(true) << "\n    to    " << otherTuple->toString(true) << endl;

        if(*thisTuple != *otherTuple) {
            cout << "Comparing on idx " << i << " with "  << thisTuple->toString(true) << "\n           !=            " << otherTuple->toString(true) << endl;
            cout << "    Failed to match!" << endl;
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
        if (!((*pos).getDummyTag().getBool())) {
            ++count;
        }
    }


    return count;
}

shared_ptr<QueryTable> QueryTable::secretShare(emp::NetIO *io, const int & party) const {
    return EmpManager::secretShareTable(this, io, party);
}

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

void QueryTable::setSortOrder(const SortDefinition &sortOrder) {
    orderBy = sortOrder;

}

SortDefinition QueryTable::getSortOrder() const {
    return orderBy;
}

shared_ptr<QueryTable> QueryTable::deserialize(const QuerySchema &schema, const vector<int8_t> & tableBits) {
    int8_t *cursor = const_cast<int8_t *>(tableBits.data());
    uint32_t tableSize = tableBits.size(); // in bytes
    uint32_t tupleSize = schema.size() / 8; // in bytes
    uint32_t tupleCount = tableSize / tupleSize;
    SortDefinition emptySortDefinition;

    cout << "Deserializing " << tupleCount << " tuples." << endl;
    shared_ptr<QueryTable> result(new QueryTable(tupleCount, schema, emptySortDefinition));

    for(int i = 0; i < tupleCount; ++i) {
        QueryTuple aTuple = QueryTuple::deserialize(schema, cursor);
        result->putTuple(i, aTuple);
        cursor += tupleSize;
    }

    return result;


}

shared_ptr<QueryTable>
QueryTable::deserialize(const QuerySchema &schema, vector<Bit> &tableBits) {
    Bit *cursor =  tableBits.data();
    uint32_t tableSize = tableBits.size(); // in bits
    uint32_t tupleSize = schema.size(); // in bits
    uint32_t tupleCount = tableSize / tupleSize;
    SortDefinition emptySortDefinition;


    QuerySchema encryptedSchema = QuerySchema::toSecure(schema);
    shared_ptr<QueryTable> result(new QueryTable(tupleCount, encryptedSchema, emptySortDefinition));


    for(int i = 0; i < tupleCount; ++i) {
        QueryTuple aTuple = QueryTuple::deserialize(encryptedSchema, cursor);
        result->putTuple(i, aTuple);
        cursor += tupleSize;
    }

    return result;

}

void QueryTable::resize(const size_t &tupleCount) {
    tuples_.resize(tupleCount);
}

