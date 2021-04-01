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




        static void bitonicMerge( std::shared_ptr<QueryTable<B> > & table, const SortDefinition & sort_def, const int &lo, const int &cnt, const bool &invertDir);

    private:
        void bitonicSort(const int &lo, const int &cnt, const bool &invertDir);

        void compareAndSwap(const int &lhsIdx, const int &rhsIdx, const bool &invertDir);

        static B swapTuples(const QueryTuple<B> & lhsTuple, const QueryTuple<B> & rhsTuple, const SortDefinition  & sort_definition, const bool & invertDir);

        static int powerOfLessThanTwo(const int &n);


    };


}

