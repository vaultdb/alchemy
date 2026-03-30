#ifndef _ORAM_SERVER_H
#define _ORAM_SERVER_H

#include <query_table/oram/oram_bucket.h>

// based on ObliVM: https://github.com/oblivm/ObliVMGC/tree/master/java/com/oblivm/backend/oram

namespace  vaultdb {

    template<typename B>
    class OramParty {
    public:
        OramParty(int N, int tuples_per_bucket, const shared_ptr<QuerySchema> & schema);

        vector<OramBucket<B>> getPath(const int & path);
        void putPath( const int & path, const vector<OramBucket<B> > & buckets);
    private:
        void setup();
        vector<OramBucket<B>> getPath(const vector<bool> & path);
        void putPath(const vector<bool> & path, const vector<OramBucket<B> > & buckets);

        OramBucket<B> dummy_bucket_;

        int tuples_per_bucket_;
        int tree_depth_;
        int bucket_cnt_;

        vector<OramBucket<B>> tree_;

        vector<bool> intToBits(int64_t src);


    };



}

#endif
