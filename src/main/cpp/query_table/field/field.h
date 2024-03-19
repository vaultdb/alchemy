#ifndef _FIELD_H_
#define _FIELD_H_


#include "field_type.h"
#include "query_table/query_field_desc.h"
#include <typeinfo>


// carries payload for each attribute
//  generic for storing heterogeneous types in the same container
namespace vaultdb {


    template<typename B> class Field;
    typedef Field<bool> PlainField;
    typedef Field<Bit> SecureField;


    template <typename B>
    class Field {
        public:
            vector<int8_t> payload_;
            FieldType  type_;

        public:
            Field() :   type_(FieldType::INVALID) { } // payload is empty

        template<typename T>
        Field(const FieldType & field_type, const T & val)  : type_(field_type) {
            assert(validateTypeAlignment<T>());
            switch(type_) {
                case FieldType::STRING:
                    setString(*((string *) &val));
                    break;
                case FieldType::SECURE_INT:
                case FieldType::SECURE_LONG:
                case FieldType::SECURE_STRING:
                    setInt(*((Integer *) &val));
                    break;
                case FieldType::SECURE_FLOAT:
                    setFloat(*((Float *) &val));
                    break;
                default: {
                    setValue<T>(val);
                }
            }
        }

        // init with default values for secret share recv
        Field(const FieldType & field_type) : type_(field_type) {
            switch(type_) {
                case FieldType::BOOL:
                    setValue<bool>(false);
                    break;
                case FieldType::INT:
                    setValue<int32_t>(0);
                    break;
                case FieldType::LONG:
                    setValue<int64_t>(0);
                    break;
                case FieldType::STRING:
                    setString("");
                    break;
                case FieldType::FLOAT:
                    setValue<float_t>(0.0);
                    break;
                case FieldType::SECURE_BOOL:
                    setValue<Bit>(Bit(false, PUBLIC));
                    break;
                case FieldType::SECURE_INT:
                    setInt(Integer(32, 0, PUBLIC));
                    break;
                case FieldType::SECURE_LONG:
                    setInt(Integer(64, 0, PUBLIC));
                    break;
                case FieldType::SECURE_STRING:
                    setInt(Integer(8, 0, PUBLIC));
                    break;
                case FieldType::SECURE_FLOAT:
                    setFloat(Float(0, PUBLIC));
                    break;
                default:
                    throw;
            }
       }


        Field(const B & value) {
                setValue(value);
                type_ = (std::is_same_v<bool, B>) ? FieldType::BOOL : FieldType::SECURE_BOOL;
            }

            Field(const Field & field) :  type_(field.type_) {
                assert(field.type_ != FieldType::INVALID);
                payload_ = vector<int8_t>(field.payload_.size());
                memcpy(payload_.data(), field.payload_.data(), field.payload_.size());
            }

            Field & operator=(const Field & other) {
                if(type_ == FieldType::INVALID && other.type_ == FieldType::INVALID) return *this; // nothing to do here
                assert(other.type_ != FieldType::INVALID);

                if(&other == this)
                    return *this;
                payload_ = vector<int8_t>(other.payload_.size());
                memcpy(payload_.data(), other.payload_.data(), other.payload_.size());
                type_ = other.type_;
                return *this;

            }

            ~Field()  = default;

            FieldType getType() const { return type_; }

            template<typename T>
            inline T getValue()  const {
                assert(validateTypeAlignment<T>());
                // all other types are non-primitive and require more setup
                switch (type_) {
                    case FieldType::BOOL:
                    case FieldType::INT:
                    case FieldType::LONG:
                    case FieldType::FLOAT:
                    case FieldType::SECURE_BOOL:
                        return *((T *) payload_.data());;
                    default:
                        throw;

                }
            }

            // need getters and setters for non-primitive types
        inline string getString()  const {
            string s(payload_.size(), ' ');
            memcpy(s.data(), payload_.data(), payload_.size());
            return s;
        }

        inline Integer getInt() const {
                int bit_cnt = payload_.size() / sizeof(Bit);
                Integer i(bit_cnt, 0L);
                memcpy(i.bits.data(), payload_.data(), payload_.size());
                return i;
        }

        inline Float getFloat() const {
            Float f;
            memcpy(f.value.data(), payload_.data(), payload_.size());
            return f;
        }


