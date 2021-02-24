#ifndef QUERY_FIELD_H
#define QUERY_FIELD_H

#include "emp-tool/emp-tool.h"
#include "query_field_desc.h"
#include "types/value.h"
#include <memory>
#include <vector>
#include "common/defs.h"

using namespace emp;
namespace vaultdb {

class QueryField {
private:


    // TODO: make the field nullable
    // intercept getValue to produce nulls as needed
    uint32_t ordinal;
  types::Value value_;

public:
  QueryField() {};
  QueryField(const QueryField &qf);
  QueryField(int fn) : ordinal(fn) {};
  QueryField(const uint32_t &fn, const types::Value & aVal)  : ordinal(fn), value_(aVal){};


  types::Value getValue() const;

  QueryField reveal(const int &party) const;
    uint32_t getOrdinal() const { return ordinal; }

    void serialize(int8_t *dst);
    void setValue(const types::Value & val);

    void setOrdinal(const uint32_t & anOrdinal) { ordinal = anOrdinal; }

    QueryField& operator=(const QueryField& other);
    bool operator==(const QueryField &other) const;
    inline bool operator!=(const QueryField &other) const { return !(*this == other); }

    static QueryField deserialize(const QueryFieldDesc desc, int8_t *cursor);
    static QueryField deserialize(const QueryFieldDesc desc, Bit *cursor);

};

    std::ostream& operator<<(std::ostream &strm, const QueryField &aField);

} // namespace vaultdb
#endif // QUERY_FIELD_H
