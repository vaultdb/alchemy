#ifndef _FIELD_H_
#define _FIELD_H_


#include "field_type.h"
#include "query_table/query_field_desc.h"
#include <typeinfo>
#include <boost/variant.hpp>


// carries payload for each attribute
//  generic for storing heterogeneous types in the same container
//  downcast to impl (e.g., IntField, FloatField, etc.) as needed for expressions
namespace vaultdb {


    template<typename B> class Field;
    typedef Field<bool> PlainField;
    typedef Field<emp::Bit> SecureField;


    template <typename B>
    class Field {
        public:
            Value payload_;
            FieldType  type_;
            size_t string_length_;




        public:
            Field();
            Field(const FieldType & fieldType,  const Value & val,  const int & strLength = 0);
            Field(const B & value);

            Field(const Field & field);

            Field & operator=(const Field & other);
            // all state is held here, not in impls (e.g., IntField, BoolField)
            ~Field()  = default;

            FieldType getType() const;

            size_t getSize() const;


            template<typename T>
            inline T getValue()  const {
                return boost::get<T>(payload_);
            }

        template<typename T>
        inline void setValue(const T & src) {
           payload_ = src;
        }




        // delegate to visitor
        B  operator == (const Field &cmp) const;
        B  operator != (const Field &cmp) const;
        B operator !() const;
        B  operator && (const Field &cmp) const;
        B  operator || (const Field &cmp) const;

        B operator>=(const Field & rhs) const;
        B operator<(const Field & rhs) const;
        B operator<=(const Field & rhs) const;
        B operator>(const Field & rhs) const;

        Field  operator^(const Field &rhs) const; // bitwise xor
        Field  operator+(const Field &rhs) const;
        Field  operator-(const Field &rhs) const;
        Field  operator*(const Field &rhs) const;
        Field  operator/(const Field &rhs) const;
        Field  operator%(const Field &rhs) const;

        //Field operator|(const Field &rhs) const; // for sort need bitwise OR


        static Field If(const B & choice,const Field & lhs, const Field & rhs);
        static void compareAndSwap(const B & choice, Field & lhs, Field & rhs);


        PlainField reveal(const int &party = emp::PUBLIC) const;
        SecureField secret_share() const; // secret share as public

        static SecureField
        secret_share_send(const PlainField &src, const QueryFieldDesc &field_desc, const int &src_party);
        static SecureField
        secret_share_recv(const QueryFieldDesc &&field_desc, const int &src_party);

        std::string toString() const;

        void serialize(int8_t *dst, const QueryFieldDesc &schema) const;
        void serializePacked(int8_t *dst, const QueryFieldDesc &schema) const;

        static Field<B> deserialize(const QueryFieldDesc &desc, const int8_t *src);

        // keep the field as its in-table representation.  If packed, don't unpack to its logical size
        static Field<B> deserializePacked(const QueryFieldDesc &desc, const int8_t *src);


        private:
            static std::string revealString(const emp::Integer & src, const int & party);
            static emp::Integer secretShareString(const std::string & s, const bool & to_send, const size_t &
            str_length,
                                                  const int & party);

        template<typename T>
        static inline void swap(Value * lhs, Value * rhs) {
            T l = boost::get<T>(*lhs);
            T r = boost::get<T>(*rhs);

            l = l ^ r;
            r = r ^ l;
            l = l ^ r;

            *lhs = Value(l);
            *rhs = Value(r);
        }

        template<typename T>
        inline T xorHelper(const Field<B> & r) const {
            T lhs = getValue<T>();
            T rhs = r.getValue<T>();
            return lhs  ^ rhs;
        }

        template<typename T>
        inline T plusHelper(const Field<B> & r) const {
            T lhs = getValue<T>();
            T rhs = r.getValue<T>();
            return lhs + rhs;
        }

        template<typename T>
        inline T minusHelper(const Field<B> & r) const {
            T lhs = getValue<T>();
            T rhs = r.getValue<T>();
            return lhs - rhs;
        }

        template<typename T>
        inline T timesHelper(const Field<B> & r) const {
            T lhs = getValue<T>();
            T rhs = r.getValue<T>();
            return lhs * rhs;
        }

        template<typename T>
        inline T divHelper(const Field<B> & r) const {
            T lhs = getValue<T>();
            T rhs = r.getValue<T>();
            return lhs  / rhs;
        }

        template<typename T>
        inline T modHelper(const Field<B> & r) const {
            T lhs = getValue<T>();
            T rhs = r.getValue<T>();
            return lhs  % rhs;
        }

        template<typename T>
        inline T bitwiseXor(const Field<B> & r) const {
            T lhs = getValue<T>();
            T rhs = r.getValue<T>();
            T res;

            int8_t *l_cursor = (int8_t *) &lhs;
            int8_t *r_cursor = (int8_t *)  &rhs;
            int8_t *dst = (int8_t *)  &res;
            for(int i = 0; i < sizeof(T); ++i) {
                *dst = *l_cursor ^ *r_cursor;
                ++dst;
                ++l_cursor;
                ++r_cursor;
            }
            return res;
        }

        static inline std::string xorStrings(const std::string & lhs, const std::string & rhs) {
            assert(lhs.size() == rhs.size());
            std::string res = lhs;

            for(int i = 0; i < lhs.size(); ++i) {
                res[i] = lhs[i] ^ rhs[i];
            }

            return res;
        }

        static Value secretShareHelper(const PlainField &field, const QueryFieldDesc &field_desc, const int &party,
                                       const bool &send);

    };



    std::ostream &operator<<(std::ostream &os, const Field<bool> &aValue);
    std::ostream &operator<<(std::ostream &os, const Field<emp::Bit> &aValue);



}
#endif