        template<typename T>
        inline void setValue(const T & src) {
            int dst_size = sizeof(src);
            if(payload_.size() != dst_size)
                payload_.resize(dst_size);
           memcpy(payload_.data(), &src, dst_size);
        }

        inline void setString(const string & src) {
            payload_.resize(src.size());
            memcpy(payload_.data(), src.data(), src.size());
        }

        inline void setInt(const Integer & src) {
            payload_.resize(src.size() * sizeof(Bit));
            memcpy(payload_.data(), src.bits.data(), src.size() * sizeof(Bit));
        }

        inline void setFloat(const Float & src) {
            payload_.resize(src.value.size() * sizeof(emp::Bit));
            memcpy(payload_.data(), src.value.data(), src.value.size() * sizeof(Bit));
        }

        // delegate to visitor
        B  operator == (const Field &cmp) const;
        B  operator != (const Field &cmp) const {
            B eq = *this == cmp;
            return !eq;
        }

        // only applicable to bool/Bit types
        B operator !() const {
            B val = getValue<B>();
            return !val;
        }

        B  operator && (const Field &cmp) const {
            assert(type_ == cmp.getType());

            if(type_ == FieldType::BOOL || type_ == FieldType::SECURE_BOOL) {
                B lhs = getValue<B>();
                B rhs = cmp.getValue<B>();
                B res = lhs & rhs;

                return res;
            }

            // Not applicable for all other types
            throw;
        }
        B  operator || (const Field &cmp) const {
            assert(type_ == cmp.getType());

            if(type_ == FieldType::BOOL || type_ == FieldType::SECURE_BOOL) {
                B lhs = getValue<B>();
                B rhs = cmp.getValue<B>();
                return lhs | rhs;
            }

            // Not applicable for all other types
            throw;
        }

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
        static void compareSwap(const B & choice, Field & lhs, Field & rhs);


        PlainField reveal( const QueryFieldDesc &desc, const int &party = PUBLIC) const;
        SecureField secret_share() const; // secret share as public

        static SecureField secret_share_send(const PlainField &src, const QueryFieldDesc &field_desc, const int &src_party);

        static SecureField secret_share_recv(const QueryFieldDesc &field_desc, const int &src_party);

        static SecureField secretShareHelper(const PlainField &field, const QueryFieldDesc &desc, const int &party,
                                             const bool &send);

        string toString() const;

        void pack(const QueryFieldDesc & schema) {// update this to a packed version of field
            if(type_ != FieldType::SECURE_INT && type_ != FieldType::SECURE_LONG) return;
            int bit_size = payload_.size() / sizeof(Bit);

            Integer si(bit_size, 0, PUBLIC);
            memcpy(si.bits.data(), payload_.data(), payload_.size());
            if(si.size() == (schema.size() + schema.bitPacked())) return; // already packed

            if(schema.bitPacked() && schema.getFieldMin() != 0) {
                si = si - Integer(si.size(), schema.getFieldMin(), PUBLIC);
            }
            si.resize(schema.size());
            auto byte_size = schema.size() * sizeof(Bit);
            payload_.resize(byte_size);
            memcpy(payload_.data(), si.bits.data(), byte_size);
        }


        // keep the field as its in-table representation.  If packed, don't unpack to its original size
        static Field<B> deserialize(const QueryFieldDesc &desc, const int8_t *src);

        void unpack(const QueryFieldDesc & schema)  { // update to an unpacked version of this
            if(type_ != FieldType::SECURE_INT && type_ != FieldType::SECURE_LONG) return;
            int bit_size = payload_.size() / sizeof(Bit);
            int dst_size = type_ == FieldType::SECURE_INT ? 32 : 64;
            if(bit_size == dst_size) return; // already unpacked

            Integer si(bit_size, 0, PUBLIC);
            memcpy(si.bits.data(), payload_.data(), payload_.size());

            si.resize(dst_size);
            if(schema.bitPacked() && schema.getFieldMin() != 0) {
                si = si + Integer(si.size(), schema.getFieldMin(), PUBLIC);
            }

            int byte_size = dst_size * sizeof(Bit);
            payload_.resize(byte_size);
            memcpy(payload_.data(), si.bits.data(), byte_size);
        }


        static string revealString(const Integer & src, const int & party = PUBLIC);
        static Integer secretShareString(const string &s, const bool &to_send, const int &src_party, const int &str_length);

