#ifndef _QUERY_TUPLE_H
#define _QUERY_TUPLE_H

#include "query_schema.h"
#include <map>
#include <memory>
#include <vector>
#include <util/type_utilities.h>
#include "field/bool_field.h"
#include "field/secure_bool_field.h"

using namespace emp;

namespace vaultdb {

    template<typename B> class QueryTuple;

    typedef vaultdb::QueryTuple<BoolField> PlainTuple;
    typedef vaultdb::QueryTuple<SecureBoolField> SecureTuple;


    template<typename B>
    class QueryTuple {
    private:

        B dummy_tag_;
        vector<Field<B> >   fields_;


    public:
        QueryTuple() {};
        ~QueryTuple() = default;

        explicit QueryTuple(const size_t & aFieldCount);
        QueryTuple(const QueryTuple &src);


        inline bool  isEncrypted() const;


        const Field<B> * getField(const int &ordinal) const;
      void putField(const int &idx, const Field<B> &f);
      void setDummyTag(const B &v);

      void setDummyTag(const bool & b);



      const B *getDummyTag() const;


        PlainTuple reveal(const int &empParty = PUBLIC) const;

        string toString(const bool &showDummies = false) const;

        void serialize(int8_t *dst, const QuerySchema &schema);
        size_t getFieldCount() const;

        QueryTuple& operator=(const QueryTuple& other);
        bool operator==(const QueryTuple<B> & other) const;
        inline bool operator!=(const QueryTuple<B> & other) const { return !(*this == other);   }
        inline Field<B> operator[](const int32_t & idx) const { return fields_[idx]; }


        static void compareAndSwap(const B &cmp, QueryTuple <B> *lhs, QueryTuple <B> *rhs);
       // static void compareAndSwap(QueryTuple  *lhs, QueryTuple *rhs, const BoolField & cmp);


        static QueryTuple deserialize(const QuerySchema & schema, int8_t *tupleBits);


        static SecureTuple
        secretShare(const PlainTuple *srcTuple, const QuerySchema &schema, const int &myParty, const int &dstParty);

        // already encrypted
        static SecureTuple
        secretShare(const SecureTuple *srcTuple, const QuerySchema &schema, const int &myParty, const int &dstParty) {
            return SecureTuple(*srcTuple);
        }

    };

    std::ostream &operator<<(std::ostream &os, const QueryTuple<BoolField> &tuple);
    std::ostream &operator<<(std::ostream &os, const QueryTuple<SecureBoolField> &tuple);



} // namespace vaultdb
#endif // QUERY_TUPLE_H
