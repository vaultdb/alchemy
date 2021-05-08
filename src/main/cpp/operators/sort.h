#include <memory>
#include "common/defs.h"
#include "operator.h"
#include <query_table/query_table.h>
#include <vector>


// based on EMP Sort
namespace  vaultdb {
    template<typename B>
    class Sort : public Operator<B> {


    public:
        Sort(Operator<B> *child, const SortDefinition &aSortDefinition, const int & limit = -1);
        Sort(shared_ptr<QueryTable<B> > child, const SortDefinition &aSortDefinition, const int & limit = -1);
        ~Sort();

        std::shared_ptr<QueryTable<B> > runSelf() override;




        static void bitonicMerge( std::shared_ptr<QueryTable<B> > & table, const SortDefinition & sort_def, const int &lo, const int &cnt, const bool &invertDir);

    private:
        void bitonicSort(const int &lo, const int &cnt, const bool &invertDir);

        static B swapTuples(const QueryTuple<B> & lhsTuple, const QueryTuple<B> & rhsTuple, const SortDefinition  & sort_definition, const bool & invertDir);

        static int powerOfLessThanTwo(const int &n);

    protected:
        string getOperatorType() const override;

        string getParameters() const override;

    private:

        int limit_; // -1 means no limit op, TODO: optimize this to minimize our sort later, i.e., top-k optimization


    };


}

