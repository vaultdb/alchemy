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
        std::unique_ptr<SortCondition> sortCondition; // pointer-izing it b/c it is an abstract class
        std::unique_ptr<SortCondition> reverseSortCondition;  // for when we need to put tuples in the opposite of the desired final sort order

    public:
        Sort(const SortDefinition &aSortDefinition, std::shared_ptr<Operator> &child);


        std::shared_ptr<QueryTable> runSelf() override;

    private:
        void bitonicSort(const int &lo, const int &cnt, bool invertDir);

        void bitonicMerge(const int &lo, const int &cnt, bool invertDir);

        void compareAndSwap(const int &lhsIdx, const int &rhsIdx, bool invertDir);

        int powerOfLessThanTwo(const int &n);

        static SortDefinition getReverseSortDefinition(const SortDefinition &aSortDef);
    };

}