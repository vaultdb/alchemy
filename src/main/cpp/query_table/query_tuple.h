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
    class QueryTuple {
    private:

        Field dummy_tag_;
        vector<Field>  fields_;


    public:
        QueryTuple() {};
        QueryTuple(const size_t & fieldCount, const bool & is_encrypted);
        ~QueryTuple() { };

        explicit QueryTuple(const size_t & aFieldCount);
        QueryTuple(const QueryTuple &src);


        inline bool  isEncrypted() const;

      const Field getField(const int &ordinal) const;
      void putField(const int &idx, const Field &f);
      void setDummyTag(const Field &v);

        const Field getDummyTag() const;


        QueryTuple reveal(const int &empParty = PUBLIC) const;

        string toString(const bool &showDummies = false) const;

        void setFieldCount(size_t fieldCount);

        void serialize(int8_t *dst, const QuerySchema &schema);
        size_t getFieldCount() const;

        QueryTuple& operator=(const QueryTuple& other);
        bool operator==(const QueryTuple & other) const;
        inline bool operator!=(const QueryTuple & other) { return !(*this == other);   }

        static void compareAndSwap(QueryTuple  *lhs, QueryTuple *rhs, const emp::Bit & cmp);
        static QueryTuple deserialize(const QuerySchema & schema, int8_t *tupleBits);
        //static QueryTuple deserialize(const QuerySchema &schema, Bit *tupleBits);

    };
    std::ostream &operator<<(std::ostream &os, const QueryTuple &tuple);



} // namespace vaultdb
#endif // QUERY_TUPLE_H
