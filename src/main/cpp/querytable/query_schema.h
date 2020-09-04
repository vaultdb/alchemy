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

    std::vector<QueryFieldDesc> fields_;
    size_t fieldCount_;

public:
    explicit QuerySchema(const int &num_fields);
    QuerySchema(const QuerySchema &s);

    [[nodiscard]] int getFieldCount() const;

  void putField(const int &index, const QueryFieldDesc &fd);

  // returns size in bits
   size_t size() const;


    const QueryFieldDesc getField(const int &i) const;

    friend std::ostream &operator<<(std::ostream &os, const QuerySchema &schema);
    QuerySchema& operator=(const QuerySchema& other);


     bool operator==(const QuerySchema &other) const;

     inline bool operator!=(const QuerySchema &other) const { return !(*this == other);  }


};

#endif // _QUERY_SCHEMA_H
