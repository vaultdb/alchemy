#ifndef _FIELD_H
#define _FIELD_H

#include <cstdint>
#include <string>


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
        virtual Field *reveal() const = 0;

        // caution: these methods (comparison and math) will create memory leaks
        // if we don't handle the output of this as a heap-allocated pointer!
        virtual Field & operator+(const Field & rhs) const = 0;

        Field & operator >=(const Field & rhs) const {
            return geq(rhs);
        }

    protected:

        virtual void copyTo(const Field & other) = 0;
        virtual Field & geq(const Field & rhs) const = 0;


        // Field class needs to be inherited
        Field() = default;
        Field(const Field&) = default;
        Field(Field&&) = default;



    };
}
#endif //_FIELD_H
