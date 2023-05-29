#ifndef _FILTER_H
#define _FILTER_H

#include "operator.h"
#include "expression/expression.h"

namespace  vaultdb {

    template<typename B>
    class Filter : public Operator<B> {

        Expression<B> *predicate_ = nullptr;

    public:
        Filter(Operator<B> *child, Expression<B> *predicate);

        Filter(shared_ptr<QueryTable<B> > child, Expression<B> *predicate);

        ~Filter() {
            if(predicate_ != nullptr) delete predicate_;
        }


    protected:

        std::shared_ptr<QueryTable<B> > runSelf()  override;
        string getOperatorType() const override;
        string getParameters() const override;


    };

}


#endif // _FILTER_H
