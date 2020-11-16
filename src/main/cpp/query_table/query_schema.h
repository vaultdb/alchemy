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
    explicit QuerySchema(const size_t &num_fields);
    QuerySchema(const QuerySchema &s);

    int getFieldCount() const;

  void putField(const QueryFieldDesc &fd);

  // returns size in bits
   size_t size() const;


    const QueryFieldDesc getField(const int &i) const;

    friend std::ostream &operator<<(std::ostream &os, const QuerySchema &schema);
    QuerySchema& operator=(const QuerySchema& other);


     bool operator==(const QuerySchema &other) const;

     inline bool operator!=(const QuerySchema &other) const { return !(*this == other);  }

     static QuerySchema toSecure(const QuerySchema & plainSchema);
    static QuerySchema toPlain(const QuerySchema & secureSchema);

};

#endif // _QUERY_SCHEMA_H
