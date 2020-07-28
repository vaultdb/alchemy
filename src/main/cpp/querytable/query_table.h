#ifndef _QUERY_TABLE_H
#define _QUERY_TABLE_H

#include "emp-tool/emp-tool.h"
#include "query_schema.h"
#include "query_tuple.h"
#include <memory>
#include <vaultdb.h>
#include <ostream>

using namespace vaultdb;


class QueryTable {
private:
  std::unique_ptr<QuerySchema> schema_;
  const bool is_encrypted_;

    std::unique_ptr<QueryTuple[]>  tuples_;
    size_t tupleCount_;


public:
  static std::unique_ptr<QueryTable> GetQueryFromProtoStream(const void *buf,
                                                             int len);
  static std::string GetQueryTableXorString(QueryTable *input_table);
  const bool GetIsEncrypted() const;
  QueryTable(int num_tuples);
  QueryTable(int num_tuples, bool is_encrypted);
  void SetSchema(std::unique_ptr<QuerySchema> s);
  void SetSchema(const QuerySchema *s);
  const QuerySchema *GetSchema() const;
  QueryTuple *GetTuple(int idx) const;
  unsigned int getTupleCount() const;
  std::unique_ptr<QuerySchema> ReleaseSchema();
  std::string toString() const;


    // TODO: validate this - work in progress, use with caution!
  //std::unique_ptr<QueryTable> reveal(EmpParty party) const;

  // TODO for encryption/decryption
  bool *serialize() const;

    friend std::ostream &operator<<(std::ostream &os, const QueryTable &table);

    std::unique_ptr<QueryTable> reveal(EmpParty party) const;
};

#endif // _QUERY_TABLE_H
