#ifndef _PLAIN_TUPLE_H
#define _PLAIN_TUPLE_H

#include <util/utilities.h>
#include "query_tuple.h"

namespace  vaultdb {
    // plaintext specialization

    typedef QueryTuple<bool> PlainTuple;

    template<>
    class QueryTuple<bool> {


    protected:
        int8_t *fields_; // has dummy tag at end, serialized representation, points to an offset in parent QueryTable
        std::shared_ptr<QuerySchema> query_schema_; // pointer to enclosing table
        std::unique_ptr<int8_t []> managed_data_;


    public:
        QueryTuple() : fields_(nullptr) {};
        ~QueryTuple() = default; // don't free fields_, this is done at the table level

        QueryTuple(std::shared_ptr<QuerySchema> & query_schema,  int8_t *tuple_payload);
        QueryTuple(const std::shared_ptr<QuerySchema> & query_schema, const int8_t *src);
        explicit QueryTuple(const  QuerySchema & schema);
        QueryTuple(const QueryTuple & src);

        int8_t *getData() const { return fields_; }

        // need a target table
        // QueryTuple(const QueryTuple &src) {

        //  }


        bool hasManagedStorage() const; // returns true if unique_ptr is initialized
        inline bool  isEncrypted() const {
            return false;
        }

        PlainField getField(const int &ordinal);
        const PlainField getField(const int &ordinal) const;

        void setField(const int &idx, const PlainField &f);

        void setDummyTag(const bool & b);

        void setDummyTag(const Field<bool> & d);

        void setSchema(std::shared_ptr<QuerySchema> q);


        bool getDummyTag() const;

        std::shared_ptr<QuerySchema> getSchema() const;


        string toString(const bool &showDummies = false) const;

        void serialize(int8_t *dst) {

        }

        size_t getFieldCount() const {
            return 0;
        }

        PlainTuple& operator=(const PlainTuple& other);

        bool operator==(const PlainTuple & other) const;

        inline bool operator!=(const PlainTuple& other) const { return !(*this == other);   }
        PlainField operator[](const int32_t & idx);

        const PlainField operator[](const int32_t & idx) const;

        static void compare_swap(const bool &cmp, PlainTuple  & lhs, PlainTuple & rhs);


        static PlainTuple deserialize(int8_t *dst_bits, std::shared_ptr<QuerySchema> & schema, int8_t *src_bits);

        // create an independent tuple with reveal
        PlainTuple reveal() const;



    };
    std::ostream &operator<<(std::ostream &os, const QueryTuple<bool> &tuple);

}

#endif //_PLAINTUPLE_H
