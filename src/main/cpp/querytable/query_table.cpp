#include <util/emp_manager.h>
#include "query_table.h"
#include "../data/proto_converter.h"

QueryTuple QueryTable::getTuple(int idx) const {
    return tuples_[idx];
}

void QueryTable::setSchema(const QuerySchema & s) {
    schema_ = s;
}


const QuerySchema & QueryTable::getSchema() const { return schema_; }

unsigned int QueryTable::getTupleCount() const {
    return tupleCount_;
}



QueryTable::QueryTable(const int & num_tuples, const  int & colCount, const bool & is_encrypted)
    : schema_(QuerySchema(colCount)), is_encrypted_(is_encrypted),  tupleCount_(num_tuples) {
    tuples_.resize(tupleCount_);

    for(int i = 0; i < tupleCount_; ++i) {
        tuples_[i].setFieldCount(colCount); // initialize tuples
    }


}

const bool QueryTable::isEncrypted() const { return is_encrypted_; }



std::unique_ptr<QueryTable> QueryTable::reveal(int empParty) const  {
    int colCount = getSchema().getFieldCount();
    uint32_t tupleCount = getTupleCount();
    bool isEncrypted = (empParty == emp::XOR);


    std::unique_ptr<QueryTable> dstTable(new QueryTable(tupleCount, colCount, isEncrypted));
    dstTable->setSchema(schema_);
    QueryTuple srcTuple; // initialized below

    for(int i = 0; i < tupleCount; ++i)  {
        srcTuple = getTuple(i);

        QueryTuple dstTuple(colCount, false);
        dstTuple = srcTuple.reveal(empParty);
        dstTable->putTuple(i, dstTuple);

    }

    return dstTable;


}

// iterate over all tuples and produce one long bit array for encrypting/decrypting in emp
// only works in PUBLIC or XOR mode
bool *QueryTable::serialize() const {
    // dst size is in bits
    size_t tupleWidth =  schema_.size();
    size_t dstSize = tupleCount_ * tupleWidth;
    bool *dst = new bool[dstSize];
    bool *cursor = dst;

    for(int i = 0; i < tupleCount_; ++i) {
        ((QueryTuple *) tuples_.data())->serialize(cursor, schema_);
        cursor += tupleWidth;
    }

    return dst;
}

std::ostream &operator<<(std::ostream &os, const QueryTable &table) {


    os <<  table.getSchema() << " isEncrypted? " << table.is_encrypted_ << std::endl;

    for(int i = 0; i < table.getTupleCount(); ++i) {
        os << table.tuples_[i];


        bool isEncrypted = table.isEncrypted();
        if(isEncrypted) {
            os << std::endl;
        }
        else {
            bool isDummy = table.getTuple(i).getDummyTag().getBool();
            if(!isDummy)
                os << std::endl;
        }


    }

    return os;
}

std::string QueryTable::toString(const bool & showDummies) const {

    std::ostringstream os;

    if(!showDummies) {
        os << *this;
        return os.str();
    }

    // show dummies case
    os <<  getSchema() << " isEncrypted? " << is_encrypted_ << std::endl;

    for(int i = 0; i < getTupleCount(); ++i) {
        os << tuples_[i].toString(showDummies) << std::endl;

    }

    return os.str();

}


QueryTable & QueryTable::operator=(const QueryTable & src) {
    if(&src == this)
        return *this;

    setSchema(src.getSchema());
    this->is_encrypted_ = src.isEncrypted();
    this->tupleCount_ = src.getTupleCount();

    tuples_.resize(tupleCount_);
    //=
            //std::unique_ptr<QueryTuple[]>(new QueryTuple[tupleCount_]);

    for(int i = 0; i < tupleCount_; ++i) {
        tuples_[i] = src.tuples_[i];
    }

    return *this;
}

void QueryTable::putTuple(int idx, const QueryTuple & tuple) {
    tuples_[idx]  = tuple;
}


QueryTable::QueryTable(const QueryTable &src) : schema_(src.getSchema()) {


    this->is_encrypted_ = src.isEncrypted();
    this->tupleCount_ = src.getTupleCount();

    tuples_.resize(tupleCount_);

    for(int i = 0; i < tupleCount_; ++i) {
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
        return false;
    }

    if(this->getTupleCount() != other.getTupleCount()) {   return false; }


    for(int i = 0; i < getTupleCount(); ++i) {
        QueryTuple *thisTuple = getTuplePtr(i);
        QueryTuple *otherTuple = other.getTuplePtr(i);
        //std::cout << "Comparing "  << thisTuple->toString(true) << " to " << otherTuple->toString(true) << std::endl;

        if(*thisTuple != *otherTuple) {
           // std::cout << "    Failed to match!" << std::endl;
            return false;
        }

    }

    return true;
}

uint32_t QueryTable::getTrueTupleCount() const {
    if(isEncrypted())
        return getTupleCount();

    uint32_t count = 0;
    for(auto pos = std::begin(tuples_); pos != std::end(tuples_); ++pos) {
        if(!((*pos).getDummyTag().getBool())) {
            ++count;
        }
    }
    for(uint32_t i = 0; i < getTupleCount(); ++i) {

    }
    return count;
}

std::shared_ptr<QueryTable> QueryTable::secretShare(emp::NetIO *io, const int & party) const {
    return EmpManager::secretShareTable(this, io, party);
}


