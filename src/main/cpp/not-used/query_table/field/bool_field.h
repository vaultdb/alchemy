#ifndef BOOL_FIELD_H
#define BOOL_FIELD_H

#include "comparator.h"
#include "field_methods.h"

// special case, wrapper for PlainBoolField and SecureBoolField
// needs to be self-contained for use in comparator
// but can't be entirely self-contained because we want it to propogate, e.g., for complex predicates
// comparators need to return by value
namespace vaultdb {

    //typedef boost::variant<bool, emp::Bit> bool_t;

    // BoolField needs to be standalone to use as output in comparators
    // otherwise gets infinite dependency loop with PlainBoolField and SecureBoolField
    class BoolField   {
    public:

        BoolField(const bool & src) {
            payload = src;
        }

        BoolField(const emp::Bit & src) {
            payload = src;
            encrypted = true;
        }

        void setPayload(const emp::Bit & p) {
            encrypted = true;
            payload = p;
        }

        void setPayload(const bool & p) {
            encrypted = false;
            payload = p;
        }



    private:

        bool payload;


    };
}

#endif //VAULTDB_EMP_BOOLFIELD_H
