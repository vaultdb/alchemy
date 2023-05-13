#ifndef _ORAM_BLOCK_
#define  _ORAM_BLOCK_

#include <vector>
#include "query_table/plain_tuple.h"
#include "query_table/secure_tuple.h"
#include "query_table/field/field_factory.h"

// shared primitives for ORAM operations
// over-simplified for now
// based on ObliVMGC: https://github.com/oblivm/ObliVMGC/blob/master/java/com/oblivm/backend/oram/Block.java

namespace vaultdb {

    // simple wrapper for tuple and supporting data
    template<typename B>
    class OramBlock {
    public:
        QueryTuple<B> tuple_; // get dummy tag from this, may need its own member variable going forward
        Field<B> pos_;
        Field<B> id_;

        OramBlock(const std::shared_ptr<QuerySchema> & schema) {
            tuple_ = QueryTuple<B>(schema);
            FieldType f = (std::is_same_v<bool, B>) ? FieldType::INT : FieldType::SECURE_INT;
            id_ = pos_ = FieldFactory<B>::getZero(f);
        }

        inline B isDummy() {
            return tuple_.getDummyTag();
        }

        void setDummyTag(const B & dummy_tag) {
            tuple_.setDummyTag(dummy_tag);
        }

        static OramBlock If(B cond, const OramBlock <B> &lhs, const OramBlock <B> &rhs) {
            assert(lhs.tuple_.getSchema() == rhs.tuple_.getSchema());

            OramBlock<B> res(lhs.tuple_.getSchema());

            res.tuple_ =  QueryTuple<B>::If(cond, lhs.tuple_, rhs.tuple_);
            res.id_ =  Field<B>::If(cond, lhs.id_, rhs.id_);
            res.pos_ = Field<B>::If(cond, lhs.pos_, rhs.pos_);

            return res;
        }

        static OramBlock<B> Xor(const OramBlock<B> & lhs, const OramBlock<B> & rhs) {
            assert(lhs.tuple_.getSchema() == rhs.tuple_.getSchema());
            std::shared_ptr<QuerySchema> schema = lhs.tuple_.getSchema();
            bool is_secure = schema->isSecure();

            OramBlock<B> res(schema);

            int schema_bytes = (is_secure) ? schema->size() * TypeUtilities::getEmpBitSize() : schema->size() / 8;


            int8_t *dst = (int8_t *) res.tuple_.getData();
            int8_t *lhs_bytes = (int8_t *)  lhs.tuple_.getData();
            int8_t *rhs_bytes = (int8_t *)  rhs.tuple_.getData();


            for(int i = 0; i < schema_bytes; ++i) {
                *dst = *lhs_bytes ^ *rhs_bytes;
                ++dst;
                ++lhs_bytes;
                ++rhs_bytes;
            }

            res.id_ = lhs.id_ ^ rhs.id_;
            res.pos_ = lhs.pos_ ^ rhs.pos_;

            return res;
        }


    };

}

template class vaultdb::OramBlock<bool>;
template class vaultdb::OramBlock<emp::Bit>;


#endif