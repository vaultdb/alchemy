#include <querytable/query_table.h>
#include <memory>

enum class SortOrder { INVALID = 0, ASCENDING, DESCENDING};

struct SortDef {
  SortOrder order;
  SortOrder dummy_order;
  // ordinal == -1 implies the sorting column is the dummy flag
  std::vector<int> ordinals;
};

void Sort(QueryTable *input, SortDef &s);
