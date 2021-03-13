#ifndef _FILTER_H
#define _FILTER_H

#include <operators/support/predicate.h>
#include "operator.h"


namespace  vaultdb {

    template<typename T>
    class Filter : public Operator {

        std::shared_ptr<Predicate<T> > predicate;


    public:
        ~Filter() = default;
        std::shared_ptr<QueryTable> runSelf() override {throw; } // should only use specializations
    };


    // template specialization for plaintext
    template <>
        class Filter<BoolField> : public Operator {
            std::shared_ptr<Predicate<BoolField> > predicate;

        public:
            Filter(Operator *child, shared_ptr<Predicate<BoolField> > & predicateClass);
            Filter(shared_ptr<QueryTable> child, shared_ptr<Predicate<BoolField> > & predicateClass);
            ~Filter() = default;

            std::shared_ptr<QueryTable> runSelf() override;

        };


    template <>
    class Filter<SecureBoolField>  : public Operator {
        std::shared_ptr<Predicate<SecureBoolField> > predicate;
    public:
        Filter(Operator *child, shared_ptr<Predicate<SecureBoolField> > & predicateClass);
        Filter(shared_ptr<QueryTable> child, shared_ptr<Predicate<SecureBoolField> > & predicateClass);
        ~Filter() = default;


        std::shared_ptr<QueryTable> runSelf() override;
        //SecureBoolField  predicateEvaluation(const QueryTuple & tuple) const;
    };

}

#endif // _FILTER_H
