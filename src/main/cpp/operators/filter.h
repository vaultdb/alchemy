#ifndef _FILTER_H
#define _FILTER_H

//#include <operators/support/predicate.h>
#include "operator.h"
#include <operators/expression/generic_expression.h>

namespace  vaultdb {

    template<typename B>
    class Filter : public Operator<B> {

        //std::shared_ptr<Predicate<B> > predicate;
        BoolExpression<B> predicate_;

    public:
        Filter(Operator<B> *child, BoolExpression<B> &predicate);

        Filter(shared_ptr<QueryTable<B> > child, BoolExpression<B> &predicate);

        ~Filter() = default;

        std::shared_ptr<QueryTable<B> > runSelf()  override;


    };

}


#endif // _FILTER_H
