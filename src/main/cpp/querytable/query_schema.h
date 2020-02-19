//
// Created by madhav on 1/13/20.
//

#ifndef TESTING_QUERY_SCHEMA_H
#define TESTING_QUERY_SCHEMA_H

#include "query_field_desc.h"
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

class QuerySchema {
private:
  std::map<int, std::unique_ptr<QueryFieldDesc>> fields_;

public:
  [[nodiscard]] int GetNumFields() const;

  std::map<std::string, int> nameToIndex;

  void PutField(int index, QueryFieldDesc &fd);

  size_t GetTupleLen();

  size_t GetFieldOffset(int field_num) const;

  explicit QuerySchema(int num_fields);

  QuerySchema(const QuerySchema &s);

  [[nodiscard]] QueryFieldDesc *GetField(int i) const;
};

#endif // TESTING_QUERY_SCHEMA_H
