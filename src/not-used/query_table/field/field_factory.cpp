#include "field_factory.h"

#include "query_table/field/secure_bool_field.h"
#include "query_table/field/bool_field.h"
#include "query_table/field/secure_int_field.h"
#include "query_table/field/int_field.h"
#include "query_table/field/secure_long_field.h"
#include "query_table/field/long_field.h"
#include "query_table/field/secure_float_field.h"
#include "query_table/field/float_field.h"
#include "query_table/field/secure_string_field.h"
#include "query_table/field/string_field.h"
#include "boost/date_time/gregorian/gregorian.hpp"
#include <util/type_utilities.h>


using namespace vaultdb;

Field *FieldFactory::copyField(const Field *src) {

    switch(src->getType()) {
        case FieldType::INT32:
            return new IntField(*((IntField *)src));
        case FieldType::INT64:
            return new LongField(*((LongField *)src));
        case FieldType::BOOL:
            return new BoolField(*((BoolField *)src));
        case FieldType::FLOAT32:
            return new FloatField(*((FloatField *)src));
        case FieldType::STRING:
            return new StringField(*((StringField *)src));
        case FieldType::SECURE_INT32:
            return new SecureIntField(*((SecureIntField *)src));
        case FieldType::SECURE_INT64:
            return new SecureLongField(*((SecureLongField *)src));
        case FieldType::SECURE_BOOL:
            return new SecureBoolField(*((SecureBoolField *)src));
        case FieldType::SECURE_FLOAT32:
            return new SecureFloatField(*((SecureFloatField *)src));
        case FieldType::SECURE_STRING:
            return new SecureStringField(*((SecureStringField *)src));
        default:
            throw;
    }
}



/* Can't seem to get past it failing on:
 * /src/main/cpp/query_table/field/field_factory.cpp:22:87: error: no matching conversion for C-style cast from 'const bool' to 'std::string' (aka 'basic_string<char, char_traits<char>, allocator<char> >')
    if(std::is_same<decltype(primitive), std::string>::value)  return new StringField((std::string) primitive);
template<typename T>
Field * vaultdb::FieldFactory<T>::getField(const T &primitive, const FieldType & fieldType) {

    if(std::is_same<decltype(primitive), bool>::value) return new BoolField((bool) primitive);
    if(std::is_same<decltype(primitive), int32_t>::value)      return new IntField((int32_t) primitive);
    if(std::is_same<decltype(primitive), int64_t>::value)      return new LongField((int64_t) primitive);
    if(std::is_same<decltype(primitive), float_t>::value)      return new FloatField((float_t) primitive);
    if(std::is_same<decltype(primitive), std::string>::value)  return new StringField((std::string) primitive);
    if(std::is_same<decltype(primitive), emp::Bit>::value)     return new SecureBoolField((emp::Bit) primitive);
    if(std::is_same<decltype(primitive), emp::Float>::value)     return new SecureFloatField((emp::Float) primitive);

    if(std::is_same<decltype(primitive), emp::Integer>::value) {
        switch (fieldType) {
            case FieldType::SECURE_INT32:
                return new SecureIntField((emp::Integer) primitive);
            case FieldType::SECURE_INT64:
                return new SecureLongField((emp::Integer) primitive);
            case FieldType::SECURE_STRING:
                return new SecureStringField((emp::Integer) primitive);
            default:
                throw;
        }
    }


}
 */

Field *FieldFactory::getField(const int8_t *src, const FieldType &fieldType, const size_t &strLength) {

    switch (fieldType) {
        case FieldType::BOOL:
            return new BoolField(src);
        case FieldType::INT32:
            return new IntField(src);
        case FieldType::INT64:
            return new LongField(src);

        case FieldType::FLOAT32:
            return new FloatField(src);

        case FieldType::STRING:
            return new StringField(src, strLength);

        case FieldType::SECURE_BOOL:
            return new SecureBoolField(src);
        case FieldType::SECURE_INT32:
            return new SecureIntField(src);
        case FieldType::SECURE_INT64:
            return new SecureLongField(src);

        case FieldType::SECURE_FLOAT32:
            return new SecureFloatField(src);

        case FieldType::SECURE_STRING:
            return new SecureStringField(src, strLength);


       default:
            return nullptr;

    }


}


