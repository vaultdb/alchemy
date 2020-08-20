//
// Created by madhav on 1/29/20.
//

#ifndef _DEFS_H
#define _DEFS_H

#include <emp-tool/utils/constants.h>
#include <vector>

namespace vaultdb {

    enum class SortOrder { INVALID = 0, ASCENDING, DESCENDING};

    struct SortDef {
        SortOrder order;
        SortOrder dummy_order;
        // ordinal == -1 implies the sorting column is the dummy flag
        std::vector<int> ordinals;
    };

}
#endif // _DEFS_H
