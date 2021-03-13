#include "field_factory.h"


#include "boost/date_time/gregorian/gregorian.hpp"
#include <util/type_utilities.h>


using namespace vaultdb;


Field * FieldFactory::getFieldFromString(const FieldType &type, const size_t &strLength, const std::string &src) {
    switch (type) {
        case FieldType::BOOL: {
            bool boolValue = (src == "1") ? true : false;
            return new BoolField(boolValue);
        }

        case FieldType::INT: {
            int32_t intValue = std::atoi(src.c_str());
            return new IntField(intValue);
        }
        case FieldType::LONG: {
            int64_t intValue = std::atol(src.c_str());
            return new LongField(intValue);
        }
        case FieldType::STRING: {
            std::string fieldStr = src;
            while(fieldStr.length() < strLength) {
                fieldStr += " ";
            }
            return new StringField(fieldStr);
        }
        case FieldType::FLOAT: {
            float_t floatValue = std::atof(src.c_str());
            return new FloatField(floatValue);
        }
        case FieldType::DATE: {
            boost::gregorian::date date(boost::gregorian::from_string(src));
            boost::gregorian::date epochStart(1970, 1, 1);
            int64_t epochTime = (date - epochStart).days() * 24 * 3600;
            return new LongField(epochTime);
        }
        default:
            throw std::invalid_argument("Unsupported type for string decoding: " + TypeUtilities::getTypeString(type) + "\n");


    };
}


// strings must be padded to max field length before calling this
// In other words: field length == string length

Field *FieldFactory::deepCopy(const Field *srcField) {
    switch(srcField->getType()) {
        case FieldType::BOOL:
            return new BoolField(*srcField);
        case FieldType::INT:
            return new IntField(*srcField);
        case FieldType::LONG:
            return new LongField(*srcField);
        case FieldType::FLOAT:
            return new FloatField(*srcField);
        case FieldType::STRING:
            return new StringField(*srcField);
        case FieldType::SECURE_BOOL:
            return new SecureBoolField(*srcField);
        case FieldType::SECURE_INT:
            return new SecureIntField(*srcField);
        case FieldType::SECURE_LONG:
            return new SecureLongField(*srcField);
        case FieldType::SECURE_FLOAT:
            return new SecureFloatField(*srcField);
        case FieldType::SECURE_STRING:
            return new SecureStringField(*srcField);
        case FieldType::INVALID:
            return new Field();
        default: // invalid
            throw;
    }
}


Field * FieldFactory::getZero(const FieldType &aType) {
    switch(aType) {
        case FieldType::BOOL:
            return new BoolField(false);
        case FieldType::INT:
            return new IntField((int32_t) 0);
        case FieldType::DATE:
        case FieldType::LONG:
            return new LongField((int64_t ) 0);
        case FieldType::FLOAT:
            return new FloatField((float_t) 0.0);
        case FieldType::STRING:
            char oneChar[2]; //  null-terminated
            oneChar[0]  = (int8_t) 0;
            return new StringField(std::string(oneChar));
        case FieldType::SECURE_BOOL:
            return new SecureBoolField(emp::Bit(0, emp::PUBLIC));
        case FieldType::SECURE_INT:
            return new SecureIntField(emp::Integer(32, 0, emp::PUBLIC));
        case FieldType::SECURE_LONG:
            return new SecureLongField(emp::Integer(64, 0, emp::PUBLIC));
        case FieldType::SECURE_FLOAT:
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
        case FieldType::INT:
            return new IntField((int32_t) 1);
        case FieldType::DATE:
        case FieldType::LONG:
            return new LongField(1L);
        case FieldType::FLOAT:
            return new FloatField((float_t) 1.0);
        case FieldType::STRING:
            char oneChar[2]; //  null-terminated
            oneChar[0]  = (int8_t) 1;
            return new StringField(std::string(oneChar));
        case FieldType::SECURE_BOOL:
            return new SecureBoolField(emp::Bit(1, emp::PUBLIC));
        case FieldType::SECURE_INT:
            return new SecureIntField(emp::Integer(32, 1, emp::PUBLIC));
        case FieldType::SECURE_LONG:
            return new SecureLongField(emp::Integer(64, 1, emp::PUBLIC));
        case FieldType::SECURE_FLOAT:
            return new SecureFloatField(emp::Float(1.0));
        case FieldType::SECURE_STRING:
            return new SecureStringField(emp::Integer(8, (int)'1'));
        default:
            throw std::invalid_argument("Type unsupported in getOne(): " + TypeUtilities::getTypeString(aType));

    };
}