        // copy field to dst (byte array)
        static void serialize(int8_t *dst, const Field<B> & f, const QueryFieldDesc & desc) {

            switch (f.getType()) {
                case FieldType::BOOL:
                case FieldType::INT:
                case FieldType::LONG:
                case FieldType::FLOAT:
                    memcpy(dst, f.payload_.data(), f.payload_.size());
                    break;
                case FieldType::STRING: {
                    string s = f.getString();
                    std::reverse(s.begin(), s.end());
                    memcpy(dst, (int8_t *) s.c_str(), s.size()); // null termination chopped
                    break;
                }
                case FieldType::SECURE_BOOL: {
                    Bit b  = f.template getValue<Bit>();
                    memcpy(dst, &b, sizeof(emp::Bit));
                    break;
                }
                case FieldType::SECURE_INT:
                case FieldType::SECURE_LONG:
                case FieldType::SECURE_STRING: {
                    Integer i = f.getInt();
                    // take the smaller of the two
                    int write_size = desc.size() < i.size() ? desc.size() : i.size();
                    memcpy(dst, i.bits.data(), write_size * sizeof(emp::Bit));
                    break;
                }
                case FieldType::SECURE_FLOAT: {
                    Float fl = f.getFloat();
                    memcpy(dst, fl.value.data(), fl.value.size() * sizeof(emp::Bit));
                    break;
                }
                default:
                    throw;
            }
     }

        private:

        static inline void swap(vector<int8_t> & lhs, vector<int8_t> & rhs) {
            assert(lhs.size() == rhs.size());
            int8_t *l = lhs.data();
            int8_t *r = rhs.data();

            for(int i = 0; i < lhs.size(); ++i) {
                *l = *l ^ *r;
                *r = *r ^ *l;
                *l = *l ^ *r;
                ++l;
                ++r;

            }
        }

        template<typename T>
        inline T xorHelper(const Field<B> & r) const {
            T lhs = *((T *) payload_.data());
            T rhs = *((T *) r.payload_.data());
            return lhs  ^ rhs;
        }

        template<typename T>
        inline T plusHelper(const Field<B> & r) const {
            T lhs = *((T *) payload_.data());
            T rhs = *((T *) r.payload_.data());
            return lhs + rhs;
        }

        template<typename T>
        inline T minusHelper(const Field<B> & r) const {
            T lhs = *((T *) payload_.data());
            T rhs = *((T *) r.payload_.data());
            return lhs - rhs;
        }

        template<typename T>
        inline T timesHelper(const Field<B> & r) const {
            T lhs = *((T *) payload_.data());
            T rhs = *((T *) r.payload_.data());
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
            T lhs = *((T *) payload_.data());
            T rhs = *((T *) r.payload_.data());
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

        static inline string xorStrings(const string & lhs, const string & rhs) {
            assert(lhs.size() == rhs.size());
            string res = lhs;

            for(int i = 0; i < lhs.size(); ++i) {
                res[i] = lhs[i] ^ rhs[i];
            }

            return res;
        }

        template<typename T>
        bool validateTypeAlignment() const {
            bool aligned = true;

            switch (type_) {
                case FieldType::BOOL:
                    aligned = std::is_same<T, bool>::value;
                    break;
                case FieldType::INT:
                    aligned = std::is_same<T, int32_t>::value;
                    break;
                case FieldType::LONG:
                    aligned = std::is_same<T, int64_t>::value;
                    break;
                case FieldType::STRING:
                    aligned = std::is_same<T, string>::value;
                    break;
                case FieldType::FLOAT:
                    aligned = std::is_same<T, float_t>::value;
                    break;
                case FieldType::SECURE_BOOL:
                    aligned = std::is_same<T, Bit>::value;
                    break;
                case FieldType::SECURE_INT:
                case FieldType::SECURE_LONG:
                case FieldType::SECURE_STRING:
                    aligned = std::is_same<T, Integer>::value;
                    break;
                case FieldType::SECURE_FLOAT:
                    aligned = std::is_same<T, Float>::value;
                    break;
                default:
                    throw;
            }

            return aligned;
        }






    };


    std::ostream &operator<<(std::ostream &os, const Field<bool> &val);
    std::ostream &operator<<(std::ostream &os, const Field<Bit> &val);



}
#endif
