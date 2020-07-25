#ifndef QUERY_FIELD_H
#define QUERY_FIELD_H

#include "emp-tool/emp-tool.h"
#include "query_field_desc.h"
#include "types/value.h"
#include <memory>
#include <vector>
#include <defs.h>

namespace vaultdb {

class QueryField {
private:


    int ordinal;
  types::Value value_;

public:
  QueryField();
  QueryField(const QueryField &qf);

  QueryField(int field_num, int64_t val);

  QueryField(int field_num, int32_t val);

  QueryField(int field_num, emp::Integer, int length);

  QueryField(int field_num, double val);

  QueryField(int field_num);

  QueryField(int field_num, std::string val);

  QueryField(int field_num, float val);


  [[nodiscard]]  types::Value *GetValue() const;
  [[nodiscard]] types::Value *GetMutableValue();
  void SetValue(const types::Value *val);

  friend std::ostream& operator<<(std::ostream &strm, const QueryField &aField);

  QueryField reveal(EmpParty party) const;

};

} // namespace vaultdb
#endif // QUERY_FIELD_H
