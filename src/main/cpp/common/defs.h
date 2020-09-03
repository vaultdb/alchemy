#ifndef _DEFS_H
#define _DEFS_H

#include <emp-tool/utils/constants.h>
#include <vector>
#include <cstdint>

namespace vaultdb {

    enum class SortDirection { INVALID = 0, ASCENDING, DESCENDING};

    typedef std::pair<int32_t, SortDirection> ColumnSort;  // ordinal, direction
    // / ordinal == -1 is for dummy tag, order of columns in vector defines comparison thereof

    typedef    std::vector<ColumnSort> SortDefinition;


}
#endif // _DEFS_H
