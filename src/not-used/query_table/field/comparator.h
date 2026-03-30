#ifndef _COMPARABLE_H
#define _COMPARABLE_H

//#include "field_methods.h"
#include <emp-tool/circuits/bit.h>

namespace  vaultdb {

  //  typedef boost::variant<bool, emp::Bit> bool_t;
   // class BoolField;


    template<typename T, typename B>
    class Comparator {



        virtual B geq(const T &rhs) const = 0;
        virtual B equal(const T &rhs) const = 0;
        virtual B negate() const = 0;
        T select(const B  & choice, const T & other) const {
            return static_cast<T &>(*this).select(other);
        }


        // comparators - based on EMP-toolkit comparable.h

        B operator>=(const T &rhs) const {
            return this->geq(rhs);

        }

        virtual B operator!() const {
            this->negate();
        }


         B operator<(const T &rhs) const {
            return !(*this >= rhs);
        }

        B operator<=(const T &rhs) const {
            return rhs >= *this;
        }

        B operator>(const T &rhs) const {
            return !(rhs >= *this);
        }

        B operator==(const T &rhs) const {
            return this->equal(rhs);
        }

        B operator!=(const T &rhs) const {
            return !(*this == rhs);
        }



    };



}



#endif //VAULTDB_EMP_COMPARABLE_H
