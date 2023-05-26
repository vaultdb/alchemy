#include "secure_tuple.h"
#include "field/field_factory.h"

using namespace vaultdb;

std::ostream &vaultdb::operator<<(std::ostream &strm,  const SecureTuple &aTuple) {

    strm << aTuple.toString(false);

    return strm;


}

QueryTuple<emp::Bit>::QueryTuple(std::shared_ptr<QuerySchema> &query_schema, emp::Bit *src) : fields_(src), query_schema_(query_schema) {
    assert(fields_ != nullptr);

}

QueryTuple<emp::Bit>::QueryTuple(std::shared_ptr<QuerySchema> &query_schema, int8_t *src) : fields_((emp::Bit *) src), query_schema_(query_schema) {
    assert(fields_ != nullptr);

}

QueryTuple<emp::Bit>::QueryTuple(const std::shared_ptr<QuerySchema> &query_schema, const int8_t *src) :  query_schema_(query_schema) {
    int8_t *tmp = const_cast<int8_t *>(src);
    fields_ = (emp::Bit *) tmp;
    assert(fields_ != nullptr);

}









QueryTuple<bool>
QueryTuple<emp::Bit>::reveal(const int &empParty, std::shared_ptr<QuerySchema> & dst_schema, int8_t *dst) const {
    QueryTuple<bool> dst_tuple(dst_schema, dst);

    emp::Bit dummy_tag = getDummyTag();
    bool plain_dummy_tag = dummy_tag.reveal(empParty);
    dst_tuple.setDummyTag(plain_dummy_tag);
    if(plain_dummy_tag) // reveal nothing else
        return dst_tuple;
	
    for(size_t i = 0; i < dst_schema->getFieldCount(); ++i) {
        SecureField src = getField(i);
        PlainField revealed = src.reveal(empParty);
        dst_tuple.setField(i, revealed);
    }

    

    return dst_tuple;

}

// self-managed tuple
PlainTuple QueryTuple<emp::Bit>::reveal(const std::shared_ptr<QuerySchema> & dst_schema, const int &empParty) const {
    PlainTuple dst_tuple(dst_schema);


    for(size_t i = 0; i < dst_schema->getFieldCount(); ++i) {
        SecureField src = getField(i);
        PlainField revealed = src.reveal(empParty);
        dst_tuple.setField(i, revealed);
    }

    emp::Bit dummy_tag = getDummyTag();
    bool plain_dummy_tag = dummy_tag.reveal(empParty);
    dst_tuple.setDummyTag(plain_dummy_tag);

    return dst_tuple;

}


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

SecureTuple QueryTuple<emp::Bit>::deserialize(emp::Bit *dst_tuple_bits, std::shared_ptr<QuerySchema> &schema,
                                              const emp::Bit *src_tuple_bits) {
    SecureTuple result(schema, dst_tuple_bits);
    size_t tuple_size = schema->size() * TypeUtilities::getEmpBitSize();
    memcpy(dst_tuple_bits, src_tuple_bits, tuple_size);
    return result;

}

std::shared_ptr<QuerySchema> QueryTuple<emp::Bit>::getSchema() const {
    return query_schema_;
}





QueryTuple<emp::Bit> &QueryTuple<emp::Bit>::operator=(const SecureTuple &other) {
    assert(*this->getSchema() == *(other.getSchema()));
    if(&other == this)
        return *this;

    emp::Bit *dst = this->fields_;
    emp::Bit *src = other.fields_;

    memcpy(dst, src, query_schema_->size() * TypeUtilities::getEmpBitSize());

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

SecureField QueryTuple<emp::Bit>::operator[](const int32_t &idx)  {
    return getField(idx);
}

const SecureField QueryTuple<emp::Bit>::operator[](const int32_t &idx)  const {
    return getField(idx);
}


emp::Bit QueryTuple<emp::Bit>::operator==(const SecureTuple &other) const {
    emp::Bit matched(true);
    assert(*this->getSchema() == *other.getSchema() );
    size_t bit_count = query_schema_->size();
    emp::Bit *lhs = this->fields_;
    emp::Bit *rhs = other.fields_;

    for(size_t i = 0; i < bit_count; ++i) {
        matched = matched & (*lhs == *rhs);
        ++lhs;
        ++rhs;
    }

    return matched;
}



QueryTuple<emp::Bit>::QueryTuple(const std::shared_ptr<QuerySchema> &schema) {
    managed_data_ = new emp::Bit[schema->size()];

    fields_ = managed_data_;

    // Warning: this will cause issues if used for mutable tuples
    // only doing shallow copy of schema!
    // formerly:
    // query_schema_ = std::make_shared<QuerySchema>(schema);
    // thus const'ing it above
    query_schema_ = schema;
}

QueryTuple<emp::Bit>::QueryTuple(const QueryTuple & src) {
        query_schema_ = src.getSchema();
        if(src.hasManagedStorage()) { // allocate storage for this copy
            managed_data_ = new emp::Bit[query_schema_->size()];
            fields_ = managed_data_;

            emp::Bit *d = this->fields_;
            emp::Bit *s = src.fields_;
            // initialize vals
            memcpy(d, s, query_schema_->size() * TypeUtilities::getEmpBitSize());
        }
        else
            fields_ = src.fields_; // point to the original fields

}

void QueryTuple<emp::Bit>::writeSubset(const SecureTuple &src_tuple, const SecureTuple &dst_tuple, uint32_t src_start_idx,
                                       uint32_t src_attr_cnt, uint32_t dst_start_idx) {

    size_t src_field_offset = src_tuple.getSchema()->getFieldOffset(src_start_idx);
    size_t dst_field_offset = dst_tuple.getSchema()->getFieldOffset(dst_start_idx);

    size_t write_size = 0;
    for(uint32_t i = src_start_idx; i < src_start_idx + src_attr_cnt; ++i) {
        write_size += src_tuple.getSchema()->getField(i).size();
    }

    assert(dst_field_offset + write_size <= dst_tuple.query_schema_->size());

    emp::Bit *read_pos = src_tuple.fields_ + src_field_offset;
    emp::Bit *write_pos = dst_tuple.fields_ + dst_field_offset;

    memcpy(write_pos, read_pos, write_size * sizeof(emp::Bit));

}

SecureTuple QueryTuple<emp::Bit>::If(const Bit &cond, const SecureTuple &lhs, const SecureTuple &rhs) {
    assert(*lhs.query_schema_ == *rhs.query_schema_);
    SecureTuple output(lhs.getSchema());

    for(int i = 0; i < output.query_schema_->getFieldCount(); ++i) {
        output[i] = SecureField::If(cond, lhs[i], rhs[i]);
    }

    emp::Bit dummy_tag = emp::If(cond, lhs.getDummyTag(), rhs.getDummyTag());
    output.setDummyTag(dummy_tag);
    return output;
}






