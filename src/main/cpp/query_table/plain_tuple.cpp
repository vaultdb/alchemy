#include "plain_tuple.h"
#include "field/field.h"
#include "field/field_factory.h"


using namespace vaultdb;

std::ostream &vaultdb::operator<<(std::ostream &strm,  const QueryTuple<bool> &aTuple) {

    strm << aTuple.toString(false);

    return strm;


}

QueryTuple<bool>::QueryTuple(std::shared_ptr<QuerySchema>  & query_schema, int8_t *table_data)
        : fields_(table_data), query_schema_(query_schema) {
    assert(fields_ != nullptr);
}

QueryTuple<bool>::QueryTuple(const std::shared_ptr<QuerySchema>  & query_schema, const int8_t *table_data)
        : query_schema_(query_schema) {
    fields_ = const_cast<int8_t *>(table_data);
    assert(fields_ != nullptr);
}


PlainField QueryTuple<bool>::getField(const int &ordinal)  {
    size_t field_offset = query_schema_->getFieldOffset(ordinal)/8;
    QueryFieldDesc fieldDesc = query_schema_->getField(ordinal);

    return FieldFactory<bool>::deserialize(fieldDesc.getType(), fieldDesc.getStringLength(), fields_ + field_offset);
}


const PlainField QueryTuple<bool>::getField(const int &ordinal) const {
    size_t field_offset = query_schema_->getFieldOffset(ordinal)/8;
    QueryFieldDesc fieldDesc = query_schema_->getField(ordinal);
    return FieldFactory<bool>::deserialize(fieldDesc.getType(), fieldDesc.getStringLength(), fields_ + field_offset);
}


void QueryTuple<bool>::setField(const int &idx, const PlainField &f) {
    size_t field_offset = query_schema_->getFieldOffset(idx)/8;
    int8_t *writePos = fields_ + field_offset;
    f.serialize(writePos);

}

void QueryTuple<bool>::setDummyTag(const bool &b) {
    size_t dummy_tag_size = sizeof(bool);
    int8_t *dst = fields_ + query_schema_->size()/8 - dummy_tag_size;
    memcpy(dst, &b, dummy_tag_size);
}

void QueryTuple<bool>::setDummyTag(const Field<bool> &d) {
    assert(d.getType() == FieldType::BOOL);
    bool dummy_tag = d.getValue<bool>();
    setDummyTag(dummy_tag);
}

void QueryTuple<bool>::setSchema(std::shared_ptr<QuerySchema> q) {
    query_schema_ = q;
}

bool QueryTuple<bool>::getDummyTag() const {
    size_t dummy_tag_size = sizeof(bool);
    int8_t *src = fields_ + query_schema_->size()/8 - dummy_tag_size;
    return *((bool *) src);
}

std::shared_ptr<QuerySchema> QueryTuple<bool>::getSchema() const {
    return query_schema_;
}

void QueryTuple<bool>::compare_swap(const bool &cmp, PlainTuple &lhs, PlainTuple &rhs) {

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

PlainTuple QueryTuple<bool>::deserialize(int8_t *dst_bits, std::shared_ptr<QuerySchema> &schema, int8_t *src_bits) {
    PlainTuple result(schema, dst_bits);

    size_t tuple_size = schema->size();
    memcpy(dst_bits, src_bits, tuple_size);
    return result;

}

bool QueryTuple<bool>::operator==(const PlainTuple &other) const {


    if(*query_schema_ != *other.getSchema())
        return false;

    // if both dummies, then ok
    if(this->getDummyTag() && other.getDummyTag()) return true;

    for(size_t i = 0; i < query_schema_->getFieldCount(); ++i) {
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

/*QueryTuple<emp::Bit>
QueryTuple<bool>::secret_share(const PlainTuple *srcTuple, std::shared_ptr<QuerySchema> &dst_schema, int8_t *dst_bits,
                               const int &myParty, const int &dstParty) {

    QueryTuple<emp::Bit> dst_tuple(dst_schema, dst_bits);

    // serialize the whole tuple, then secret share it
    size_t tuple_bits = dst_schema->size();
    size_t tuple_bytes = tuple_bits/8;


    emp::Integer dst_bytes(tuple_bits, 0L, dstParty);

    if(srcTuple != nullptr) {

        std::vector<int8_t> srcData(dst_schema->size() / 8);
        bool *bools = Utilities::bytesToBool(srcData.data(), tuple_bytes);
        ProtocolExecution::prot_exec->feed((block *)dst_bytes.bits.data(), dstParty, bools, tuple_bits);
        delete [] bools;
    }
    else {
        ProtocolExecution::prot_exec->feed((block *)dst_bytes.bits.data(), dstParty, nullptr, tuple_bits);
    }



    memcpy(dst_bits, (int8_t *) dst_bytes.bits.data(), tuple_bits * sizeof(emp::block));
    return dst_tuple;
}
*/

string QueryTuple<bool>::toString(const bool &showDummies) const {
    std::stringstream sstream;
    PlainField f;

    bool printValue = showDummies || !getDummyTag();

    if(printValue) {
        f = getField(0);
        sstream <<   "(" <<  f;

        for (size_t i = 1; i < query_schema_->getFieldCount(); ++i) {
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




QueryTuple<bool>::QueryTuple(const QuerySchema &schema) {
    size_t tuple_byte_cnt = schema.size()/8;
    managed_data_ = std::unique_ptr<int8_t[]>(new int8_t[tuple_byte_cnt]);
    fields_ = managed_data_.get();
    query_schema_ = std::make_shared<QuerySchema>(schema);
}

QueryTuple<bool>::QueryTuple(const QueryTuple<bool> &src) {
    query_schema_ = std::shared_ptr<QuerySchema>(new QuerySchema(*(src.getSchema())));
    if(src.hasManagedStorage()) { // allocate storage for this copy
        size_t tuple_byte_cnt = query_schema_->size()/8;
        managed_data_ = std::unique_ptr<int8_t[]>(new int8_t[tuple_byte_cnt]);
        fields_ = managed_data_.get();
    }

}


bool QueryTuple<bool>::hasManagedStorage() const {
    return managed_data_.get() != nullptr;
}

PlainTuple &QueryTuple<bool>::operator=(const PlainTuple &other) {
    assert(*this->getSchema() == *(other.getSchema()));

    if(&other == this)
        return *this;

    int8_t *dst = getData();
    int8_t *src = other.getData();

    memcpy(dst, src, query_schema_->size()/8);

    return *this;


}

PlainTuple QueryTuple<bool>::reveal() const {
    PlainTuple tuple(*this->getSchema());
    tuple = *this;
    return tuple;
}


PlainField QueryTuple<bool>::operator[](const int32_t &idx) {
    return getField(idx);
}

const PlainField QueryTuple<bool>::operator[](const int32_t &idx) const {
    return getField(idx);
}
