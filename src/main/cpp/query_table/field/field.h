#ifndef _FIELD_H_
#define _FIELD_H_


#include "field_type.h"
#include "field_instance.h"
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
    // forward declaration
    class BoolField;
    class SecureBoolField;

    template<typename B> class Field;
    typedef Field<BoolField> PlainField;
    typedef Field<SecureBoolField> SecureField;


    template <typename B>
    class Field {
        protected:
            FieldType  type_;
            size_t allocated_size_;
            // inspired by NoisePage: https://github.com/cmu-db/noisepage
            std::unique_ptr<std::byte[]> managed_data_;
            std::byte *data_;


        public:
            Field() : type_(FieldType::INVALID), instance_(nullptr) { }
            Field(const FieldType & typeId, const int & strLength = 0);

            Field(const Field & field);

            Field & operator=(const Field & other);
            // all state is held here, not in impls (e.g., IntField, BoolField)
             virtual ~Field() = default;

            FieldType getType() const;

            size_t getSize() const;

            // instance to the wrapper around this field that overloads its operators
            FieldInstance<B> *getInstance() const { return instance_.get(); }

            template<typename T>
            inline T getValue()  const {
                // use getStringField for this case
                assert(type_ != FieldType::STRING);

                return *(reinterpret_cast<T*>(data_));

            }

        template<typename T>
        inline void setValue(const T & src) {
            *(reinterpret_cast<T*>(data_)) = src;
        }

        std::string getStringValue() const;
        std::byte *getData() const { return data_; }

        void setStringValue(const std::string & src);


            // delegate to instance_
            virtual B  operator == (const Field &cmp) const;
            virtual B  operator != (const Field &cmp) const;
            virtual B operator !() const;
            virtual B operator>=(const Field & rhs) const;
            virtual B operator<(const Field & rhs) const;
            virtual B operator<=(const Field & rhs) const;
            virtual B operator>(const Field & rhs) const;

            virtual Field  operator+(const Field &rhs) const;
            virtual Field  operator-(const Field &rhs) const;
            virtual Field  operator*(const Field &rhs) const;
            virtual Field  operator/(const Field &rhs) const;
            virtual Field  operator%(const Field &rhs) const;


        static Field If(const B & choice,const Field & lhs, const Field & rhs);
            // need to keep BoolField out of header file to avoid circular dependency

            static void compareAndSwap(const B & choice, Field & lhs, Field & rhs);


            // if field is encrypted, decrypt
            PlainField reveal(const int & party = emp::PUBLIC) const;

            static SecureField
            secretShare(const PlainField *field, const FieldType &type, const size_t &strLength, const int &myParty,
                        const int &dstParty);

            std::string toString() const;

            void serialize(int8_t *dst) const;


            static Field deserialize(const QueryFieldDesc & fieldDesc, const int8_t * src);
            static Field deserialize(const FieldType & field, const int & strLength, const int8_t *src);



    protected:
        void copy(const Field & src);

        private:
            static std::string revealString(const emp::Integer & src, const int & party);
            void initialize(const FieldType &type, const size_t &strLength);
            std::unique_ptr<FieldInstance<B> > instance_;
    };

   template<typename B>
    std::ostream &operator<<(std::ostream &os, const Field<B> &aValue);



}
#endif