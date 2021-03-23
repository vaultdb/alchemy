#include <memory>
#include "common/defs.h"
#include "operator.h"
#include <query_table/query_table.h>
#include <vector>


namespace  vaultdb {
    template<typename B>
    class Sort : public Operator<B> {
        SortDefinition sortDefinition;


    public:
        Sort(Operator<B> *child, const SortDefinition &aSortDefinition);
        ~Sort();
        Sort(shared_ptr<QueryTable<B> > child, const SortDefinition &aSortDefinition);

        std::shared_ptr<QueryTable<B> > runSelf() override;


    private:
        void bitonicSort(const int &lo, const int &cnt, const bool &invertDir);

        void bitonicMerge(const int &lo, const int &cnt, const bool &invertDir);

        void compareAndSwap(const int &lhsIdx, const int &rhsIdx, const bool &invertDir);

        B swapTuples(const int & lhsIdx, const int & rhsIdx, const bool & invertDir);

        int powerOfLessThanTwo(const int &n);


    };


}

