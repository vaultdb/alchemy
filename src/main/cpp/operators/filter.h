#ifndef _FILTER_H
#define _FILTER_H

#include "operator.h"
#include "expression/expression.h"

namespace  vaultdb {

    template<typename B>
    class Filter : public Operator<B> {
        public:
            Expression<B> *predicate_ = nullptr;


            Filter(Operator<B> *child, Expression<B> *predicate);

            Filter(QueryTable<B> *child, Expression<B> *predicate);

            Operator<B> *clone() const override {
                return new Filter<B>(this->lhs_child_->clone(), this->predicate_->clone());
            }

            virtual ~Filter() {
                 if(predicate_ != nullptr) delete predicate_;
            }


    protected:

        QueryTable<B> *runSelf()  override;
        string getOperatorType() const override;
        string getParameters() const override;


    };

}


#endif // _FILTER_H
