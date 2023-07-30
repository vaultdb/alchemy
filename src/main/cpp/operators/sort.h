#ifndef _SORT_H_
#define _SORT_H_
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
        Operator<B> *clone() const override {
            return new Sort<B>(this->lhs_child_->clone(), this->sort_definition_, this->limit_);
        }

        virtual ~Sort() = default;
        QueryTable<B> *runSelf() override;
        static void bitonicMerge( QueryTable<B> *table, const SortDefinition & sort_def, const int &lo, const int &cnt, const bool &invert_dir,  int & counter);
        static int powerOfTwoLessThan(const int &n);

    protected:
        string getOperatorType() const override;
        string getParameters() const override;

    private:
        void bitonicSort(const int &lo, const int &cnt, const bool &dir,  int & counter);
        static B swapTuples(const QueryTable<B> *table, const int & lhs_idx, const int & rhs_idx, const SortDefinition  & sort_definition, const bool & dir);

        int limit_; // -1 means no LIMIT op


    };



}
#endif