Field * FieldFactory::getField(const string &strValue, const FieldType &type, const int & strLength) {


    switch (type) {
        case FieldType::INT32: {
            int32_t intValue = std::atoi(strValue.c_str());
            return new IntField( intValue);
        }
        case FieldType::INT64: {
            int64_t intValue = std::atol(strValue.c_str());
            return new LongField(intValue);
        }
        case FieldType::BOOL: {
            bool boolValue = (strValue == "1") ? true : false;
            return new BoolField(boolValue);
        }
        case FieldType::STRING: {
            std::string fieldStr = strValue;
            while(fieldStr.length() < strLength) {
                fieldStr += " ";
            }
            return new StringField(fieldStr);
        }
        case FieldType::FLOAT32: {
            float_t floatValue = std::atof(strValue.c_str());
            return new FloatField(floatValue);
        }
        case FieldType::DATE: {
            boost::gregorian::date date(boost::gregorian::from_string(strValue));
            boost::gregorian::date epochStart(1970, 1, 1);
            int64_t epochTime = (date - epochStart).days() * 24 * 3600;
            return new LongField(epochTime);
        }
        default:
            throw std::invalid_argument("Unsupported type for string decoding: " + TypeUtilities::getTypeString(type) + "\n");


    };
}
Field * FieldFactory::getZero(const FieldType &aType) {
    switch(aType) {
        case FieldType::BOOL:
            return new BoolField(false);
        case FieldType::INT32:
            return new IntField((int32_t) 0);
        case FieldType::DATE:
        case FieldType::INT64:
            return new LongField((int64_t ) 0);
        case FieldType::FLOAT32:
            return new FloatField((float_t) 0.0);
        case FieldType::STRING:
            char oneChar[2]; //  null-terminated
            oneChar[0]  = (int8_t) 0;
            return new StringField(std::string(oneChar));
        case FieldType::SECURE_BOOL:
            return new SecureBoolField(emp::Bit(0, emp::PUBLIC));
        case FieldType::SECURE_INT32:
            return new SecureIntField(emp::Integer(32, 0, emp::PUBLIC));
        case FieldType::SECURE_INT64:
            return new SecureLongField(emp::Integer(64, 0, emp::PUBLIC));
        case FieldType::SECURE_FLOAT32:
            return new SecureFloatField(emp::Float(0.0));
        case FieldType::SECURE_STRING:
            return new SecureStringField(emp::Integer(8, (int)'0'));
        default:
            throw std::invalid_argument("Type unsupported in getZero(): " + TypeUtilities::getTypeString(aType));

    };
}

Field * FieldFactory::getOne(const FieldType &aType) {
    switch(aType) {
        case FieldType::BOOL:
            return new BoolField(true);
        case FieldType::INT32:
            return new IntField((int32_t) 1);
        case FieldType::DATE:
        case FieldType::INT64:
            return new LongField(1L);
        case FieldType::FLOAT32:
            return new FloatField((float_t) 1.0);
        case FieldType::STRING:
            char oneChar[2]; //  null-terminated
            oneChar[0]  = (int8_t) 1;
            return new StringField(std::string(oneChar));
        case FieldType::SECURE_BOOL:
            return new SecureBoolField(emp::Bit(1, emp::PUBLIC));
        case FieldType::SECURE_INT32:
            return new SecureIntField(emp::Integer(32, 1, emp::PUBLIC));
        case FieldType::SECURE_INT64:
            return new SecureLongField(emp::Integer(64, 1, emp::PUBLIC));
        case FieldType::SECURE_FLOAT32:
            return new SecureFloatField(emp::Float(1.0));
        case FieldType::SECURE_STRING:
            return new SecureStringField(emp::Integer(8, (int)'1'));
        default:
            throw std::invalid_argument("Type unsupported in getOne(): " + TypeUtilities::getTypeString(aType));

    };
}


Field *FieldFactory::obliviousIf(const Field & choice, const Field & lhs,const Field & rhs) {
    return lhs.selectField(choice, rhs);
}

void FieldFactory::compareAndSwap(const Field & choice,  Field & lhs,  Field & rhs) {
    Field *lhsOutput = lhs.selectField(choice, rhs);
    Field *rhsOutput = rhs.selectField(choice, lhs);
    lhs.copyFrom(*lhsOutput);
    rhs.copyFrom(*rhsOutput);

    delete lhsOutput;
    delete rhsOutput;


}

Field *FieldFactory::toFloat(const Field *src) {
    assert(!TypeUtilities::isEncrypted(src->getType()));
    switch(src->getType()) {
        case FieldType::BOOL:
            return new FloatField((float_t) TypeUtilities::getBool(*src));
        case FieldType::INT32:
            return new FloatField((float_t) TypeUtilities::getInt(*src));
        case FieldType::INT64:
            return new FloatField((float_t) TypeUtilities::getLong(*src));
        case FieldType::FLOAT32:
            return FieldFactory::copyField(src);
        default:
            throw std::invalid_argument("Cannot convert value of type " +
                                        TypeUtilities::getTypeString(src->getType()) + " to float.");

    }

}




