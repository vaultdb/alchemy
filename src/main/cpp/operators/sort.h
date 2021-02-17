#include <memory>
#include "common/defs.h"
#include "operator.h"
#include <operators/support/sort_condition.h>
#include <query_table/query_table.h>
#include <vector>
#include <operators/support/secure_sort_condition.h>
#include <operators/support/plain_sort_condition.h>


namespace  vaultdb {
    class Sort : public Operator {
        SortDefinition sortDefinition;

        SortCondition *sortCondition = 0; // pointer-izing it b/c it is an abstract class
        SortCondition *reverseSortCondition = 0;  // for when we need to put tuples in the opposite of the desired final sort order

    public:
        Sort(Operator *child, const SortDefinition &aSortDefinition);
        ~Sort();
        Sort(shared_ptr<QueryTable> child, const SortDefinition &aSortDefinition);

        std::shared_ptr<QueryTable> runSelf() override;

    private:
        void bitonicSort(const int &lo, const int &cnt, bool invertDir);

        void bitonicMerge(const int &lo, const int &cnt, bool invertDir);

        void compareAndSwap(const int &lhsIdx, const int &rhsIdx, bool invertDir);

        int powerOfLessThanTwo(const int &n);

        static SortDefinition getReverseSortDefinition(const SortDefinition &aSortDef);
    };

}