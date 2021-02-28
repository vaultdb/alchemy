#ifndef _FIELD_H
#define _FIELD_H

#include <cstdint>
#include <string>
#include <memory>


namespace vaultdb {
    enum class FieldType {
        INVALID = 0,
        BOOL,
        DATE, // DATE is just a stand-in for long, TODO: delete this
        INT32,
        INT64,
        FLOAT32,
        STRING,
        SECURE_INT32,
        SECURE_INT64,
        SECURE_BOOL,
        SECURE_FLOAT32,
        SECURE_STRING, // need all types to have encrypted counterpart so that we can translate them back to query tables when we decrypt the results
    };

    // top-level class so we can store heterogeneous types in the same container
    // TODO: overload the << operator
    class Field {
    public:
        virtual ~Field() = default;

        virtual std::unique_ptr<Field> clone() const = 0;



        virtual Field & operator=(const Field& other) {
            this->copyTo(other);
            return *this;
        }


        virtual std::string toString() const = 0;
        virtual void serialize(int8_t *dst) const = 0;
        virtual size_t size() const  = 0;

        virtual Field *reveal() const = 0;

        // caution: these methods (comparison and math) will create memory leaks
        // if we don't handle the output of this as a heap-allocated pointer!
        virtual Field & operator+(const Field & rhs) const = 0;
        virtual Field & operator-(const Field & rhs) const = 0;
        virtual Field & operator/(const Field & rhs) const = 0;
        virtual Field & operator*(const Field & rhs) const = 0;
        virtual Field & operator%(const Field & rhs) const = 0;

        virtual Field & compareAndSwap(const Field & select, const Field & rhs) const = 0;



        // comparators - based on EMP-toolkit comparable.h
        Field & operator >=(const Field & rhs) const {
            return this->geq(rhs);
        }

        Field & operator<(const Field & rhs) const {
            return !( *this >= rhs );
        }

        Field & operator<=(const Field & rhs) const {
            return rhs >= *this;
        }

        Field & operator>(const Field & rhs) const {
            return !(rhs >= *this);
        }

        Field & operator==(const Field & rhs) const {
            return this->equal(rhs);
        }
        Field & operator!=(const Field & rhs) const {
            return !(*this == rhs);
        }

        virtual Field & operator &(const Field & rhs)  const = 0;
        virtual Field & operator |(const Field & rhs)  const = 0;
        virtual Field & operator ^(const Field & rhs)  const = 0;



        virtual void copyTo(const Field & other) = 0;
        virtual Field & geq(const Field & rhs) const = 0;
        virtual Field & equal(const Field & rhs) const = 0;
        virtual Field &  operator !() const = 0;

    protected:

        // Field class needs to be inherited
        Field() = default;
        Field(const Field&) = default;
        Field(Field&&) = default;




    };
    /*std::ostream &operator<<(std::ostream &os, const Field &field) {
        return os << field.toString();
    }*/

}
#endif //_FIELD_H
