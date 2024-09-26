#ifndef _COMMON_DEFS_H
#define _COMMON_DEFS_H
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <cstdint>
#include <utility>
#include <boost/variant.hpp>
#include <emp-tool/utils/constants.h>
#include <emp-tool/emp-tool.h>

#if  __has_include("emp-sh2pc/emp-sh2pc.h")
#include <emp-sh2pc/emp-sh2pc.h>
namespace emp {
    static const int TP = 10086;
    static const int N = 3;
    using OMPCPackedWire = Bit;
}
#elif __has_include("emp-zk/emp-zk.h")
#include <emp-zk/emp-zk.h>
namespace emp {
    static const int TP = 10086;
    static const int N = 3;
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


struct ForeignKeyConstraint {
    ColumnReference primary_key_;
    ColumnReference foreign_key_;
    ForeignKeyConstraint(ColumnReference primary_key, ColumnReference foreign_key) {
        primary_key_ = primary_key;
        foreign_key_ = foreign_key;
    }

    bool operator==(const ForeignKeyConstraint &other) const {
        return primary_key_ == other.primary_key_ && foreign_key_ == other.foreign_key_;
    }

    string toString() const {
        return primary_key_.first + "." + primary_key_.second + " <-- " + foreign_key_.first + "." + foreign_key_.second;
    }
};

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

//    std::ostream &operator<<(std::ostream &os, const BitPackingDefinition &def) {
//        os << def.toString();
//
//        return os;
//
//    }

typedef std::map<ColumnReference , BitPackingDefinition> BitPackingMetadata;

enum class  StorageModel {
    ROW_STORE,
    COLUMN_STORE,
    PACKED_COLUMN_STORE,
    COMPRESSED_STORE
};

enum class CompressionScheme {
    PLAIN,
    RLE,
    DICTIONARY,
    DELTA,
    BITMAP,
    BIT_PACKED // this may replace our current impl bit packing
};

enum class EmpMode {
    PLAIN,
    SH2PC,
    SH2PC_OUTSOURCED, // just one sending party, ALICE
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
    TABLE_SCAN,
    STORED_TABLE_SCAN,
    PACKED_TABLE_SCAN,
    FILTER,
    PROJECT,
    NESTED_LOOP_JOIN,
    LEFT_KEYED_NESTED_LOOP_JOIN,
    KEYED_NESTED_LOOP_JOIN,
    BLOCK_NESTED_LOOP_JOIN,
    SORT_MERGE_JOIN,
    KEYED_SORT_MERGE_JOIN,
    MERGE_JOIN,
    SORT,
    SHRINKWRAP,
    SCALAR_AGGREGATE,
    SORT_MERGE_AGGREGATE,
    NESTED_LOOP_AGGREGATE,
    UNION
};


// data from same column can be same page - easy to get offset.
typedef struct page_id_ {
    int table_id_;
    int col_id_;
    int page_idx_;

    page_id_(const int & table_id, const int & col_id, const int & page_idx) : table_id_(table_id), col_id_(col_id), page_idx_(page_idx) {}
    page_id_(const page_id_ & other) : table_id_(other.table_id_) , col_id_(other.col_id_), page_idx_(other.page_idx_) {}
    page_id_() {} // for map

    page_id_ & operator=(const page_id_ & other) {
        table_id_ = other.table_id_;
        col_id_ = other.col_id_;
        page_idx_ = other.page_idx_;
        return *this;
    }

    bool operator==(const page_id_ &other) const {
        return table_id_ == other.table_id_ && col_id_ == other.col_id_ && page_idx_ == other.page_idx_;
    }

    bool operator!=(const page_id_ &other) const {
        return table_id_ != other.table_id_ && col_id_ != other.col_id_ && page_idx_ != other.page_idx_;
    }
    bool operator<(const page_id_ &o)  const {
        if(table_id_ < o.table_id_) {
            return true;
        } else if(table_id_ == o.table_id_) {
            if(col_id_ < o.col_id_) {
                return true;
            } else if(col_id_ == o.col_id_) {
                return page_idx_ < o.page_idx_;
            }
        }
        return false;
    }

    string toString() const {
        return "(" + std::to_string(table_id_) + ", " + std::to_string(col_id_) + ", " + std::to_string(page_idx_) + ")";
    }

} PageId;

static std::ostream &operator<<(std::ostream &os, const PageId &pid)   {
    os << pid.toString();
    return os;
}

// for use in the buffer pool
typedef struct position_map_entry_ {

    int slot_id_ = -1; // what slot is it in the unpacked buffer pool?
    bool pinned_ = false; // may need to make this a pin count in the long run
    bool dirty_ = false;

    vector<int> last_k_accesses_; // for LRU-K

    position_map_entry_() {} // for map
    position_map_entry_(const int & slot, const bool & pinned, const bool & dirty) : slot_id_(slot) , pinned_(pinned), dirty_(dirty) {}
    position_map_entry_(const int & slot) : slot_id_(slot) , pinned_(false), dirty_(false) {}
    position_map_entry_(const position_map_entry_ & other) : slot_id_(other.slot_id_) , pinned_(other.pinned_), dirty_(other.dirty_) {}

    position_map_entry_ &operator=(const position_map_entry_ &src) {
        slot_id_ = src.slot_id_;
        pinned_ = src.pinned_;
        dirty_ = src.dirty_;
        return *this;
    }

    // just check slot_id for now for easier searching.  lift PageId comparator if need more logic
    bool operator==(const position_map_entry_ &other) const {
        return slot_id_ == other.slot_id_;
    }

    bool operator!=(const position_map_entry_ &other) const {
        return slot_id_ != other.slot_id_;
    }

    // made up logic to make this sortable
    bool operator<(const position_map_entry_ &o)  const {
        if(this->slot_id_ < o.slot_id_) {
            return true;
        }
        return false;
    }

    string toString() const {
        return "(" + std::to_string(slot_id_) + ", "
               + (dirty_ ? "dirty" : "clean") + ", "
               + (pinned_ ? "pinned" : "unpinned") + ")";
    }

} PositionMapEntry;

static std::ostream &operator<<(std::ostream &os, const PositionMapEntry &entry)   {
    os << entry.toString();
    return os;
}

struct ConnectionInfo {
    std::map<int, std::string> hosts_; // party_id --> IP as string
    int32_t port_ = -1;
    int32_t ctrl_port_ = -1;
};

struct empBitSizesInPhysicalBytes {
    const static int num_computing_party_ = 3;
    const static int32_t block_size_ = 16;

