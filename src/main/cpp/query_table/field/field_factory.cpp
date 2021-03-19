#include "field_factory.h"


#include "boost/date_time/gregorian/gregorian.hpp"
#include <util/type_utilities.h>

#include "instance/bool_instance.h"
#include "instance/int_instance.h"
#include "instance/secure_int_instance.h"

using namespace vaultdb;

// strings must be padded to max field length before calling this
// In other words: field length == string length

Field<BoolField> *FieldFactory<BoolField>::deepCopy(const Field<BoolField> *srcField) {
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
        case FieldType::INVALID:
            return new Field<BoolField>();
        default: // invalid
            throw;
    }
}

Field<BoolField> * FieldFactory<BoolField>::getFieldFromString(const FieldType &type, const size_t &strLength, const std::string &src) {
    switch (type) {
        case FieldType::BOOL: {
            bool boolField = (src == "1") ? true : false;
            return new BoolField(boolField);
        }

        case FieldType::INT: {
            int32_t intField = std::atoi(src.c_str());
            return new IntField(intField);
        }
        case FieldType::LONG: {
            int64_t intField = std::atol(src.c_str());
            return new LongField(intField);
        }
        case FieldType::STRING: {
            std::string fieldStr = src;
            while(fieldStr.length() < strLength) {
                fieldStr += " ";
            }
            return new StringField(fieldStr);
        }
        case FieldType::FLOAT: {
            float_t floatField = std::atof(src.c_str());
            return new FloatField(floatField);
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



Field<BoolField> FieldFactory<BoolField>::getZero(const FieldType &aType) {
    switch(aType) {
        case FieldType::BOOL:
            return  BoolField(false);
        case FieldType::INT:
            return  IntField((int32_t) 0);
        case FieldType::DATE:
        case FieldType::LONG:
            return  LongField((int64_t ) 0);
        case FieldType::FLOAT:
            return  FloatField((float_t) 0.0);
        case FieldType::STRING:
            char oneChar[2]; //  null-terminated
            oneChar[0]  = (int8_t) 0;
            return  StringField(std::string(oneChar));
        default:
            throw std::invalid_argument("Type unsupported in getZero(): " + TypeUtilities::getTypeString(aType));

    };
}


Field<BoolField> FieldFactory<BoolField>::getOne(const FieldType &aType) {
    switch(aType) {
        case FieldType::BOOL:
            return  BoolField(true);
        case FieldType::INT:
            return  IntField((int32_t) 1);
        case FieldType::DATE:
        case FieldType::LONG:
            return  LongField(1L);
        case FieldType::FLOAT:
            return  FloatField((float_t) 1.0);
        case FieldType::STRING:
            char oneChar[2]; //  null-terminated
            oneChar[0]  = (int8_t) 1;
            return  StringField(std::string(oneChar));
        default:
            throw std::invalid_argument("Type unsupported in getOne(): " + TypeUtilities::getTypeString(aType));

    };
}



FieldInstance<BoolField> *FieldFactory<BoolField>::getFieldInstance(Field<BoolField> *src) {
    FieldType aType = src->getType();

    switch(aType) {
        case FieldType::BOOL:
            return new BoolInstance(src);
        case FieldType::INT:
            return  new IntInstance(src);
            /*case FieldType::DATE:
            case FieldType::LONG:
                return  LongField(1L);
            case FieldType::FLOAT:
                return  FloatField((float_t) 1.0);
            case FieldType::STRING:
                char oneChar[2]; //  null-terminated
                oneChar[0]  = (int8_t) 1; */
        default:
            throw std::invalid_argument("Type unsupported in getInstance(): " + TypeUtilities::getTypeString(aType));

    }
}

FloatField FieldFactory<BoolField>::toFloat(const Field<BoolField> *src) {
    switch(src->getType()) {
        case FieldType::BOOL:
            return FloatField((float_t) ((BoolField *) src)->getPayload());
        case FieldType::INT:
            return FloatField((float_t) ((IntField *) src)->getPayload());
        case FieldType::LONG:
            return FloatField((float_t) ((LongField *) src)->getPayload());
        case FieldType::FLOAT:
            return FloatField(((FloatField *) src)->getPayload());

        default:
            throw std::invalid_argument("Cannot convert value of type " +
                                        TypeUtilities::getTypeString(src->getType()) + " to float.");
    }

}

// ************  Start SecureBoolField *************/
Field<SecureBoolField> *FieldFactory<SecureBoolField>::deepCopy(const Field<SecureBoolField> *srcField) {
    switch(srcField->getType()) {
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
            return new Field<SecureBoolField>();
        default: // invalid
            throw;
    }
}




Field<SecureBoolField> FieldFactory<SecureBoolField>::getZero(const FieldType &aType) {
    switch(aType) {
        case FieldType::SECURE_BOOL:
            return  SecureBoolField(emp::Bit(0, emp::PUBLIC));
        case FieldType::SECURE_INT:
            return  SecureIntField(emp::Integer(32, 0, emp::PUBLIC));
        case FieldType::SECURE_LONG:
            return  SecureLongField(emp::Integer(64, 0, emp::PUBLIC));
        case FieldType::SECURE_FLOAT:
            return  SecureFloatField(emp::Float(0.0));
        case FieldType::SECURE_STRING:
            return  SecureStringField(emp::Integer(8, (int)'0'));
        default:
            throw std::invalid_argument("Type unsupported in getZero(): " + TypeUtilities::getTypeString(aType));

    };
}

Field<SecureBoolField> FieldFactory<SecureBoolField>::getOne(const FieldType &aType) {
    switch(aType) {
        case FieldType::SECURE_BOOL:
            return  SecureBoolField(emp::Bit(1, emp::PUBLIC));
        case FieldType::SECURE_INT:
            return  SecureIntField(emp::Integer(32, 1, emp::PUBLIC));
        case FieldType::SECURE_LONG:
            return  SecureLongField(emp::Integer(64, 1, emp::PUBLIC));
        case FieldType::SECURE_FLOAT:
            return  SecureFloatField(emp::Float(1.0));
        case FieldType::SECURE_STRING:
            return  SecureStringField(emp::Integer(8, (int)'1'));
        default:
            throw std::invalid_argument("Type unsupported in getOne(): " + TypeUtilities::getTypeString(aType));

    };
}

FieldInstance<SecureBoolField> *FieldFactory<SecureBoolField>::getFieldInstance( Field<SecureBoolField> *src) {
    FieldType aType = src->getType();

    switch(aType) {
/*        case FieldType::SECURE_BOOL:
            return  SecureBoolField(emp::Bit(1, emp::PUBLIC)); */
        case FieldType::SECURE_INT:
            return new SecureIntInstance(src);
/*        case FieldType::SECURE_LONG:
            return  SecureLongField(emp::Integer(64, 1, emp::PUBLIC));
        case FieldType::SECURE_FLOAT:
            return  SecureFloatField(emp::Float(1.0));
        case FieldType::SECURE_STRING:
            return  SecureStringField(emp::Integer(8, (int)'1'));*/
        default:
            throw std::invalid_argument("Type unsupported in getInstance(): " + TypeUtilities::getTypeString(aType));

    }
}

Field<SecureBoolField> FieldFactory<SecureBoolField>::toFloat(const Field<SecureBoolField> *src) {

    switch (src->getType()) {
        case FieldType::SECURE_INT:
            return SecureFloatField::toFloat(*((SecureIntField *) src));
        case FieldType::SECURE_LONG:
            return SecureFloatField::toFloat(*((SecureLongField *) src));
        default:
            throw std::invalid_argument("Cannot convert value of type " +
                                        TypeUtilities::getTypeString(src->getType()) + " to emp::Float.");


    }
}



