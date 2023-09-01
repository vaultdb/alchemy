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

            Filter(const Filter<B> &src) : Operator<B>(src) {
                predicate_ = src.predicate_->clone();
            }
            Operator<B> *clone() const override {
                return new Filter<B>(*this);
            }

            virtual ~Filter() {
                 if(predicate_ != nullptr) delete predicate_;
            }

            void updateCollation() override {
                this->getChild(0)->updateCollation();
                this->sort_definition_ = this->getChild(0)->getSortOrder();
            }

            bool operator==(const Operator<B> &other) const override {
                if (other.getType() != OperatorType::FILTER) {
                    return false;
                }

                auto other_node = dynamic_cast<const Filter<B> &>(other);
                if(*this->predicate_ != *other_node.predicate_)   return false;

                return this->operatorEquality(other);
            }

    protected:

        QueryTable<B> *runSelf()  override;

        OperatorType getType() const override {
            return OperatorType::FILTER;
        }
        // in cpp to avoid circular dependency
        string getParameters() const override;


    };

}


#endif // _FILTER_H
