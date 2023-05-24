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
        std::shared_ptr<QuerySchema> query_schema_; // pointer to enclosing table
       int8_t  *managed_data_ = nullptr;


        QueryTuple() : fields_(nullptr) {};
        ~QueryTuple()  { if(managed_data_ != nullptr) delete [] managed_data_; } // don't free fields_, this is done at the table level

        QueryTuple(std::shared_ptr<QuerySchema> & query_schema,  int8_t *tuple_payload);
        QueryTuple(const std::shared_ptr<QuerySchema> & query_schema, const int8_t *src);
        explicit QueryTuple(const std::shared_ptr<QuerySchema> & schema);
        QueryTuple(const QueryTuple & src);

        int8_t *getData() const { return fields_; }


        inline bool hasManagedStorage() const { return managed_data_ != nullptr; } // returns true if unique_ptr is initialized
        inline bool  isEncrypted() const {
            return false;
        }

        PlainField getField(const int &ordinal);
        const PlainField getField(const int &ordinal) const;

        void setField(const int &idx, const PlainField &f);

        inline void setDummyTag(const bool & b) {
            size_t dummy_tag_size = sizeof(bool);
            int8_t *dst = fields_ + query_schema_->size()/8 - dummy_tag_size;
            memcpy(dst, &b, dummy_tag_size);

        }

        inline void setDummyTag(const Field<bool> & d) {
            assert(d.getType() == FieldType::BOOL);
            bool dummy_tag = d.getValue<bool>();
            setDummyTag(dummy_tag);

        }

        inline void setSchema(std::shared_ptr<QuerySchema> q) {
            query_schema_ = q;
        }


        bool getDummyTag() const {
            size_t dummy_tag_size = sizeof(bool);
            int8_t *src = fields_ + query_schema_->size()/8 - dummy_tag_size;
            return *((bool *) src);

        }

        inline std::shared_ptr<QuerySchema> getSchema() const {     return query_schema_; }


        string toString(const bool &showDummies = false) const;

        void serialize(int8_t *dst) {

        }

        inline size_t getFieldCount() const {
            return query_schema_->getFieldCount(); // was 0????
        }

        PlainTuple& operator=(const PlainTuple& other);

        bool operator==(const PlainTuple & other) const;

        inline bool operator!=(const PlainTuple& other) const { return !(*this == other);   }
        PlainField operator[](const int32_t & idx);

        const PlainField operator[](const int32_t & idx) const;

        static void compareSwap(const bool &cmp, PlainTuple  & lhs, PlainTuple & rhs);


        static PlainTuple deserialize(int8_t *dst_bits, std::shared_ptr<QuerySchema> & schema, int8_t *src_bits);

        // create an independent tuple with reveal, for PUBLIC
        PlainTuple reveal(const int & party = emp::PUBLIC) const;

        static void writeSubset(const PlainTuple & src_tuple, const PlainTuple & dst_tuple, uint32_t src_start_idx, uint32_t src_attr_cnt, uint32_t dst_start_idx);

        static PlainTuple  If(const bool & cond,  const PlainTuple & lhs, const PlainTuple & rhs) {
                if(cond)
                    return lhs;
                return rhs;
        }

    };
    std::ostream &operator<<(std::ostream &os, const QueryTuple<bool> &tuple);

}

#endif //_PLAINTUPLE_H
