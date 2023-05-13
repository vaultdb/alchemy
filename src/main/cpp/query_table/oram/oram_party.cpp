#include "oram_party.h"

using namespace vaultdb;

template<typename B>
OramParty<B>::OramParty(int tree_depth, int tuples_per_bucket, const shared_ptr<QuerySchema> &schema)
: tuples_per_bucket_(tuples_per_bucket), tree_depth_(tree_depth) {
  //  root_ = new OramBucket<B>(schema, blocks_per_bucket, 0, tree_depth);

}

template<typename B>
OramBucket<B> *OramParty<B>::readBucket(const int & path, const int & depth) {
    return nullptr;
}


template<typename B>
vector<bool> OramParty<B>::intToBits(const int &src) {
    vector<bool> bits(tree_depth_);
    int8_t *bytes = (int8_t *) &src;

    int path_bytes = tree_depth_ / 8 + (tree_depth_ % 8 > 0);
    int cursor = 0;
    for(int i = 0; i < path_bytes; ++i) {
        uint8_t b = bytes[i];
        for(int j = 0; j < 8; ++j) {
            bits[cursor] = ((b & (1<<j)) != 0);
            ++cursor;
        }
    }

    return bits;

}
