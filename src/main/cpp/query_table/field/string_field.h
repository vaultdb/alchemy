#ifndef _STRING_FIELD_H
#define _STRING_FIELD_H


#include "field_instance.h"
#include "bool_field.h"
#include "util/utilities.h"
#include <emp-tool/circuits/integer.h>
#include <emp-tool/execution/protocol_execution.h>

#include <cstdint>
#include <cstring>



namespace vaultdb {
    //  // T = derived field
    //    // P = primitive / payload of field, needed for serialize
    //    // B = boolean field result
    //    template<typename T, typename P, typename B>
    class StringField : public FieldInstance<StringField, std::string, BoolField> {
    protected:
        std::string payload;

    public:

        StringField() {}
        StringField(const StringField & src) { payload = src.payload; }
        StringField(const std::string & src)  { payload = src; }
        StringField(const int8_t * src, const size_t & strLength)  {
                   payload = std::string((char *) src, strLength); // string is null-terminated
                }

        StringField(const emp::Integer &src, const int &party) {
            long bitCount = src.size();
            long byteCount = bitCount / 8;


            bool *bools = new bool[bitCount];
            std::string bitString = src.reveal<std::string>(party);
            std::string::iterator strPos = bitString.begin();
            for(int i =  0; i < bitCount; ++i) {
                bools[i] = (*strPos == '1');
                ++strPos;
            }

            vector<int8_t> decodedBytesVector = Utilities::boolsToBytes(bools, bitCount);
            decodedBytesVector.resize(byteCount + 1);
            decodedBytesVector[byteCount] = '\0';
            payload = string((char * ) decodedBytesVector.data());
            std::reverse(payload.begin(), payload.end());

            delete[] bools;


        }

        StringField& operator=(const StringField& other) {
            this->payload = other.payload;
            return *this;
        }


        static FieldType type() { return FieldType::STRING; }

        bool encrypted() const { return false; }
        size_t size() const override { return payload.length() * 8; }  // payload already padded to max length of field from schema
        void copy(const StringField & src) { payload = src.payload; }
        void assign(const std::string & src) {payload = src; }



        std::string primitive() const { return payload; }
        std::string str() const { return payload; }


        StringField & operator+(const StringField &rhs) const { return *(new StringField(payload + rhs.payload)); }
        StringField & operator-(const StringField &rhs) const {  throw; } // semantics not clear for this and other math ops
        StringField & operator*(const StringField &rhs) const { throw; }
        StringField & operator/(const StringField &rhs) const { throw; }
        StringField & operator%(const StringField &rhs) const { throw;  }


        // comparators
        Field &  operator !() const override { throw;  } // semantics not clear here
        BoolField & operator >= (const StringField &cmp) const { return *(new BoolField(payload >= cmp.payload)); }
        BoolField & operator == (const StringField &cmp) const { return *(new BoolField(payload == cmp.payload)); }


        // swappable
        StringField & select(const BoolField & choice, const StringField & other) const {
            bool selection =  choice.primitive();
            return selection ? *(new StringField(*this)) : *(new StringField(other));
        }

        void serialize(int8_t *dst) const override {
            memcpy(dst, (int8_t *) payload.c_str(), payload.size());
        }

        StringField & operator&(const StringField &right) const { throw; } // bitwise ops not defined for string
        StringField & operator^(const StringField &right) const { throw; } // bitwise ops not defined for string
        StringField & operator|(const StringField &right) const { throw; } // bitwise ops not defined for string


    };






}
#endif //_STRING_FIELD_H
