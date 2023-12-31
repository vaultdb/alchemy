#ifndef _COMMON_DEFS_H
#define _COMMON_DEFS_H

#include <emp-tool/utils/constants.h>
#include <vector>
#include <string>
#include <map>
#include <cstdint>
#include <boost/variant.hpp>
#include <sstream>
#include <emp-tool/emp-tool.h>


#if  __has_include("emp-sh2pc/emp-sh2pc.h")
#include <emp-sh2pc/emp-sh2pc.h>
namespace emp {
    const static int TP = 10086;
    const static int N = 3;
    using OMPCPackedWire = Bit;
}
#elif __has_include("emp-zk/emp-zk.h")
#include <emp-zk/emp-zk.h>
namespace emp {
    const static int TP = 10086;
    const static int N = 3;
    using OMPCPackedWire = Bit;

}
#elif __has_include("emp-rescu/emp-rescu.h")
#include "emp-rescu/emp-rescu.h"

namespace  emp {
    using Integer = Integer_T<OMPCBackend<N>::wire_t>;
    using Float = Float_T<OMPCBackend<N>::wire_t>;
    using Bit = Bit_T<OMPCBackend<N>::wire_t>;
}
#endif


namespace vaultdb {

    typedef std::pair<std::vector<int8_t>, std::vector<int8_t> > SecretShares;

    enum class SortDirection { ASCENDING = 0, DESCENDING = 1, INVALID = 2};

// ordinal == -1 is for dummy tag, order of columns in vector defines comparison thereof
    typedef std::pair<int32_t, SortDirection> ColumnSort;  // ordinal, direction
    typedef std::pair<std::string, std::string> ColumnReference; // pair of <table_name, column_name>


    typedef    std::vector<ColumnSort> SortDefinition;


    class BitPackingDefinition {
    public:
        int min_; // only covering ints for now
        int max_;
        int domain_size_;

        BitPackingDefinition() { min_ = max_ = domain_size_ = 0; }
        std::string toString() const {
            std::stringstream s;
            s << "[" << min_ << ", " << max_ << "] domain size: " << domain_size_;
            return s.str();
        }
    };


    typedef std::map<ColumnReference , BitPackingDefinition> BitPackingMetadata;

    enum class  StorageModel {
        ROW_STORE,
        COLUMN_STORE
    };


    enum class EmpMode {
        PLAIN,
        SH2PC,
        ZK, // ZK only enabled manually during unit test
        OUTSOURCED
    };

    enum class OperatorType {
        SQL_INPUT,
        ZK_SQL_INPUT,
        SECURE_SQL_INPUT,
        TABLE_INPUT,
        CSV_INPUT,
        MERGE_INPUT,
        FILTER,
        PROJECT,
        NESTED_LOOP_JOIN,
        KEYED_NESTED_LOOP_JOIN,
        KEYED_SORT_MERGE_JOIN,
        MERGE_JOIN,
        SORT,
        SHRINKWRAP,
        SCALAR_AGGREGATE,
        SORT_MERGE_AGGREGATE,
        NESTED_LOOP_AGGREGATE,
        UNION
    };

}
#endif // _COMMON_DEFS_H
