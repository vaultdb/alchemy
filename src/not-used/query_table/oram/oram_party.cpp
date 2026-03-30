#include "oram_party.h"

using namespace vaultdb;

template<typename B>
OramParty<B>::OramParty(int bucket_count, int tuples_per_bucket, const shared_ptr<QuerySchema> &schema)
: tuples_per_bucket_(tuples_per_bucket), dummy_bucket_(schema, tuples_per_bucket_) {


    setup();
    tree_ = vector<OramBucket<B> >(bucket_cnt_, dummy_bucket_);

}


template<typename B>
void OramParty<B>::setup() {
    int a = 1;
    tree_depth_ = 1;

    while(a < bucket_cnt_) {
        a *= 2;
        ++tree_depth_;
    }

    --tree_depth_;
    bucket_cnt_ = 1 << tree_depth_;

}

template<typename B>
vector<OramBucket<B>> OramParty<B>::getPath(const int &path) {
    vector<bool> path_bits = intToBits(path);
    return getPath(path_bits);
}


template<typename B>
vector<OramBucket<B>> OramParty<B>::getPath(const vector<bool> &path) {
        vector<OramBucket<B>> result(tree_depth_, dummy_bucket_);
        int idx = 1;

        result[0] = tree_[idx];
        for(int i = 1; i < tree_depth_; ++i ) {
            idx *= 2;
            if(path[tree_depth_ - i])
                ++idx;
            result[i] = tree_[idx];
        }
        return result;
}


template<typename B>
void OramParty<B>::putPath(const int & path, const vector<OramBucket<B> > & buckets) {
    vector<bool> path_bits = intToBits(path);
    putPath(path_bits, buckets);
}


template<typename B>
void OramParty<B>::putPath( const vector<bool> & path, const vector<OramBucket<B> > & buckets) {

    int idx = 1;
    tree_[idx] = buckets[0];
    for(int i = 1; i < tree_depth_; ++i) {
        idx *= 2;
        if(path[tree_depth_ - i])
            ++idx;
        tree_[idx] = buckets[i];
    }

}

template<typename B>
vector<bool> OramParty<B>::intToBits(int64_t src) {
    vector<bool> bits(tree_depth_, 0);
    for(int i = 0; i < tree_depth_; ++i) {
        if((src & i) == 1)
            bits[i] = true;

        src >>= 1;
    }
    return bits;
}
