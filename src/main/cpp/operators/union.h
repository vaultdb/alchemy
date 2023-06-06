#ifndef _UNION_H
#define _UNION_H

#include "operator.h"


// concatenates two input arrays.  Equivalent to UNION ALL in SQL
namespace  vaultdb {

    template<typename B>
    class Union : public Operator<B> {


    public:
        // not order preserving, so no sort definition
        Union(Operator<B> *lhs, Operator<B> *rhs);

        Union(QueryTable<B> *lhs, QueryTable<B> *rhs);

        virtual ~Union() = default;


    protected:
        QueryTable<B> *runSelf()  override;
        string getOperatorType() const override;
        string getParameters() const override;
        void setupScheme() {

        }


    };


}



#endif //_UNION_H
