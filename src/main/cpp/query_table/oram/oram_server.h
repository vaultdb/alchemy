#ifndef _ORAM_SERVER_H
#define _ORAM_SERVER_H

#include <query_table/oram/oram_bucket.h>

namespace  vaultdb {

    class OramServer {
    public:
        OramServer(int bucket_count, );

    private:
        int bucket_size_;
        vector<int8_t> buckets_;

    };

}

#endif /
