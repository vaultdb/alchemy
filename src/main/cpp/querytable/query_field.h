#ifndef QUERY_FIELD_H
#define QUERY_FIELD_H

#include "emp-tool/emp-tool.h"
#include "query_field_desc.h"
#include "types/value.h"
#include <memory>
#include <vector>
#include "common/defs.h"

namespace vaultdb {

class QueryField {
private:


    int ordinal;
  types::Value value_;

public:
  QueryField() {};

  QueryField(const QueryField &qf);
  QueryField(int fn) : ordinal(fn) {};
  QueryField(const int &fn, const types::Value & aVal)  : ordinal(fn), value_(aVal){};


  types::Value getValue() const;

  QueryField reveal(EmpParty party) const;
    int getOrdinal() const { return ordinal; }
    void serialize(bool *dst);
    void setValue(const types::Value & val);

    QueryField& operator=(const QueryField& other);
    friend std::ostream& operator<<(std::ostream &strm, const QueryField &aField);

};

} // namespace vaultdb
#endif // QUERY_FIELD_H
