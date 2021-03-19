#include "field.h"
#include "util/utilities.h"
#include "field_factory.h"
#include "secure_bool_field.h"
#include "secure_int_field.h"
#include "secure_long_field.h"
#include "secure_float_field.h"
#include "secure_string_field.h"
#include "bool_field.h"
#include "int_field.h"
#include "long_field.h"
#include "float_field.h"
#include "string_field.h"


using namespace vaultdb;


template<typename B>
std::ostream &vaultdb::operator<<(std::ostream &os, const Field<B> &aValue) {
    return os << aValue.toString();
}

template<typename B>
Field<B>::Field(const FieldType &typeId, const int &strLength) : type_(typeId) {
    initialize(typeId, strLength);
    instance_ = FieldFactory<B>::getInstance(this);
}

template<typename B>
Field<B>::Field(const Field<B> &field) : type_(field.type_), allocated_size_(field.allocated_size_) {
    assert(field.type_ != FieldType::INVALID);

    managed_data_ = std::unique_ptr<std::byte[]>(new std::byte[allocated_size_]);
    data_ = managed_data_.get();

    std::memcpy(data_, field.data_, allocated_size_);
    instance_ = FieldFactory<B>::getInstance(this);

}

template<typename B>
Field<B> &Field<B>::operator=(const Field<B> &other) {
    assert(other.type_ != FieldType::INVALID);

    if(&other == this)
        return *this;


    allocated_size_ = other.allocated_size_;
    type_ = other.type_;
    managed_data_ = std::unique_ptr<std::byte[]>(new std::byte[allocated_size_]);
    data_ = this->managed_data_.get();
    instance_ = FieldFactory<B>::getInstance(this);


    memcpy(data_, other.data_, allocated_size_);
    return *this;

}

template<typename B>
FieldType Field<B>::getType() const {
    return type_;
}

template<typename B>
size_t Field<B>::getSize() const {
    return allocated_size_;
}






template<typename B>
B Field<B>::operator==(const Field<B> &cmp) const {
    if(type_ != cmp.type_) return B(false);
    if(allocated_size_ != cmp.allocated_size_) return B(false);

    return *instance_ == cmp;
}

template<typename B>
B Field<B>::operator!=(const Field &cmp) const {
    B eq = *this == cmp;

    return eq.neg();
}

template<typename B>
B Field<B>::operator!() const {
    return !(*instance_);
}

template<typename B>
B Field<B>::operator>=(const Field &rhs) const {
    return  *instance_ >= rhs;
}

template<typename B>
B Field<B>::operator<(const Field &rhs) const {
    return  *instance_ < rhs;
}

template<typename B>
B Field<B>::operator<=(const Field &rhs) const {
    return *instance_ <= rhs;
}

template<typename B>
B Field<B>::operator>(const Field &rhs) const {
    return  *instance_ > rhs;
}


template<typename B>
Field<BoolField>  *Field<B>::reveal(const int &party) const {
    switch (type_) {
        case FieldType::SECURE_BOOL: {
            auto src = getValue<emp::Bit>();
            return new  BoolField(src.reveal(party));
        }
        case FieldType::SECURE_INT: {
            emp::Integer src = getValue<emp::Integer>();
            assert(src.size() == 32);
            return new IntField(src.reveal<int32_t>(party));
        }
        case FieldType::SECURE_LONG: {
            emp::Integer src = getValue<emp::Integer>();
            assert(src.size() == 64);
            return new LongField(src.reveal<int64_t>(party));
        }
        case FieldType::SECURE_FLOAT: {
            emp::Float src = getValue<emp::Float>();
            return new FloatField(src.reveal<double>(party));
        }
        case FieldType::SECURE_STRING: {
            auto src = getValue<emp::Integer>();
            std::string revealed = revealString(src, party);
            return new StringField(revealed);
        }

            // already in the clear, throw so we don't lose the impl object (e.g., IntField)
        case FieldType::BOOL:
        case FieldType::INT:
        case FieldType::LONG:
        case FieldType::FLOAT:
        case FieldType::STRING:
        default:
            throw;

    }
}


template<typename B>
std::string Field<B>::toString() const {
    return instance_->str();
}

template<typename B>
void Field<B>::serialize(int8_t *dst) const {
      instance_->serialize(dst);
}

template<typename B>
Field<B> Field<B>::deserialize(const QueryFieldDesc &fieldDesc, const int8_t *src) {
    return deserialize(fieldDesc.getType(), fieldDesc.getStringLength(), src);
}

template<typename B>
Field<B> Field<B>::deserialize(const FieldType & type, const int & strLength, const int8_t *src) {
    Field f(type, strLength);
    memcpy(f.data_, src, f.allocated_size_);

    return f;
}

template<typename B>
std::string Field<B>::revealString(const emp::Integer &src, const int &party) {
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
    std::string payload = string((char * ) decodedBytesVector.data());
    std::reverse(payload.begin(), payload.end());

    delete[] bools;

    return payload;

}

template<typename B>
Field<SecureBoolField> *Field<B>::secretShare(const Field<BoolField>  *field, const FieldType &type, const size_t &strLength, const int &myParty,
                           const int &dstParty) {
    switch(type) {
        case FieldType::BOOL:
            return  new SecureBoolField (static_cast<const BoolField *>(field), myParty, dstParty);
        case FieldType::INT:
            return   new SecureIntField(static_cast<const IntField *>(field), myParty, dstParty);
        case FieldType::LONG:
            return   new SecureLongField(static_cast<const LongField *>(field), myParty, dstParty);
        case FieldType::FLOAT:
            return  new  SecureFloatField(static_cast<const FloatField *>(field), myParty, dstParty);
        case FieldType::STRING:
            return new SecureStringField(static_cast<const StringField *>(field), strLength, myParty, dstParty);
        default:// may want to throw here
            throw;

    }
}

