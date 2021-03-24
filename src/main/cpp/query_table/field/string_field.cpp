#include "string_field.h"
#include "util/utilities.h"
using namespace vaultdb;

std::ostream &operator<<(std::ostream &os, const StringField &aValue) {
    return os << aValue.toString();
}


// use default to automatically copy out string length
StringField::StringField(const Field &srcField) : Field(srcField) { }

StringField::StringField(const StringField &src) : Field(src) {
}

StringField::StringField(const std::string &src) :  Field(FieldType::STRING, src.length()){
    setStringValue(src);
}


StringField::StringField(const int8_t *src, const size_t & strLength) : Field(FieldType::STRING, strLength) {
    std::string input((char *) src, strLength);
    std::reverse(input.begin(), input.end()); // serialize in reverse for EMP format

    memcpy(data_, input.c_str(), strLength);
    *((char *) (data_ + strLength)) = '\0'; // null-terminate the string

}


// decrypt
StringField::StringField(const emp::Integer &src, const int &party) : Field(FieldType::STRING, src.size()/8) {
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

    setStringValue(payload);
    delete[] bools;
}

StringField &StringField::operator=(const StringField &other) {
    if(this == &other) return *this;
    copy(other);
    return *this;
}

BoolField StringField::operator>=(const StringField &cmp) const {
    bool res =  getPayload() >= cmp.getPayload();
    return BoolField(res);
}

BoolField StringField::operator==(const StringField &cmp) const {
    //std::cout << "Equality check: " << getPayload() << "==" << cmp.getPayload() << std::endl;
    bool res = (getPayload()) == (cmp.getPayload());
    return  BoolField(res);
}

StringField StringField::selectValue(const BoolField &choice, const StringField &other) const {
    bool selection =  choice.getPayload();
    return selection ? StringField(*this) :  StringField(other);
}

void StringField::ser(int8_t *target) const {
    std::string p = getPayload();
    std::reverse(p.begin(), p.end()); // reverse it so we can more easily conver to EMP format
    memcpy(target, (int8_t *) p.c_str(), allocated_size_ - 1); // null termination chopped


}

