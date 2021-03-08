#ifndef _FIELD_H_
#define _FIELD_H_


#include "field_type.h"
#include "query_table/query_field_desc.h"
#include <emp-tool/circuits/bit.h>
#include <emp-tool/circuits/integer.h>
#include <emp-tool/circuits/float32.h>



namespace vaultdb {
    class Field {
    protected:
        FieldType  type_;
        size_t allocated_size_;
        // inspired by NoisePage: https://github.com/cmu-db/noisepage
        std::unique_ptr<std::byte[]> managed_data_;
        std::byte *data_;


    public:
        Field() : type_(FieldType::INVALID) { }
        Field(const FieldType & typeId, const int & strLength = 0);

        Field(const Field & field);

        Field & operator=(const Field & other);

        ~Field() = default;

        FieldType getType() const;

        size_t getSize() const;

        template<typename T>
        T getValue()  const;

        std::string getValue() const;

        template<typename T>
        void setValue(const T & src);

        void setValue(const std::string & src);

        std::byte *getData() const;


        // only really meaningful for plain (not EMP) types
        bool  operator == (const Field &cmp) const;

        bool  operator != (const Field &cmp) const;



        static Field createBool(const bool & value);


        static Field createInt32(const int32_t & value);

        static Field createInt64(const int64_t & value);

        static Field createFloat32(const float_t & value);

        // strings must be padded to max field length before calling this
        // In other words: field length == string length
        static Field createString(const std::string & value);

        static Field createSecureBool(const emp::Bit & value);

        static Field createSecureFloat(const emp::Float & value);

        static Field createSecureInt32(const emp::Integer & value);

        static Field createSecureInt64(const emp::Integer & value);


        static Field createSecureString(const emp::Integer & value);


        static void compareAndSwap(const bool & choice,  Field & lhs,  Field & rhs);

        static void compareAndSwap(const emp::Bit & choice, const Field & lhs, const Field & rhs);


        // TODO: refactor this out of impls, e.g., SecureBoolField
        // if field is encrypted, decrypt
        Field reveal(const int & party = emp::PUBLIC) const;

        // TODO: handle strings
        std::string toString() const;

        void serialize(int8_t *dst) const;

        static Field deserialize(const QueryFieldDesc & fieldDesc, const int8_t * src);
        static Field deserialize(const FieldType & field, const int & strLength, const int8_t *src);

    };

    std::ostream &operator<<(std::ostream &os, const Field &aValue);


}
#endif