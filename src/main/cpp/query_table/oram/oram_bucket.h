#ifndef _ORAM_BUCKET_
#define _ORAM_BUCKET_

#include <vector>
#include "query_table/query_tuple.h"
#include "query_table/oram/oram_block.h"

using namespace std;
namespace vaultdb {



    template<typename B>
    class OramBucket {
    public:
        vector<OramBlock<B>> blocks_;
        OramBlock<B> dummy_block_;

        OramBucket(const shared_ptr<QuerySchema> & schema, int z);

        ~OramBucket() = default;

        B isFull();
        B isEmpty();
        OramBlock<B> readAndRemove(const Field<B> & id);
        OramBlock<B> conditionalReadAndRemove(const Field<B> & id, const B & cond);
        void add(const OramBlock<B> & block, const Field<B> & id);
        void conditionalAdd(const OramBlock<B> & block, const Field<B> & id, const B & cond);
        OramBlock<B> pop();
        OramBlock<B> conditionalPop(const B & cond);
        static OramBucket<B> Xor(const OramBucket<B> & a, const OramBucket<B> & b);

    };










}


template class vaultdb::OramBucket<bool>;
template class vaultdb::OramBucket<emp::Bit>;

#endif