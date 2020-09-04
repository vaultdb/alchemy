#ifndef _DEFS_H
#define _DEFS_H

#include <emp-tool/utils/constants.h>
#include <vector>
#include <cstdint>
#include <boost/variant.hpp>

//#include "operators/support/expression.h"


namespace vaultdb {

    // forward declaration -- included in project.h
    //class Expression;

    enum class SortDirection { INVALID = 0, ASCENDING, DESCENDING};

// ordinal == -1 is for dummy tag, order of columns in vector defines comparison thereof
    typedef std::pair<int32_t, SortDirection> ColumnSort;  // ordinal, direction

    typedef    std::vector<ColumnSort> SortDefinition;






}
#endif // _DEFS_H