template<typename B>
void Field<B>::copy(const Field &src) {
    this->type_ = src.type_;
    this->allocated_size_ = src.allocated_size_;

    if(type_ == FieldType::STRING) {
        std::string srcData = src.getStringValue();
        setStringValue(srcData);
    }
    else {
        std::memcpy(data_, src.data_, allocated_size_);
    }
}

template<typename B>
void Field<B>::setStringValue(const string &src) {
    memcpy(data_, src.c_str(), allocated_size_-1);
    *((char *) (data_ + allocated_size_ - 1)) = '\0'; // null-terminate the string
}

template<typename B>
std::string Field<B>::getStringValue() const {
    return std::string((char *) data_);
}


// initialize by type
template<typename B>
void Field<B>::initialize(const FieldType &type, const size_t &strLength) {
    // need strLen to be initialized for string fields
    if(type == FieldType::STRING || type == FieldType::SECURE_STRING) {
        assert(strLength >= 1);
    }

    switch (type) {
        case FieldType::BOOL: {
            allocated_size_ = sizeof(bool);
            managed_data_ = std::unique_ptr<std::byte[]>(reinterpret_cast<std::byte *>(
                                                                 new bool(false)));
            break;
        }
        case FieldType::INT: {
            allocated_size_ = sizeof(int32_t);
            managed_data_ = std::unique_ptr<std::byte[]>(reinterpret_cast<std::byte *>(
                                                                 new int32_t(0)));
            break;
        }
        case FieldType::DATE:
        case FieldType::LONG: {
            allocated_size_ = sizeof(int64_t);
            managed_data_ = std::unique_ptr<std::byte[]>(reinterpret_cast<std::byte *>(
                                                                 new int64_t (0L)));
            break;
        }
        case FieldType::FLOAT: {
            allocated_size_ = sizeof(float_t);
            managed_data_ = std::unique_ptr<std::byte[]>(reinterpret_cast<std::byte *>(
                                                                 new float_t (0.0)));
            break;
        }
        case FieldType::STRING: {
            allocated_size_ = sizeof(char) * (strLength + 1); // +1 for '\0' null-termination
            managed_data_ = std::unique_ptr<std::byte[]>(new std::byte[strLength+1]);
            std::memset(managed_data_.get(), 0, strLength);
            *((char *) managed_data_.get() + strLength - 1) = '\0'; // null-terminated
            break;
        }

        case FieldType::SECURE_BOOL: {
            allocated_size_ = sizeof(emp::Bit(false));
            managed_data_ = std::unique_ptr<std::byte[]>(reinterpret_cast<std::byte *>(
                   new emp::Bit(false)));
         break;
        }
        case FieldType::SECURE_INT:  {
            allocated_size_ = sizeof(emp::Integer(32, 0));
            managed_data_ = std::unique_ptr<std::byte[]>(reinterpret_cast<std::byte *>(
                                                                 new emp::Integer(32, 0)));
            break;
        }

        case FieldType::SECURE_LONG:{
            allocated_size_ = sizeof(emp::Integer(64, 0));
            managed_data_ = std::unique_ptr<std::byte[]>(reinterpret_cast<std::byte *>(
                                                                 new emp::Integer(64, 0)));
            break;
        }
        case FieldType::SECURE_FLOAT: {
            allocated_size_ = sizeof(emp::Float(0.0));
            managed_data_ = std::unique_ptr<std::byte[]>(reinterpret_cast<std::byte *>(new emp::Float(0.0)));
            break;
        }
        case FieldType::SECURE_STRING: {
            allocated_size_ = sizeof(emp::Integer(strLength * 8, 0));
            managed_data_ = std::unique_ptr<std::byte[]>(reinterpret_cast<std::byte *>(
                                                                 new emp::Integer(strLength * 8, 0)));
            break;
        }
        case FieldType::INVALID:
            // do nothing, this is for warnings
            return;
    }// end switch statement

    data_ = managed_data_.get();


}

template<typename B>
void Field<B>::compareAndSwap(const B & choice, Field & lhs, Field & rhs) {
    FieldInstance<B>::compareAndSwap(choice, lhs, rhs);

}



template<typename B>
Field<B> Field<B>::If(const B &choice, const Field &lhs, const Field &rhs) {
    return FieldInstance<B>::If(choice,  lhs, rhs);
}

template<typename B>
Field<B> Field<B>::operator+(const Field &rhs) const {
    return instance_->plus(rhs);
}

template<typename B>
Field<B> Field<B>::operator-(const Field &rhs) const {
    return instance_->minus(rhs);
}


template<typename B>
Field<B> Field<B>::operator*(const Field &rhs) const {
    return instance_->multiply(rhs);
}

template<typename B>
Field<B> Field<B>::operator/(const Field &rhs) const {
    return instance_->div(rhs);
}

template<typename B>
Field<B> Field<B>::operator%(const Field &rhs) const {
    return instance_->modulus(rhs);
}

template class vaultdb::Field<BoolField>;
template class vaultdb::Field<SecureBoolField>;

