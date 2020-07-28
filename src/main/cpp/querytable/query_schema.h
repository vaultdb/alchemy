#ifndef _QUERY_SCHEMA_H
#define _QUERY_SCHEMA_H

#include "query_field_desc.h"
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>
#include <ostream>

class QuerySchema {
private:

    // TODO: safely delete fieldStorage in a destructor
    std::unique_ptr<QueryFieldDesc[]>  fields_;
    size_t fieldCount_;

public:
  [[nodiscard]] int getFieldCount() const;

  std::map<std::string, int> nameToIndex;

  void PutField(int index, QueryFieldDesc &fd);

  // returns size in bits
  [[nodiscard]] size_t size() const;

  explicit QuerySchema(int num_fields);

  QuerySchema(const QuerySchema &s);

  [[nodiscard]] QueryFieldDesc *GetField(int i) const;

    friend std::ostream &operator<<(std::ostream &os, const QuerySchema &schema);

};

#endif // TESTING_QUERY_SCHEMA_H
