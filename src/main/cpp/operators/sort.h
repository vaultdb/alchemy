#include <memory>
#include "common/defs.h"
#include "operator.h"
#include <operators/support/sort_condition.h>
#include <query_table/query_table.h>
#include <vector>


namespace  vaultdb {
    template<typename B>
    class Sort : public Operator<B> {
        SortDefinition sortDefinition;

        SortCondition<B> sortCondition; // pointer-izing it b/c it is an abstract class
        SortCondition<B> reverseSortCondition;  // for when we need to put tuples in the opposite of the desired final sort order

    public:
        Sort(Operator<B> *child, const SortDefinition &aSortDefinition);
        ~Sort();
        Sort(shared_ptr<QueryTable<B> > child, const SortDefinition &aSortDefinition);

        std::shared_ptr<QueryTable<B> > runSelf() override; // {throw; } // implemented in template specializations

        static SortDefinition getReverseSortDefinition(const SortDefinition &aSortDef);


    private:
        void bitonicSort(const int &lo, const int &cnt, bool invertDir);

        void bitonicMerge(const int &lo, const int &cnt, bool invertDir);

        void compareAndSwap(const int &lhsIdx, const int &rhsIdx, bool invertDir);


        int powerOfLessThanTwo(const int &n);


    };


}

