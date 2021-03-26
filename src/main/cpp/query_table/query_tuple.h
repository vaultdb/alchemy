#ifndef _QUERY_TUPLE_H
#define _QUERY_TUPLE_H

#include "query_schema.h"
#include <map>
#include <memory>
#include <vector>
#include <util/type_utilities.h>

using namespace emp;

namespace vaultdb {

    template<typename B> class QueryTuple;

    typedef vaultdb::QueryTuple<bool> PlainTuple;
    typedef vaultdb::QueryTuple<emp::Bit> SecureTuple;


    template<typename B>
    class QueryTuple {

    protected:
        vector<Field<B> >   fields_;
        B dummy_tag_;

    public:
        QueryTuple() {};
        ~QueryTuple() = default;

        explicit QueryTuple(const size_t & aFieldCount);
        QueryTuple(const QueryTuple &src);


        inline bool  isEncrypted() const;


      const Field<B> * getField(const int &ordinal) const;
      void setField(const int &idx, const Field<B> &f);
      void setDummyTag(const bool & b);
      void setDummyTag(const Field<B> & d);


      const B getDummyTag() const;


        PlainTuple reveal(const int &empParty = PUBLIC) const;

        string toString(const bool &showDummies = false) const;

        void serialize(int8_t *dst, const QuerySchema &schema);
        size_t getFieldCount() const;

        QueryTuple& operator=(const QueryTuple& other);
        bool operator==(const QueryTuple<B> & other) const;
        inline bool operator!=(const QueryTuple<B> & other) const { return !(*this == other);   }
        inline Field<B> operator[](const int32_t & idx) const { return fields_[idx]; }


        static void compareAndSwap(const B &cmp, QueryTuple <B> *lhs, QueryTuple <B> *rhs);


        static PlainTuple deserialize(const QuerySchema & schema, int8_t *tupleBits);
        static SecureTuple deserialize(const QuerySchema & schema, emp::Bit *tupleBits);


        static SecureTuple
        secretShare(const PlainTuple *srcTuple, const QuerySchema &schema, const int &myParty, const int &dstParty);

        // already encrypted
        static SecureTuple
        secretShare(const SecureTuple *srcTuple, const QuerySchema &schema, const int &myParty, const int &dstParty) {
            return SecureTuple(*srcTuple);
        }


    private:
        std::string specializedToString(const PlainTuple & tuple, const bool & showDummies) const;
        std::string specializedToString(const SecureTuple & tuple, const bool & showDummies) const;
        static bool tuplesEqual(const PlainTuple &t1, const PlainTuple & t2);
        static bool tuplesEqual(const SecureTuple &t1, const SecureTuple & t2);

    };

    std::ostream &operator<<(std::ostream &os, const PlainTuple &tuple);
    std::ostream &operator<<(std::ostream &os, const SecureTuple &tuple);



} // namespace vaultdb
#endif // QUERY_TUPLE_H
