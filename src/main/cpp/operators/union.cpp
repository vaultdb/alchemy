#include "union.h"

using namespace vaultdb;


template<typename B>
Union<B>::Union(Operator<B> *lhs, Operator<B> *rhs) : Operator<B>(lhs, rhs) {
    if(lhs == nullptr || rhs == nullptr) {
        throw std::invalid_argument("Union can't take in null child operators!");
    }
    Operator<B>::sort_definition_.clear(); // union will change collation
    Operator<B>::output_schema_ = lhs->getOutputSchema();
}

template<typename B>
Union<B>::Union(QueryTable<B> *lhs, QueryTable<B> *rhs) : Operator<B>(lhs, rhs) {
    if(lhs == nullptr || rhs == nullptr) {
        throw std::invalid_argument("Union can't take in null tables!");
    }
    Operator<B>::output_schema_  = lhs->getSchema();
    Operator<B>::sort_definition_.clear();

}




template<typename B>
QueryTable<B> * Union<B>::runSelf() {
    QueryTable<B> *lhs = Operator<B>::getChild(0)->getOutput();
    QueryTable<B> *rhs = Operator<B>::getChild(1)->getOutput();

    assert(lhs->getSchema() == rhs->getSchema()); // union compatible
    QuerySchema output_schema = lhs->getSchema();

    // copy array
    size_t tuple_count = lhs->getTupleCount() + rhs->getTupleCount();
    size_t tuple_size_bytes = lhs->tuple_size_;

    Operator<B>::output_ = new QueryTable<B>(tuple_count, output_schema); // intentionally empty sort definition

    int8_t *write_ptr = Operator<B>::output_->tuple_data_.data();
    int8_t *read_ptr = lhs->tuple_data_.data();
    size_t write_size = tuple_size_bytes * lhs->getTupleCount();

    memcpy(write_ptr, read_ptr, write_size);

    write_ptr += write_size;
    read_ptr = rhs->tuple_data_.data();
    write_size = tuple_size_bytes * rhs->getTupleCount();

    memcpy(write_ptr, read_ptr, write_size);

    return Operator<B>::output_;
}

template<typename B>
string Union<B>::getOperatorType() const {
    return "Union";
}

template<typename B>
string Union<B>::getParameters() const {
    return std::string();
}


template class vaultdb::Union<bool>;
template class vaultdb::Union<emp::Bit>;
