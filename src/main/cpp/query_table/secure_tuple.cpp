#include "secure_tuple.h"
#include "field/field_factory.h"

using namespace vaultdb;

std::ostream &vaultdb::operator<<(std::ostream &strm,  const SecureTuple &aTuple) {
    strm << aTuple.toString(false);
    return strm;
}





QueryTuple<emp::Bit>::QueryTuple(QuerySchema *query_schema, const int8_t *src) : schema_(query_schema) {
    int8_t *tmp = const_cast<int8_t *>(src);
    fields_ = (emp::Bit *) tmp;
    initializeFieldOffsets();
    assert(fields_ != nullptr);
}




QueryTuple<bool>
QueryTuple<emp::Bit>::reveal(const int &party, QuerySchema *dst_schema, int8_t *dst) const {
    QueryTuple<bool> dst_tuple(dst_schema, dst);

    emp::Bit dummy_tag = getDummyTag();
    bool plain_dummy_tag = dummy_tag.reveal(party);
    dst_tuple.setDummyTag(plain_dummy_tag);
    if(plain_dummy_tag) // reveal nothing else
        return dst_tuple;

    for(size_t i = 0; i < dst_schema->getFieldCount(); ++i) {
        SecureField src = getField(i);
        PlainField revealed = src.reveal(schema_->getField(i), party);
        dst_tuple.setField(i, revealed);
    }
    return dst_tuple;

}

// self-managed tuple
PlainTuple QueryTuple<emp::Bit>::reveal(QuerySchema *dst_schema, const int &party) const {
    PlainTuple dst_tuple(dst_schema);

    emp::Bit dummy_tag = getDummyTag();
    bool plain_dummy_tag = dummy_tag.reveal(party);
    dst_tuple.setDummyTag(plain_dummy_tag);

    if(plain_dummy_tag) { // reveal nothing else
        return dst_tuple;
    }


    for(size_t i = 0; i < dst_schema->getFieldCount(); ++i) {
        SecureField src = getField(i);
        PlainField revealed = src.reveal(schema_->getField(i), party);
        dst_tuple.setField(i, revealed);
    }

    return dst_tuple;

}

PlainTuple QueryTuple<emp::Bit>::revealInsecure(QuerySchema *dst_schema, const int &party) const {
    PlainTuple dst_tuple(dst_schema);

    emp::Bit dummy_tag = getDummyTag();
    bool plain_dummy_tag = dummy_tag.reveal(party);
    dst_tuple.setDummyTag(plain_dummy_tag);

    for(size_t i = 0; i < dst_schema->getFieldCount(); ++i) {
        SecureField src = getField(i);
        PlainField revealed = src.reveal(schema_->getField(i), party);
        dst_tuple.setField(i, revealed);
    }

    return dst_tuple;

}

// TODO: update this for OMPC - model it after QueryTable's C&S
void QueryTuple<emp::Bit>::compareSwap(const Bit &cmp, SecureTuple  & lhs, SecureTuple  & rhs) {
    size_t tuple_size = lhs.getSchema()->size(); // size in bits

    emp::Integer lhs_payload(tuple_size, 0, emp::PUBLIC);
    emp::Integer rhs_payload(tuple_size, 0, emp::PUBLIC);

    memcpy(lhs_payload.bits.data(), lhs.getData(), tuple_size * TypeUtilities::getEmpBitSize());
    memcpy(rhs_payload.bits.data(), rhs.getData(), tuple_size * TypeUtilities::getEmpBitSize());

    emp::swap(cmp, lhs_payload, rhs_payload);

    memcpy(lhs.getData(), lhs_payload.bits.data(), tuple_size * TypeUtilities::getEmpBitSize());
    memcpy(rhs.getData(), rhs_payload.bits.data(), tuple_size * TypeUtilities::getEmpBitSize());



}

SecureTuple QueryTuple<emp::Bit>::deserialize(emp::Bit *dst_tuple_bits, QuerySchema *schema, const emp::Bit *src_tuple_bits) {
    SecureTuple result(schema, dst_tuple_bits);
    size_t tuple_size = schema->size() * TypeUtilities::getEmpBitSize();
    memcpy(dst_tuple_bits, src_tuple_bits, tuple_size);
    return result;

}






QueryTuple<emp::Bit> &QueryTuple<emp::Bit>::operator=(const SecureTuple &other) {
    assert(*this->getSchema() == *(other.getSchema()));
    if(&other == this)
        return *this;

    emp::Bit *dst = this->fields_;
    emp::Bit *src = other.fields_;

    memcpy(dst, src, schema_->size() * TypeUtilities::getEmpBitSize());

    return *this;

}



string QueryTuple<emp::Bit>::toString(const bool &showDummies) const {
    std::stringstream sstream;


    sstream <<   "(" <<  getField(0).toString();

    for (size_t i = 1; i < getFieldCount(); ++i)
        sstream << ", " << getField(i);

    sstream << ")";

    if(showDummies) {
        sstream <<  " (dummy=" << "SECRET BIT" << ")";
    }

    return sstream.str();
}


emp::Bit QueryTuple<emp::Bit>::operator==(const SecureTuple &other) const {
    emp::Bit matched(true);
    assert(*this->getSchema() == *other.getSchema() );

    size_t bit_count = schema_->size();
    emp::Bit *lhs = this->fields_;
    emp::Bit *rhs = other.fields_;

    for(size_t i = 0; i < bit_count; ++i) {
        matched = matched & (*lhs == *rhs);
        ++lhs;
        ++rhs;
    }

    return matched;
}



QueryTuple<emp::Bit>::QueryTuple(QuerySchema *schema) {
    managed_data_ = new emp::Bit[schema->bitCnt()];

    fields_ = managed_data_;
    schema_ = schema;
    initializeFieldOffsets();

    this->setDummyTag(true);
}

QueryTuple<emp::Bit>::QueryTuple(const QueryTuple & src) {
    schema_ = src.getSchema();
    field_offset_bytes_ = src.field_offset_bytes_;
    tuple_size_bytes_ = src.tuple_size_bytes_;

    if(src.hasManagedStorage()) { // allocate storage for this copy
        managed_data_ = new emp::Bit[schema_->size()];
        fields_ = managed_data_;

        emp::Bit *d = this->fields_;
        emp::Bit *s = src.fields_;
        // initialize vals
        memcpy(d, s, schema_->size() * TypeUtilities::getEmpBitSize());
    }
    else
        fields_ = src.fields_; // point to the original fields

}

SecureTuple QueryTuple<emp::Bit>::If(const Bit &cond, const SecureTuple &lhs, const SecureTuple &rhs) {
    assert(*lhs.schema_ == *rhs.schema_);
    SecureTuple output(lhs.getSchema());

    for(int i = 0; i < output.schema_->getFieldCount(); ++i) {
        output[i] = SecureField::If(cond, lhs[i], rhs[i]);
    }

    emp::Bit dummy_tag = emp::If(cond, lhs.getDummyTag(), rhs.getDummyTag());
    output.setDummyTag(dummy_tag);
    return output;
}





