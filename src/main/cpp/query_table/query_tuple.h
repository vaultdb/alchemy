#ifndef _QUERY_TUPLE_H
#define _QUERY_TUPLE_H

#include "query_field.h"
#include "query_schema.h"
#include <map>
#include <memory>
#include <vector>
#include <query_table/types/value.h>
#include <util/type_utilities.h>

namespace vaultdb {
    class QueryTuple {
    private:

        vaultdb::types::Value dummy_tag_;
        std::vector<QueryField>  fields_;


    public:
        QueryTuple() {};
        QueryTuple(const size_t & fieldCount, const bool & is_encrypted);
        ~QueryTuple() { };

        explicit QueryTuple(const size_t & aFieldCount);
        QueryTuple(const QueryTuple &src);


        inline bool  isEncrypted() const { return dummy_tag_.getType() == types::TypeId::ENCRYPTED_BOOLEAN; }

      const vaultdb::QueryField getField(int ordinal) const;
      vaultdb::QueryField *getFieldPtr(const uint32_t &ordinal) const; // returns a pointer to the original field, mutable
      void putField(const QueryField &f);
      void setDummyTag(const types::Value &v);
      const vaultdb::types::Value getDummyTag() const;


        QueryTuple reveal(const int &empParty = emp::PUBLIC) const;
        friend std::ostream& operator<<(std::ostream &strm, const QueryTuple &aTuple);
        std::string toString(const bool &showDummies = false) const;

        void setFieldCount(size_t fieldCount);

        void serialize(bool *dst, const QuerySchema &schema);
        size_t getFieldCount() const;

        QueryTuple& operator=(const QueryTuple& other);
        bool operator==(const QueryTuple & other);
        inline bool operator!=(const QueryTuple & other) { return !(*this == other);   }

        static void compareAndSwap(QueryTuple  *lhs, QueryTuple *rhs, const emp::Bit & cmp);

    };

} // namespace vaultdb
#endif // QUERY_TUPLE_H
