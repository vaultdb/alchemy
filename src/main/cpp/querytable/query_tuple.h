#ifndef _QUERY_TUPLE_H
#define _QUERY_TUPLE_H

#include "query_field.h"
#include "query_schema.h"
#include <map>
#include <memory>
#include <vector>
#include <querytable/types/value.h>

namespace vaultdb {
class QueryTuple {
private:

    vaultdb::types::Value dummy_tag_;
    bool is_encrypted_{};
    std::unique_ptr<QueryField[]>  fields_;
    size_t fieldCount_;
public:
    void setFieldCount(size_t fieldCount);


public:

    QueryTuple(const size_t & fieldCount, const bool & is_encrypted);

    explicit QueryTuple(const size_t & aFieldCount);
    QueryTuple(const QueryTuple &src);

    void initDummy();

  void setIsEncrypted(bool isEncrypted);
  const vaultdb::QueryField getField(int ordinal) const;
  vaultdb::QueryField *getFieldPtr(const uint32_t &ordinal) const; // returns a pointer to the original field, mutable
  void putField(const QueryField &f);
  void setDummyTag(const types::Value &v);
  const vaultdb::types::Value getDummyTag();


    QueryTuple reveal(const int &empParty) const;
    friend std::ostream& operator<<(std::ostream &strm, const QueryTuple &aTuple);
    std::string toString(const bool &showDummies = false) const;

    QueryTuple();

    void serialize(bool *dst, const QuerySchema &schema);
    size_t getFieldCount() const;

    QueryTuple& operator=(const QueryTuple& other);
    bool operator==(const QueryTuple & other);
    inline bool operator!=(const QueryTuple & other) { return !(*this == other);   }

    static void compareAndSwap(QueryTuple & lhs, QueryTuple & rhs, const emp::Bit & cmp);
};

} // namespace vaultdb
#endif // QUERY_TUPLE_H
