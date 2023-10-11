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
        Sort(const Sort & src) : Operator<B>(src),  limit_(src.limit_), sort_key_map_(src.sort_key_map_), sort_key_size_bits_(src.sort_key_size_bits_), projected_schema_(src.projected_schema_) {}

        Operator<B> *clone() const override {
            return new Sort<B>(*this);
        }

        virtual ~Sort() = default;
        QueryTable<B> *runSelf() override;
//        static void bitonicMerge( QueryTable<B> *table, const SortDefinition & sort_def, const int &lo, const int &cnt, const bool &invert_dir,  int & counter);
        void bitonicMergeNormalized( QueryTable<B> *table, const SortDefinition & sort_def, const int &lo, const int &cnt, const bool &invert_dir,  int & counter);

        static int powerOfTwoLessThan(const int &n);
        QueryTable<B> *normalizeTable(QueryTable<B> *src);
        QueryTable<B> *denormalizeTable(QueryTable<B> *src);

        int getLimit() const { return limit_; }
        void updateCollation() override {}

        bool operator==(const Operator<B> & rhs) const override {
            if (rhs.getType() != OperatorType::SORT) {
                return false;
            }

            auto rhs_node = dynamic_cast<const Sort<B> &>(rhs);
            if(this->limit_ != rhs_node.limit_) return false;
            return this->operatorEquality(rhs);
        }

    protected:
        OperatorType getType() const override { return OperatorType::SORT; }
        string getParameters() const override;

        int limit_; // -1 means no LIMIT op
        map<int, int> sort_key_map_;
        int sort_key_size_bits_ = 0;
        QuerySchema projected_schema_;


    private:
//        void bitonicSort(const int &lo, const int &cnt, const bool &dir,  int & counter);
        void bitonicSortNormalized(const int &lo, const int &cnt, const bool &dir,  int & counter);

//        static B swapTuples(const QueryTable<B> *table, const int & lhs_idx, const int & rhs_idx, const SortDefinition  & sort_definition, const bool & dir);
        static Bit swapTuplesNormalized(const QueryTable<Bit> *table, const int & lhs_idx, const int & rhs_idx, const bool & dir, const int & sort_key_width_bits);
        static bool swapTuplesNormalized(const QueryTable<bool> *table, const int & lhs_idx, const int & rhs_idx, const bool & dir, const int & sort_key_width_bits);
//        static Bit swapTuplesNormalizedOmpc(const QueryTable<Bit> *table, const int &lhs_idx, const int &rhs_idx, const bool &dir,
//                                            const int &sort_key_width_bits);



    };



}
#endif
