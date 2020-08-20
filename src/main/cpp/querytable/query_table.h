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
  QuerySchema schema_;
  bool is_encrypted_;

    std::unique_ptr<QueryTuple[]>  tuples_;
    size_t tupleCount_;


public:
    QueryTable(const int & num_tuples, const int & colCount, const bool & is_encrypted);
    QueryTable(const QueryTable & src);
    ~QueryTable() {
        tuples_.release();
    }

    static std::string getQueryTableXorString(const QueryTable  & input_table) {throw; } // not yet implemented

  const bool isEncrypted() const;
  void setSchema(const QuerySchema & schema);
  const QuerySchema & getSchema() const;
  QueryTuple getTuple(int idx) const;
  unsigned int getTupleCount() const;
  std::string toString() const;
  void putTuple(int idx, const QueryTuple & tuple);
  void setTupleDummyTag(const int & tupleIdx, const types::Value & dummyTag);


  // TODO for encryption/decryption
  bool *serialize() const;

    friend std::ostream &operator<<(std::ostream &os, const QueryTable &table);

    std::unique_ptr<QueryTable> reveal(int empParty) const;

    QueryTable & operator=(const QueryTable & src);
};

#endif // _QUERY_TABLE_H
