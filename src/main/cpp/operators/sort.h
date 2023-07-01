#include <memory>
#include "common/defs.h"
#include "operator.h"
#include <query_table/query_table.h>
#include <vector>


// based on EMP Sort
// also: https://www.inf.hs-flensburg.de/lang/algorithmen/sortieren/bitonic/oddn.htm
namespace  vaultdb {
    template<typename B>
    class Sort : public Operator<B> {


    public:
        Sort(Operator<B> *child, const SortDefinition &sort_def, const int & limit = -1);
        Sort(QueryTable<B> *child, const SortDefinition &sort_def, const int & limit = -1);
        virtual ~Sort() = default;

        QueryTable<B> *runSelf() override;

        static void bitonicMerge( QueryTable<B> *table, const SortDefinition & sort_def, const int &lo, const int &cnt, const bool &invertDir,  int & counter);
        static int powerOfTwoLessThan(const int &n);

    private:
        void bitonicSort(const int &lo, const int &cnt, const bool &dir,  int & counter);

        static B swapTuples(const QueryTuple<B> & lhsTuple, const QueryTuple<B> & rhsTuple, const SortDefinition  & sort_definition, const bool & dir);

        static B swapTuples(const QueryTable<B> *table, const int & lhs_idx, const int & rhs_idx, const SortDefinition  & sort_definition, const bool & dir);


    protected:
        string getOperatorType() const override;

        string getParameters() const override;

    private:

        int limit_; // -1 means no limit op


    };



}

