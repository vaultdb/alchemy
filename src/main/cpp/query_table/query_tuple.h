#ifndef _QUERY_TUPLE_H
#define _QUERY_TUPLE_H

#include "query_field.h"
#include "query_schema.h"
#include <map>
#include <memory>
#include <vector>
#include <query_table/types/value.h>
#include <util/type_utilities.h>

using namespace emp;
using namespace std;

namespace vaultdb {
    class QueryTuple {
    private:

        types::Value dummy_tag_;
        vector<QueryField>  fields_;


    public:
        QueryTuple() {};
        QueryTuple(const size_t & fieldCount, const bool & is_encrypted);
        ~QueryTuple() { };

        explicit QueryTuple(const size_t & aFieldCount);
        QueryTuple(const QueryTuple &src);


        inline bool  isEncrypted() const { return dummy_tag_.getType() == types::TypeId::ENCRYPTED_BOOLEAN; }

      const QueryField getField(int ordinal) const;
      QueryField *getFieldPtr(const uint32_t &ordinal) const; // returns a pointer to the original field, mutable
      void putField(const QueryField &f);
      void setDummyTag(const types::Value &v);
      const types::Value getDummyTag() const;


        QueryTuple reveal(const int &empParty = PUBLIC) const;
        friend ostream &operator<<(ostream &os, const QueryTuple &table);

        string toString(const bool &showDummies = false) const;

        void setFieldCount(size_t fieldCount);

        void serialize(int8_t *dst, const QuerySchema &schema);
        size_t getFieldCount() const;

        QueryTuple& operator=(const QueryTuple& other);
        bool operator==(const QueryTuple & other) const;
        inline bool operator!=(const QueryTuple & other) { return !(*this == other);   }

        static void compareAndSwap(QueryTuple  *lhs, QueryTuple *rhs, const emp::Bit & cmp);
        static QueryTuple deserialize(const QuerySchema & schema, int8_t *tupleBits);
        static QueryTuple deserialize(const QuerySchema &schema, Bit *tupleBits);

    };

} // namespace vaultdb
#endif // QUERY_TUPLE_H
