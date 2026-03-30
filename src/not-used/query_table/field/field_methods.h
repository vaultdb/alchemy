#ifndef _FIELD_METHODS_H
#define _FIELD_METHODS_H

// TODO: may make this "not abstract" if needed
#include "field_type.h"
#include "bool_field.h"

namespace vaultdb {
    // all of the virtual methods each field needs to implement
// curiously recurring pattern
    template<typename T>
    class FieldMethods {
    public:
       virtual const size_t size() const = 0;
       virtual void serialize(int8_t *dst) const = 0;
       virtual FieldType type() const = 0;
       virtual bool encrypted() const = 0;


        virtual T  operator+(const T & rhs) const { return static_cast<T *>(*this) + rhs; }
        virtual T  operator-(const T & rhs) const  { return static_cast<T *>(*this) - rhs; }
        virtual T  operator/(const T & rhs) const  { return static_cast<T *>(*this) / rhs; }
        virtual T  operator*(const T & rhs) const  { return static_cast<T *>(*this) * rhs; }
        virtual T  operator%(const T & rhs) const  { return static_cast<T *>(*this) % rhs; }

        virtual T  operator &(const T & rhs)  const = 0;
        virtual T  operator |(const T & rhs)  const = 0;
        virtual T  operator ^(const T & rhs)  const = 0;




    };
}
#endif //VAULTDB_EMP_FIELD_METHODS_H
