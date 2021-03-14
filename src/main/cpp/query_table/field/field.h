#ifndef _FIELD_H_
#define _FIELD_H_


#include "field_type.h"
#include "query_table/query_field_desc.h"
#include <emp-tool/circuits/bit.h>
#include <emp-tool/circuits/integer.h>
#include <emp-tool/circuits/float32.h>
#include <typeinfo>

// TODO: adapt std::string to store payload as string object instead of character array
//  - to unify our interface

// carries payload for each attribute
//  generic for storing heterogeneous types in the same container
//  downcast to impl (e.g., IntField, FloatField, etc.) as needed for expressions
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
            // all state is held here, not in impls (e.g., IntField, BoolField)
             virtual ~Field() = default;

            FieldType getType() const;

            size_t getSize() const;

            template<typename T>
            inline T getValue()  const {
                // use getStringField for this case
                assert(type_ != FieldType::STRING);

                return *(reinterpret_cast<T*>(data_));

            }
//
//            template <typename T>
//            inline void initializePrimitive(const FieldType & fieldType) {
//
//                assert((fieldType == FieldType::BOOL) || (fieldType == FieldType::INT) || (fieldType == FieldType::LONG))
//                allocated_size_ = sizeof(T);
//            }
        template<typename T>
        inline void setValue(const T & src) {
            *(reinterpret_cast<T*>(data_)) = src;
        }

        std::string getStringValue() const;

        void setStringValue(const std::string & src);


            // only really meaningful for plain (not EMP) types
            bool  operator == (const Field &cmp) const;

            bool  operator != (const Field &cmp) const;


            static void compareAndSwap(const bool & choice,  Field & lhs,  Field & rhs);

            static void compareAndSwap(const emp::Bit & choice, const Field & lhs, const Field & rhs);


            // if field is encrypted, decrypt
            Field * reveal(const int & party = emp::PUBLIC) const;

            static Field *
            secretShare(const Field *field, const FieldType &type, const size_t &strLength, const int &myParty,
                        const int &dstParty);

            std::string toString() const;

            void serialize(int8_t *dst) const;

            static Field deserialize(const QueryFieldDesc & fieldDesc, const int8_t * src);
            static Field deserialize(const FieldType & field, const int & strLength, const int8_t *src);

            // only for bool types
            // TODO: remove previous overloads in FieldInstance before using them here
            // meld this with FieldInstance?
        //Field operator !() const;

        /*Field operator+(const Field &rhs) const  {  return static_cast<T const &>(*this) + rhs; }
        Field operator-(const Field &rhs) const  {   return static_cast<const T &>(*this) - rhs; }
        Field operator*(const Field &rhs) const  {   return static_cast<const T &>(*this) * rhs; }
        Field operator/(const Field &rhs) const  {   return static_cast<const T &>(*this) / rhs; }
        Field operator%(const Field &rhs) const  {   return static_cast<const T &>(*this) % rhs; }*/


    protected:
        void copy(const Field & src);

        private:
            static std::string revealString(const emp::Integer & src, const int & party);
            void initialize(const FieldType &type, const size_t &strLength);
    };

    std::ostream &operator<<(std::ostream &os, const Field &aValue);


}
#endif