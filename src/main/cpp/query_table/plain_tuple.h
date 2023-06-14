#ifndef _PLAIN_TUPLE_H
#define _PLAIN_TUPLE_H

#include <util/utilities.h>
#include "query_tuple.h"

namespace  vaultdb {
    // plaintext specialization

    typedef QueryTuple<bool> PlainTuple;

    template<>
    class QueryTuple<bool> {


    public:
        int8_t *fields_; // has dummy tag at end, serialized representation, points to an offset in parent QueryTable
        QuerySchema *schema_; // pointer to enclosing table
       int8_t  *managed_data_ = nullptr;


        QueryTuple() : fields_(nullptr) {};
        ~QueryTuple()  { if(managed_data_ != nullptr) delete [] managed_data_; } // don't free fields_, this is done at the table level

        QueryTuple(QuerySchema *query_schema,  int8_t *tuple_payload);
        QueryTuple(QuerySchema *query_schema, const int8_t *src);
        explicit QueryTuple(QuerySchema *schema);
        QueryTuple(const QueryTuple & src);

        int8_t *getData() const { return fields_; }


        inline bool hasManagedStorage() const { return managed_data_ != nullptr; } // returns true if unique_ptr is initialized
        inline bool  isEncrypted() const {
            return false;
        }

        inline PlainField getField(const int &ordinal) {
            QueryFieldDesc field_desc = schema_->getField(ordinal);
            return Field<bool>::deserialize(field_desc, fields_ + (schema_->getFieldOffset(ordinal) / 8));

        }

        inline const PlainField getPackedField(const int &ordinal) const {
            QueryFieldDesc field_desc = schema_->getField(ordinal);
            return Field<bool>::deserializePacked(field_desc, fields_ + (schema_->getFieldOffset(ordinal) / 8));
        }

        inline const PlainField getField(const int &ordinal) const {
            QueryFieldDesc field_desc = schema_->getField(ordinal);
            return Field<bool>::deserialize(field_desc, fields_ + (schema_->getFieldOffset(ordinal) / 8));

        }

        void setField(const int &idx, const PlainField &f);

        inline void setDummyTag(const bool & b) {
            size_t dummy_tag_size = sizeof(bool);
            int8_t *dst = fields_ + schema_->size() / 8 - dummy_tag_size;
            memcpy(dst, &b, dummy_tag_size);

        }

        inline void setDummyTag(const Field<bool> & d) {
            assert(d.getType() == FieldType::BOOL);
            bool dummy_tag = d.getValue<bool>();
            setDummyTag(dummy_tag);

        }

        inline void setSchema(QuerySchema *q) {
            schema_ = q;
        }


        inline bool getDummyTag() const {
            size_t dummy_tag_size = sizeof(bool);
            int8_t *src = fields_ + schema_->size() / 8 - dummy_tag_size;
            return *((bool *) src);

        }

        inline QuerySchema *getSchema() const {     return schema_; }


        string toString(const bool &showDummies = false) const;

        inline void serialize(int8_t *dst) {
            memcpy(dst, fields_, schema_->size() / 8);
        }

        inline size_t getFieldCount() const {
            return schema_->getFieldCount();
        }

        PlainTuple& operator=(const PlainTuple& other);

        bool operator==(const PlainTuple & other) const;

        inline bool operator!=(const PlainTuple& other) const { return !(*this == other);   }
        PlainField operator[](const int32_t & idx);

        const PlainField operator[](const int32_t & idx) const;

        static void compareSwap(const bool &cmp, PlainTuple  & lhs, PlainTuple & rhs);


        static PlainTuple deserialize(int8_t *dst_bits, QuerySchema *schema, int8_t *src_bits);

        // create an independent tuple with reveal, for PUBLIC
        PlainTuple reveal(const int & party = emp::PUBLIC) const;

        static void writeSubset(const PlainTuple & src_tuple, const PlainTuple & dst_tuple, uint32_t src_start_idx, uint32_t src_attr_cnt, uint32_t dst_start_idx);

        static inline PlainTuple  If(const bool & cond,  const PlainTuple & lhs, const PlainTuple & rhs) {
                if(cond)
                    return lhs;
                return rhs;
        }

    };
    std::ostream &operator<<(std::ostream &os, const QueryTuple<bool> &tuple);

}

#endif //_PLAINTUPLE_H
