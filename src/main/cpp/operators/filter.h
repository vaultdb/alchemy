#ifndef _FILTER_H
#define _FILTER_H

#include <operators/support/predicate.h>
#include "operator.h"


namespace  vaultdb {

    template<typename T>
    class Filter : public Operator {

        std::shared_ptr<Predicate<T> > predicate;


    public:
        Filter(Operator *child, shared_ptr<Predicate<T> > &predicateClass);

        Filter(shared_ptr<QueryTable> child, shared_ptr<Predicate<T> > &predicateClass);

        ~Filter() = default;

        std::shared_ptr<QueryTable> runSelf()  override;


    };

}


#endif // _FILTER_H
