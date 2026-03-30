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
        OramBlock<B> dummy_block_;
        int blocks_per_bucket_;
        int log_n_;
        int loglogN;

        OramBucket(const shared_ptr<QuerySchema> & schema, int z, int logN)  :  dummy_block_(schema), blocks_per_bucket_(z), log_n_(logN) {}

        ~OramBucket() = default;

        B isFull(const vector<OramBlock<B>> & blocks);
        B isEmpty(const vector<OramBlock<B>> & blocks);
        OramBlock<B> readAndRemove(vector<OramBlock<B>> & blocks, const Field<B> & id);
        OramBlock<B> conditionalReadAndRemove(vector<OramBlock<B>> & blocks, const Field<B> & id, const B & cond);
        void add(vector<OramBlock<B>> & blocks, const OramBlock<B> & block, const Field<B> & id);
        void conditionalAdd(vector<OramBlock<B>> & blocks, const OramBlock<B> & block, const Field<B> & id, const B & cond);
        OramBlock<B> pop(vector<OramBlock<B>> & blocks);
        OramBlock<B> conditionalPop(vector<OramBlock<B>> & blocks, const B & cond);
        static vector<OramBlock<B>>  Xor(const vector<OramBlock<B>> & a, const vector<OramBlock<B>>  & b);

        // CircuitLib in ObliVM
        Field<B> deepestLevelLocal(const Field<B> & pos, const Field<B> & path );
        vector<Field<B>> deepestBlockShort(const vector<OramBlock<B>> & blocks, const Field<B> & path);
         void flush(vector<vector<OramBlock<B>>> & blocks,  vector<bool> & path, vector<OramBlock<B>> & srcQueue);

         vector<emp::Bit> leadingZeros(const vector<emp::Bit> & src);
        vector<bool> leadingZeros(const vector<bool> & src);
    };


}


#endif