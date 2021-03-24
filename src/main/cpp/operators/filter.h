#ifndef _FILTER_H
#define _FILTER_H

#include <operators/support/predicate.h>
#include "operator.h"


namespace  vaultdb {

    template<typename B>
    class Filter : public Operator<B> {

        std::shared_ptr<Predicate<B> > predicate;


    public:
        Filter(Operator<B> *child, shared_ptr<Predicate<B> > &predicateClass);

        Filter(shared_ptr<QueryTable<B> > child, shared_ptr<Predicate<B> > &predicateClass);

        ~Filter() = default;

        std::shared_ptr<QueryTable<B> > runSelf()  override;


    };

}


#endif // _FILTER_H
