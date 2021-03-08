#ifndef _COMPUTABLE_H
#define _COMPUTABLE_H

#include "bool_field.h"

namespace vaultdb {
    class BoolField;

    template<typename T>
    class Computable {
        T select(const BoolField  & choice, const T & other) const = 0;

            virtual T  operator+(const T & rhs) const = 0;
            virtual T  operator-(const T & rhs) const = 0;
            virtual T  operator/(const T & rhs) const = 0;
            virtual T  operator*(const T & rhs) const = 0;
            virtual T  operator%(const T & rhs) const = 0;

          virtual T  operator &(const T & rhs)  const = 0;
          virtual T  operator |(const T & rhs)  const = 0;
          virtual T  operator ^(const T & rhs)  const = 0;

    }
    };
}
#endif //_COMPUTABLE_H
