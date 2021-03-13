#ifndef _PREDICATE_CLASS_H
#define _PREDICATE_CLASS_H

#include <query_table/query_tuple.h>

using namespace emp;

// predicates will inherit from this class
// it is a shell for creating state for the predicate, e.g., encrypted constants for equality checks
namespace vaultdb {
    template<typename  T>
    class Predicate {

    public:
        Predicate() {}
        virtual ~Predicate() {}
        // override when we instantiate a predicate
        virtual T predicateCall(const QueryTuple & aTuple) const = 0;

    };

    template class Predicate<BoolField>;
    template class Predicate<SecureBoolField>;

}



//typedef  Field * (Predicate::*predicateCall)(const QueryTuple & aTuple);
#endif //_PREDICATE_CLASS_H
