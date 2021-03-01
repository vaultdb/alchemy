#ifndef _SECURE_STRING_FIELD_H
#define _SECURE_STRING_FIELD_H


#include "field_instance.h"
#include "util/utilities.h"
#include <emp-tool/circuits/integer.h>
#include <emp-tool/execution/protocol_execution.h>

#include <cstdint>
#include <cstring>



namespace vaultdb {
    //  // T = derived field
    //    // P = primitive / payload of field, needed for serialize
    //    // B = boolean field result
    //    template<typename T, typename R, typename B, typename P>
class SecureStringField : public FieldInstance<SecureStringField, emp::Integer, SecureBoolField> {
    protected:
       emp::Integer payload;

    public:

        SecureStringField() {}
        SecureStringField(const SecureStringField & src) { payload = src.payload; }
        SecureStringField(const emp::Integer & src) { payload = src; }
        SecureStringField(const int8_t * src, const size_t & strLength)   {
            payload = emp::Integer(strLength * 8, 0, emp::PUBLIC);
            memcpy(payload.bits.data(), src, strLength*sizeof(emp::Bit));
        }

        SecureStringField(const std::string & src, const int & myParty, const int & dstParty)  {
            std::string input = (myParty == dstParty) ?
                                src :
                                std::to_string(0);

            size_t stringByteCount = src.size();

            assert(input.length() <= stringByteCount);   // while loop will be infinite if the string is already oversized

            while(input.length() != stringByteCount) { // pad it to the right length
                input += " ";
            }

            std::string inputReversed = input;
            std::reverse(inputReversed.begin(), inputReversed.end());
            bool *bools = Utilities::bytesToBool((int8_t *) inputReversed.c_str(), stringByteCount);


            payload = emp::Integer(stringByteCount * 8, 0L, dstParty);
            if(myParty == dstParty) {
                emp::ProtocolExecution::prot_exec->feed((emp::block *)payload.bits.data(), dstParty, bools, size());
            }
            else {
                emp::ProtocolExecution::prot_exec->feed((emp::block *)payload.bits.data(), dstParty, nullptr, size());
            }


            delete [] bools;
        }

        SecureStringField& operator=(const SecureStringField& other) {
            this->payload = other.payload;
            return *this;
        }

        bool encrypted() { return true; }
        static FieldType type() { return FieldType::SECURE_STRING; }
        static FieldType revealedType() { return FieldType::STRING; }

        size_t size() const override { return payload.size(); }
        void copy(const SecureStringField & src) { payload = src.payload; }
        void assign(const emp::Integer & src) {payload = src; }

        Field & encrypt(const int & myParty, const int & dstParty = emp::PUBLIC) const {
            return *(new SecureStringField(*this));
        } // copy constructor

       std::string decrypt(const int & party) const {

           long bitCount = payload.size();
           long byteCount = bitCount / 8;


           bool *bools = new bool[bitCount];
           std::string bitString = payload.reveal<std::string>(party);
           std::string::iterator strPos = bitString.begin();
           for(int i =  0; i < bitCount; ++i) {
               bools[i] = (*strPos == '1');
               ++strPos;
           }

           vector<int8_t> decodedBytesVector = Utilities::boolsToBytes(bools, bitCount);
           decodedBytesVector.resize(byteCount + 1);
           decodedBytesVector[byteCount] = '\0';
           string dst((char * ) decodedBytesVector.data());
           std::reverse(dst.begin(), dst.end());

           delete[] bools;

           return dst;

       }


        emp::Integer primitive() const { return payload; }
        std::string str() const { return "SECRET STRING"; }


        SecureStringField & operator+(const SecureStringField &rhs) const { return *(new SecureStringField(payload + rhs.payload)); }
        SecureStringField & operator-(const SecureStringField &rhs) const {  throw; } // semantics not clear for this and other math ops
        SecureStringField & operator*(const SecureStringField &rhs) const { throw; }
        SecureStringField & operator/(const SecureStringField &rhs) const { throw; }
        SecureStringField & operator%(const SecureStringField &rhs) const { throw;  }


        // comparators
        Field &  operator !() const override { throw;  } // semantics not clear here
        SecureBoolField & operator >= (const SecureStringField &cmp) const { return *(new SecureBoolField(payload >= cmp.payload)); }
        SecureBoolField & operator == (const SecureStringField &cmp) const { return *(new SecureBoolField(payload == cmp.payload)); }


        // swappable
        SecureStringField & select(const SecureBoolField & choice, const SecureStringField & other) const {
            emp::Bit selection =  choice.primitive();
            emp::Integer result = emp::If(selection, payload, other.payload);
            return *(new SecureStringField(result));
        }

        void serialize(int8_t *dst) const override {
            memcpy(dst, (int8_t *) payload.bits.data(), size()/8);
        }

        SecureStringField & operator&(const SecureStringField &right) const { throw; } // bitwise ops not defined for string
        SecureStringField & operator^(const SecureStringField &right) const { throw; } // bitwise ops not defined for string
        SecureStringField & operator|(const SecureStringField &right) const { throw; } // bitwise ops not defined for string


    };


}
#endif //_STRING_FIELD_H
