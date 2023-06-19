#include "plain_tuple.h"
#include "field/field.h"


using namespace vaultdb;

std::ostream &vaultdb::operator<<(std::ostream &strm,  const QueryTuple<bool> &aTuple) {

    strm << aTuple.toString(false);

    return strm;


}

QueryTuple<bool>::QueryTuple(QuerySchema *query_schema, int8_t *table_data)
        : fields_(table_data), schema_(query_schema) {
    assert(fields_ != nullptr);
}

QueryTuple<bool>::QueryTuple(QuerySchema *query_schema, const int8_t *table_data)
        : schema_(query_schema) {
    fields_ = const_cast<int8_t *>(table_data);
    assert(fields_ != nullptr);
}






void QueryTuple<bool>::setField(const int &idx, const PlainField &f) {
    size_t field_offset = schema_->getFieldOffset(idx) / 8;
    int8_t *write_pos = fields_ + field_offset;
    f.serialize(write_pos, schema_->getField(idx));
}




void QueryTuple<bool>::compareSwap(const bool &cmp, PlainTuple &lhs, PlainTuple &rhs) {

    if(cmp) {
        assert(*lhs.getSchema() == *rhs.getSchema());
        size_t byte_count = lhs.getSchema()->size()/8;
        int8_t *l = lhs.getData();
        int8_t *r = rhs.getData();

        // XOR swap
        for(size_t i = 0; i < byte_count; ++i) {
            l[i] = l[i] ^ r[i];
            r[i] = r[i] ^ l[i];
            l[i] = l[i] ^ r[i];
        }

    }
}

PlainTuple QueryTuple<bool>::deserialize(int8_t *dst_bits, QuerySchema *schema, int8_t *src_bits) {
    PlainTuple result(schema, dst_bits);

    size_t tuple_size = schema->size();
    memcpy(dst_bits, src_bits, tuple_size);
    return result;

}

bool QueryTuple<bool>::operator==(const PlainTuple &other) const {


    if(*schema_ != *other.getSchema())
        return false;

    // if both dummies, then ok
    if(this->getDummyTag() && other.getDummyTag()) return true;

    for(size_t i = 0; i < schema_->getFieldCount(); ++i) {
        PlainField lhs_field = getField(i);
        PlainField  rhs_field = other.getField(i);
        if(lhs_field != rhs_field) {
            return false;
        }
    }

    bool lhs_dummy_tag = getDummyTag();
    bool rhs_dummy_tag = other.getDummyTag();
    if(lhs_dummy_tag != rhs_dummy_tag) {
        return false;
    }
    return true;
}

string QueryTuple<bool>::toString(const bool &showDummies) const {
    std::stringstream sstream;
    PlainField f;

    bool printValue = showDummies || !getDummyTag();

    if(printValue) {
        f = getField(0);
        sstream <<   "(" <<  f;

        for (size_t i = 1; i < schema_->getFieldCount(); ++i) {
            f = getField(i);
            sstream << ", " << f;

        }

        sstream << ")";
    }

    if(showDummies) {
        sstream <<  " (dummy=" << getDummyTag()  << ")";
    }

    return sstream.str();
}




QueryTuple<bool>::QueryTuple(QuerySchema *schema) {
    size_t tuple_byte_cnt = schema->size()/8;
    managed_data_ = new int8_t[tuple_byte_cnt];
    memset(managed_data_, 0, tuple_byte_cnt); // zero out memory (for dummy tuples
    fields_ = managed_data_;
    schema_ = schema;


}

QueryTuple<bool>::QueryTuple(const QueryTuple<bool> &src) {
    schema_ = src.getSchema();
    if(src.hasManagedStorage()) { // allocate storage for this copy
        size_t tuple_byte_cnt = schema_->size() / 8;
        managed_data_ = new int8_t[tuple_byte_cnt];
        fields_ = managed_data_;
    }
    memcpy(fields_, src.fields_, schema_->size() / 8);

}



PlainTuple &QueryTuple<bool>::operator=(const PlainTuple &other) {
    assert(*this->getSchema() == *(other.getSchema()));

    if(&other == this)
        return *this;

    int8_t *dst = getData();
    int8_t *src = other.getData();

    memcpy(dst, src, schema_->size() / 8);

    return *this;


}

// party is ignored
PlainTuple QueryTuple<bool>::reveal(const int & party) const {
    PlainTuple tuple(this->getSchema());
    tuple = *this;
    return tuple;
}

void QueryTuple<bool>::writeSubset(const PlainTuple &src_tuple, const PlainTuple &dst_tuple, uint32_t src_start_idx,
                                   uint32_t src_attr_cnt, uint32_t dst_start_idx) {

    size_t src_field_offset = src_tuple.getSchema()->getFieldOffset(src_start_idx)/8;
    size_t dst_field_offset = dst_tuple.getSchema()->getFieldOffset(dst_start_idx)/8;

    size_t write_size = 0;
    for(uint32_t i = src_start_idx; i < src_start_idx + src_attr_cnt; ++i) {
        write_size += src_tuple.getSchema()->getField(i).size()/8;
    }

    assert(dst_field_offset + write_size <= dst_tuple.schema_->size() / 8);

    int8_t *read_pos = src_tuple.fields_ + src_field_offset;
    int8_t *write_pos = dst_tuple.fields_ + dst_field_offset;

    memcpy(write_pos, read_pos, write_size);

}


PlainField QueryTuple<bool>::operator[](const int32_t &idx) {
    return getField(idx);
}

const PlainField QueryTuple<bool>::operator[](const int32_t &idx) const {
    return getField(idx);
}
