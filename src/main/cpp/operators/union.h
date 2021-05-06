#ifndef _UNION_H
#define _UNION_H

#include "operator.h"


// concatenates two input arrays.  Equiavalent to UNION ALL in SQL
namespace  vaultdb {

    template<typename B>
    class Union : public Operator<B> {


    public:
        Union(Operator<B> *lhs, Operator<B> *rhs, const SortDefinition & sort = SortDefinition()) : Operator<B>(lhs, rhs, sort) {
            Operator<B>::output_schema_ = lhs->getOutputSchema();
        }

        Union(shared_ptr<QueryTable<B> > lhs, shared_ptr<QueryTable<B> > rhs, const SortDefinition & sort = SortDefinition()) : Operator<B>(lhs, rhs, sort) {
            Operator<B>::output_schema_ = *lhs->getSchema();
        }

        ~Union() = default;

        std::shared_ptr<QueryTable<B> > runSelf()  override;


    };


}



#endif //_UNION_H
