#ifndef _DEFS_H
#define _DEFS_H

#include <emp-tool/utils/constants.h>
#include <vector>

namespace vaultdb {

    enum class SortDirection { INVALID = 0, ASCENDING, DESCENDING};

    typedef std::pair<int32_t, SortDirection> ColumnSort;  // ordinal, direction
    struct SortDefinition {
        // ordinal == -1 is for dummy tag, order of columns in vector defines comparison thereof

        std::vector<ColumnSort> columnOrders;
    };

}
#endif // _DEFS_H
