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
  bool is_encrypted_;

    std::unique_ptr<QueryTuple[]>  tuples_;
    size_t tupleCount_;


public:
    QueryTable(int num_tuples, int colCount, bool is_encrypted);
    QueryTable(const QueryTable & src);
    ~QueryTable() {
        std::cout << "Freeing a query table at " << this << std::endl;
    }

    static std::unique_ptr<QueryTable> GetQueryFromProtoStream(const void *buf,int len);
  static std::string GetQueryTableXorString(QueryTable *input_table);
  const bool isEncrypted() const;
  void setSchema(std::unique_ptr<QuerySchema> s);
  void setSchema(const QuerySchema *s);
  const QuerySchema *getSchema() const;
  QueryTuple *getTuple(int idx) const;
  unsigned int getTupleCount() const;
  std::unique_ptr<QuerySchema> ReleaseSchema();
  std::string toString() const;
  void putTuple(int idx, QueryTuple tuple);


  // TODO for encryption/decryption
  bool *serialize() const;

    friend std::ostream &operator<<(std::ostream &os, const QueryTable &table);

    std::unique_ptr<QueryTable> reveal(EmpParty party) const;

    QueryTable & operator=(const QueryTable & src);
};

#endif // _QUERY_TABLE_H
