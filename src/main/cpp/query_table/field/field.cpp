#include "field.h"
#include "util/utilities.h"
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

#include <util/field_utilities.h>

using namespace vaultdb;


std::ostream &vaultdb::operator<<(std::ostream &os, const Field &aValue) {
    return os << aValue.toString();
}

Field::Field(const FieldType &typeId, const int &strLength) : type_(typeId) {
    initialize(typeId, strLength);
}

Field::Field(const Field &field) : type_(field.type_), allocated_size_(field.allocated_size_) {
    assert(field.type_ != FieldType::INVALID);

    managed_data_ = std::unique_ptr<std::byte[]>(new std::byte[allocated_size_]);
    data_ = managed_data_.get();

    std::memcpy(data_, field.data_, allocated_size_);
}

Field &Field::operator=(const Field &other) {
    assert(other.type_ != FieldType::INVALID);

    if(&other == this)
        return *this;


    allocated_size_ = other.allocated_size_;
    type_ = other.type_;
    managed_data_ = std::unique_ptr<std::byte[]>(new std::byte[allocated_size_]);
    data_ = this->managed_data_.get();

    memcpy(data_, other.data_, allocated_size_);
    return *this;

}

FieldType Field::getType() const {
    return type_;
}

size_t Field::getSize() const {
    return allocated_size_;
}







bool Field::operator==(const Field &cmp) const {
    if(type_ != cmp.type_) return false;
    if(allocated_size_ != cmp.allocated_size_) return false;

    // this still works for string because we are storing the character array
    // might not work for emp types
    for (size_t i = 0; i < allocated_size_; ++i) {
        if (data_[i] != cmp.data_[i])
                return false;
    }
    return true;
}

bool Field::operator!=(const Field &cmp) const {
    return !(*this == cmp);
}


void Field::compareAndSwap(const bool &choice, Field &lhs, Field &rhs) {
    if(choice) {
        Field tmp = lhs;
        lhs = rhs;
        rhs = tmp;
    }
}

void Field::compareAndSwap(const emp::Bit &choice, const Field &lhs, const Field &rhs) {
    switch(lhs.type_) {
        case FieldType::SECURE_BOOL:
            emp::swap(choice, reinterpret_cast<emp::Bit &>(*lhs.data_), reinterpret_cast<emp::Bit&>(*rhs.data_) );
            return;
        case FieldType::SECURE_INT:
        case FieldType::SECURE_LONG:
        case FieldType::SECURE_STRING:
            emp::swap(choice, reinterpret_cast<emp::Integer &>(*lhs.data_), reinterpret_cast<emp::Integer&>(*rhs.data_) );
            return;
        case FieldType::SECURE_FLOAT:
            emp::swap(choice, reinterpret_cast<emp::Float &>(*lhs.data_), reinterpret_cast<emp::Float &>(*rhs.data_) );
            return;
        default:
            throw;

    }


}

Field * Field::reveal(const int &party) const {
    switch (type_) {
        case FieldType::SECURE_BOOL: {
            auto src = getValue<emp::Bit>();
            return new BoolField(src.reveal(party));
        }
        case FieldType::SECURE_INT: {
            auto src = getValue<emp::Integer>();
            assert(src.size() == 32);
            return new IntField(src.reveal<int32_t>(party));
        }
        case FieldType::SECURE_LONG: {
            auto src = getValue<emp::Integer>();
            assert(src.size() == 64);
            return new LongField(src.reveal<int64_t>(party));
        }
        case FieldType::SECURE_FLOAT: {
            auto src = getValue<emp::Float>();
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

std::string Field::toString() const {
    switch(type_) {
        case FieldType::BOOL: {
            bool res = getValue<bool>();
            return res ? "true" : "false";
        }
        case FieldType::INT: {
            auto res = getValue<int32_t>();
            return std::to_string(res);
        }
        case FieldType::LONG: {
            auto res = getValue<int64_t>();
            return std::to_string(res);
        }
        case FieldType::FLOAT: {
            auto res = getValue<float_t>();
            return std::to_string(res);
        }
        case FieldType::STRING: {
            return std::string((char *) data_);
        }
        case FieldType::SECURE_BOOL:
            return "SECRET BIT";
        case FieldType::SECURE_INT:
            return "SECRET INT32";
        case FieldType::SECURE_LONG:
            return "SECURE INT64";
        case FieldType::SECURE_FLOAT:
            return "SECURE FLOAT";
        case FieldType::SECURE_STRING:
            return "SECURE STRING";
        default:
            throw;
    }
}

void Field::serialize(int8_t *dst) const {
        memcpy(dst, data_, allocated_size_);
}

Field Field::deserialize(const QueryFieldDesc &fieldDesc, const int8_t *src) {
    return deserialize(fieldDesc.getType(), fieldDesc.getStringLength(), src);
}

Field Field::deserialize(const FieldType & type, const int & strLength, const int8_t *src) {
    Field f(type, strLength);
    memcpy(f.data_, src, f.allocated_size_);

    return f;
}

std::string Field::revealString(const emp::Integer &src, const int &party) {
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

Field * Field::secretShare(const Field *field, const FieldType &type, const size_t &strLength, const int &myParty,
                           const int &dstParty) {
    switch(type) {
        case FieldType::BOOL:
            return new SecureBoolField (field, myParty, dstParty);
        case FieldType::INT:
            return new  SecureIntField(field, myParty, dstParty);
        case FieldType::LONG:
            return new  SecureLongField(field, myParty, dstParty);
        case FieldType::FLOAT:
            return new  SecureFloatField(field, myParty, dstParty);
        case FieldType::STRING:
            return new SecureStringField(field, strLength, myParty, dstParty);
        default:// may want to throw here
            return new Field(*field);

    }
}

void Field::copy(const Field &src) {
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

void Field::setStringValue(const string &src) {
    memcpy(data_, src.c_str(), allocated_size_-1);
    *((char *) (data_ + allocated_size_ - 1)) = '\0'; // null-terminate the string
}

std::string Field::getStringValue() const {
    return std::string((char *) data_);
}


// initialize by type
void Field::initialize(const FieldType &type, const size_t &strLength) {
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
        }
        case FieldType::DATE:
            throw; // we should never get here, date is handled in PsqlDataProvider
        case FieldType::INVALID:
            // do nothing, this is for warnings
            return;
    }// end switch statement

    data_ = managed_data_.get();


}

