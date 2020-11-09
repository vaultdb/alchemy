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

    std::vector<QueryTuple> tuples_;
    size_t tupleCount_;


public:
    QueryTable(const int & num_tuples, const int & colCount, const bool & is_encrypted);
    QueryTable(const QueryTable & src);
    ~QueryTable() {
    }

    static std::string getQueryTableXorString(const QueryTable  & input_table) {throw; } // not yet implemented

  const bool isEncrypted() const;
  void setSchema(const QuerySchema & schema);
  const QuerySchema & getSchema() const;
  QueryTuple getTuple(int idx) const;
  unsigned int getTupleCount() const;
  std::string toString(const bool & showDummies = false) const;
  void putTuple(const int &idx, const QueryTuple & tuple);
  void setTupleDummyTag(const int & tupleIdx, const types::Value & dummyTag);

    QueryTuple* getTuplePtr(const int & idx)  const;


    // retrieves # of tuples that are not dummies
    // only works for unencrypted tables, o.w. returns getTupleCount()
    uint32_t getTrueTupleCount() const;

  bool *serialize() const;

  friend std::ostream &operator<<(std::ostream &os, const QueryTable &table);

    std::shared_ptr<QueryTable> secretShare(emp::NetIO *io, const int &  party) const; // shared_ptr so we can pass it among Operator instances
    [[nodiscard]] std::unique_ptr<QueryTable> reveal(int empParty = emp::PUBLIC) const;

    QueryTable & operator=(const QueryTable & src);

    bool operator==(const QueryTable & other) const;
    bool operator!=(const QueryTable & other) const { return !(*this == other); }
};

#endif // _QUERY_TABLE_H
