#include <querytable/query_table.h>
#include <memory>

enum class SortOrder { INVALID = 0, ASCENDING, DESCENDING};

struct SortDef {
  SortOrder order;
  std::vector<int> ordinals;
};

void Sort(QueryTable *input, SortDef &s);
