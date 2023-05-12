#ifndef _ORAM_CLIENT_
#define _ORAM_CLIENT_

#include <query_table/query_table.h>
#include <map>

using namespace std;

namespace vaultdb {
    template<typename B>
    class OramClient {
        OramClient(int tree_depth, int blocks_per_bucket, const shared_ptr<QuerySchema> & schema);

        QueryTuple<B> readRow(int )

       map<int, int> position_map_; // map ID to path in server

    };
}


#endif