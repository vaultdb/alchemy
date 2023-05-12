#ifndef _ORAM_PRIMITIVES_
#define _ORAM_PRIMITIVES_

#include <vector>
#include "query_table/query_tuple.h"

// shared primitives for ORAM operations
// over-simplified for now
namespace vaultdb {

    
    // each tuple is a block
    typedef  std::vector<int8_t> OramBlock;

    // a bucket contains up to Z real blocks
    typedef std::vector<OramBlock> OramBucket;

    template <typename B>
    OramBlock createBlock(QueryTuple<B> & t) {
        int8_t *data = t.getData();

        int bit_size = t.query_schema_->size();


    }

}

template class vaultdb::O<bool>;
template class vaultdb::QueryTable<emp::Bit>;

#endif