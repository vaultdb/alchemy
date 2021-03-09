#include "field.h"
#include "util/utilities.h"

using namespace vaultdb;

std::ostream &vaultdb::operator<<(std::ostream &os, const Field &aValue) {
    return os << aValue.toString();
}

Field::Field(const FieldType &typeId, const int &strLength) : type_(typeId) {

    allocated_size_ = FieldUtils::getPhysicalSize(typeId);

    // need strLen to be initialized for string fields
    if(typeId == FieldType::STRING || typeId == FieldType::SECURE_STRING) {
        assert(strLength >= 1);
        allocated_size_ *= strLength;
    }


    managed_data_ = std::unique_ptr<std::byte[]>(new std::byte[allocated_size_]);
    data_ = managed_data_.get();
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

template<typename T>
T Field::getValue() const {
    return *(reinterpret_cast<T*>(data_));
}

std::string Field::getValue() const {
    if(type_ == FieldType::STRING) {
        return std::string((char *) data_);
    }
    else 
        return toString();
}

template<typename T>
void Field::setValue(const T &src) {
    *(reinterpret_cast<T*>(data_)) = src;
}

void Field::setValue(const string &src) {

    assert(type_ == FieldType::STRING);
    memcpy(data_, src.c_str(), allocated_size_);

}



std::byte *Field::getData() const { return data_; }

bool Field::operator==(const Field &cmp) const {
    if(type_ != cmp.type_) return false;
    if(allocated_size_ != cmp.allocated_size_) return false;

    if(type_ != FieldType::STRING) {
        for (size_t i = 0; i < allocated_size_; ++i) {
            if (data_[i] != cmp.data_[i])
                return false;
        }
    }
    else {
        string lhs = this->getValue();
        string rhs = cmp.getValue();
        return lhs == rhs;
    }
    return true;
}

bool Field::operator!=(const Field &cmp) const {
    return !(*this == cmp);
}

Field Field::createBool(const bool &value) {
    Field f(FieldType::BOOL);
    f.setValue<bool>(value);
    return f;
}

Field Field::createInt32(const int32_t &value) {
    Field f(FieldType::INT32);
    f.setValue<int32_t>(value);
    return f;
}

Field Field::createInt64(const int64_t &value) {
    Field f(FieldType::INT64);
    f.setValue<int64_t>(value);
    return f;
}

Field Field::createFloat32(const float_t &value) {
    Field f(FieldType::FLOAT32);
    f.setValue<float_t>(value);
    return f;
}

Field Field::createString(const string &value) {
    Field f(FieldType::STRING, value.length());
    f.setValue(value);
    return f;
}

Field Field::createSecureBool(const emp::Bit &value) {
    Field f(FieldType::SECURE_BOOL);
    f.setValue<emp::Bit>(value);
    return f;
}

Field Field::createSecureFloat(const emp::Float &value) {
    Field f(FieldType::SECURE_FLOAT32);
    f.setValue<emp::Float>(value);
    return f;
}

Field Field::createSecureInt32(const emp::Integer &value) {
    assert(value.bits.size() == 32);

    Field f(FieldType::SECURE_INT32);
    f.setValue<emp::Integer>(value);
    return f;
}

Field Field::createSecureInt64(const emp::Integer &value) {
    assert(value.bits.size() == 64);

    Field f(FieldType::SECURE_INT64);
    f.setValue<emp::Integer>(value);
    return f;
}

Field Field::createSecureString(const emp::Integer &value) {
    Field f(FieldType::SECURE_STRING);
    f.setValue<emp::Integer>(value);
    return f;
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
        case FieldType::SECURE_INT32:
        case FieldType::SECURE_INT64:
        case FieldType::SECURE_STRING:
            emp::swap(choice, reinterpret_cast<emp::Integer &>(*lhs.data_), reinterpret_cast<emp::Integer&>(*rhs.data_) );
            return;
        case FieldType::SECURE_FLOAT32:
            emp::swap(choice, reinterpret_cast<emp::Float &>(*lhs.data_), reinterpret_cast<emp::Float &>(*rhs.data_) );
            return;
        default:
            throw;

    }


}

// need to call getValue<T> on each type we use at least once or else it doesn't show up in symbol table
Field Field::reveal(const int &party) const {
    switch (type_) {
        case FieldType::SECURE_BOOL: {
            emp::Bit src = getValue<emp::Bit>();
            Field ret(FieldType::BOOL);
            ret.setValue<bool>(src.reveal(party));
            return ret;
        }
        case FieldType::SECURE_INT32: {
            emp::Integer src = getValue<emp::Integer>();
            assert(src.size() == 32);

            Field ret(FieldType::INT32);
            ret.setValue<int32_t>(src.reveal<int32_t>(party));
            return ret;
        }
        case FieldType::SECURE_INT64: {
            emp::Integer src = getValue<emp::Integer>();
            assert(src.size() == 64);
            Field ret(FieldType::INT64);
            ret.setValue<int64_t>(src.reveal<int64_t>(party));
            return ret;
        }
        case FieldType::SECURE_FLOAT32: {
            emp::Float src = getValue<emp::Float>();
            Field ret(FieldType::FLOAT32);
            ret.setValue<float_t>(src.reveal<double>(party));
            return ret;
        }
        case FieldType::SECURE_STRING: {
            emp::Integer src = getValue<emp::Integer>();
            std::string revealed = revealString(src, party);
            Field ret = createString(revealed);
            return ret;
        }

            // already in the clear
        case FieldType::BOOL:
        case FieldType::INT32:
        case FieldType::INT64:
        case FieldType::FLOAT32:
        case FieldType::STRING:
            return Field(*this);
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
        case FieldType::INT32: {
            int32_t res = getValue<int32_t>();
            return std::to_string(res);
        }
        case FieldType::INT64: {
            int64_t res = getValue<int64_t>();
            return std::to_string(res);
        }
        case FieldType::FLOAT32: {
            float_t res = getValue<float_t>();
            return std::to_string(res);
        }
        case FieldType::STRING: {
            return getValue();
        }
        case FieldType::SECURE_BOOL:
            return "SECRET BIT";
        case FieldType::SECURE_INT32:
            return "SECRET INT32";
        case FieldType::SECURE_INT64:
            return "SECURE INT64";
        case FieldType::SECURE_FLOAT32:
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
