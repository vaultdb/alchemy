#ifndef _UNION_H
#define _UNION_H

#include "operator.h"


// concatenates two input arrays.  Equiavalent to UNION ALL in SQL
namespace  vaultdb {

    template<typename B>
    class Union : public Operator<B> {


    public:
        Union(Operator<B> *lhs, Operator<B> *rhs) : Operator<B>(lhs, rhs) { }

        Union(shared_ptr<QueryTable<B> > lhs, shared_ptr<QueryTable<B> > rhs) : Operator<B>(lhs, rhs) { }

        ~Union() = default;

        std::shared_ptr<QueryTable<B> > runSelf()  override;


    };


}



#endif //_UNION_H
