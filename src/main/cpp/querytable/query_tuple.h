#ifndef _QUERY_TUPLE_H
#define _QUERY_TUPLE_H

#include "query_field.h"
#include "query_schema.h"
#include <map>
#include <memory>
#include <vector>

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

    QueryTuple(size_t fieldCount, bool is_encrypted);

    QueryTuple(size_t aFieldCount);
    QueryTuple(QueryTuple &src);

    void InitDummy();

  void SetIsEncrypted(bool isEncrypted);
  const vaultdb::QueryField *GetField(int ordinal) const;
  vaultdb::QueryField *GetMutableField(int ordinal);
  void PutField(int ordinal, std::unique_ptr<QueryField> f);
  void PutField(int ordinal, const QueryField *f);
  void SetDummyTag(vaultdb::types::Value *v);
  void SetDummyTag(bool flag);
  const vaultdb::types::Value *GetDummyTag();
  vaultdb::types::Value *GetMutableDummyTag();


   // QueryTuple reveal(EmpParty party) const;
    friend std::ostream& operator<<(std::ostream &strm, const QueryTuple &aTuple);

    QueryTuple();

    void serialize(bool *dst, QuerySchema *schema);
};

} // namespace vaultdb
#endif // QUERY_TUPLE_H
