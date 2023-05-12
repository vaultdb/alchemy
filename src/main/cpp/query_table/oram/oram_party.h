#ifndef _ORAM_SERVER_H
#define _ORAM_SERVER_H

#include <query_table/oram/oram_bucket.h>

// based on ObliVM: https://github.com/oblivm/ObliVMGC/tree/master/java/com/oblivm/backend/oram

namespace  vaultdb {

    template<typename B>
    class OramParty {
    public:
        OramParty(int tree_depth, int tuples_per_bucket, const shared_ptr<QuerySchema> & schema);

        OramBucket<B> *readBucket(const int & path, const int & depth);

    private:
        int tuples_per_bucket_;
        int tree_depth_;

        vector<OramBucket<B>> payload_;

        vector<bool> intToBits(const int & src);

    };



}

#endif
