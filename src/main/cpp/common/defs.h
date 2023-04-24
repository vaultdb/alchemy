#ifndef _DEFS_H
#define _DEFS_H

#include <emp-tool/utils/constants.h>
#include <vector>
#include <string>
#include <map>
#include <cstdint>
#include <boost/variant.hpp>



namespace vaultdb {


    enum class SortDirection { ASCENDING = 0, DESCENDING = 1, INVALID = 2};

// ordinal == -1 is for dummy tag, order of columns in vector defines comparison thereof
    typedef std::pair<int32_t, SortDirection> ColumnSort;  // ordinal, direction
    typedef std::pair<std::string, std::string> ColumnReference; // pair of <table_name, column_name>


    typedef    std::vector<ColumnSort> SortDefinition;


    // TODO: expand this later for dictionary compression
    class BitPackingDefinition {
    public:
        int min_; // only covering ints for now
        int max_;
        int domain_size_;

        BitPackingDefinition() { min_ = max_ = domain_size_ = 0; }
    };

    typedef std::map<ColumnReference , BitPackingDefinition> BitPackingMetadata;

}
#endif // _DEFS_H