    // 2N blocks + 1 block (1 block for lambda bool)
    const static int32_t auth_ram_size_ = (2 * num_computing_party_ + 1) * block_size_;
    // a block<N> for mac and key respectively, a lambda bool (1 byte), and a masked value bool (1 byte)
    const static int32_t auth_disk_size_ = 2 * num_computing_party_ * block_size_ + 1;
    // auth + evaluator labels (N blocks) + 1 block (1 block for masked value bool)
    //const static int32_t evaluator_ram_size_ = auth_ram_size_ + (emp::N + 1) * sizeof(emp::block);
    // only store auth + masked value (1 byte) (evaluator labels will be regenerated after loading)
    const static int32_t evaluator_disk_size_ = auth_disk_size_ + 1;
    // auth + garbler label (1 block)
    //const static int32_t garbler_ram_size_ = auth_ram_size_ + sizeof(emp::block);
    // only store auth (garbler labels will be regenerated after loading)
    const static int32_t garbler_disk_size_ = auth_disk_size_;
    // This is a fixed size for all parties
    // auth share
    // evaluator labels (garbler has zero_blocks for it)
    // masked value (garbler has zero_block for it)
    // garbler labels (evaluator has zero_block for it)
    const static int32_t bit_ram_size_ = auth_ram_size_ + (num_computing_party_ + 2) * block_size_;
};

} // namespace vaultdb
#endif // _COMMON_DEFS_H
