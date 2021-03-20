#include "field_factory.h"

#include "boost/date_time/gregorian/gregorian.hpp"
#include <util/type_utilities.h>
#include <field/instance/long_instance.h>
#include <field/instance/float_instance.h>
#include <field/instance/string_instance.h>
#include <field/instance/secure_bool_instance.h>
#include <field/instance/secure_long_instance.h>
#include <field/instance/secure_float_instance.h>
#include <field/instance/secure_string_instance.h>

#include "instance/bool_instance.h"
#include "instance/int_instance.h"
#include "instance/secure_int_instance.h"

using namespace vaultdb;

PlainField * FieldFactory<BoolField>::getFieldFromString(const FieldType &type, const size_t &strLength, const std::string &src) {
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



PlainField FieldFactory<BoolField>::getZero(const FieldType &aType) {
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


PlainField FieldFactory<BoolField>::getOne(const FieldType &aType) {
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



FieldInstance<BoolField> *FieldFactory<BoolField>::getFieldInstance(PlainField *src) {
    FieldType aType = src->getType();

    switch(aType) {
        case FieldType::BOOL:
            return new BoolInstance(src);
        case FieldType::INT:
            return new IntInstance(src);
        case FieldType::DATE:
        case FieldType::LONG:
            return new LongInstance(src);
        case FieldType::FLOAT:
            return new FloatInstance(src);
        case FieldType::STRING:
            return new StringInstance(src);
        default:
            throw std::invalid_argument("Type unsupported in getInstance(): " + TypeUtilities::getTypeString(aType));

    }
}

FloatField FieldFactory<BoolField>::toFloat(const PlainField &src) {
    switch(src.getType()) {
        case FieldType::BOOL:
            return FloatField((float_t) ((BoolField) src).getPayload());
        case FieldType::INT:
            return FloatField((float_t) ((IntField) src).getPayload());
        case FieldType::LONG:
            return FloatField((float_t) ((LongField) src).getPayload());
        case FieldType::FLOAT:
            return FloatField(((FloatField) src).getPayload());

        default:
            throw std::invalid_argument("Cannot convert value of type " +
                                        TypeUtilities::getTypeString(src.getType()) + " to float.");
    }

}

PlainField FieldFactory<BoolField>::getMin(const FieldType & type) {
    switch(type) {
        case FieldType::BOOL:
            return BoolField(false);
        case FieldType::INT:
            return IntField(INT_MIN);
        case FieldType::LONG:
            return  LongField(LONG_MIN);
        case FieldType::FLOAT:
            return FloatField(FLT_MIN);
        default:
            throw std::invalid_argument("Type " + TypeUtilities::getTypeString(type) + " not supported by FieldFactory<BoolField>::getMin()");
    }

}

PlainField FieldFactory<BoolField>::getMax(const FieldType & type) {
    switch(type) {
        case FieldType::BOOL:
            return BoolField(true);
        case FieldType::INT:
            return IntField(INT_MAX);
        case FieldType::LONG:
            return  LongField(LONG_MAX);
        case FieldType::FLOAT:
            return FloatField(FLT_MAX);
        default:
            throw std::invalid_argument("Type " + TypeUtilities::getTypeString(type) + " not supported by FieldFactory<BoolField>::getMax()");
    }


}


// ************  Start SecureBoolField *************/



SecureField FieldFactory<SecureBoolField>::getZero(const FieldType &aType) {
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

SecureField FieldFactory<SecureBoolField>::getOne(const FieldType &aType) {
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

FieldInstance<SecureBoolField> *FieldFactory<SecureBoolField>::getFieldInstance( SecureField *src) {
    FieldType aType = src->getType();

    switch(aType) {
        case FieldType::SECURE_BOOL:
            return new SecureBoolInstance(src);
        case FieldType::SECURE_INT:
            return new SecureIntInstance(src);
       case FieldType::SECURE_LONG:
           return new SecureLongInstance(src);
        case FieldType::SECURE_FLOAT:
            return new SecureFloatInstance(src);
        case FieldType::SECURE_STRING:
            return new SecureStringInstance(src);
        default:
            throw std::invalid_argument("Type unsupported in getInstance(): " + TypeUtilities::getTypeString(aType));

    }
}

SecureField FieldFactory<SecureBoolField>::toFloat(const SecureField &src) {

    switch (src.getType()) {
        case FieldType::SECURE_INT:
            return SecureFloatField::toFloat((SecureIntField) src);
        case FieldType::SECURE_LONG:
            return SecureFloatField::toFloat((SecureLongField) src);
        case FieldType::SECURE_FLOAT:
            return SecureFloatField((SecureFloatField) src);

        default:
            throw std::invalid_argument("Cannot convert value of type " +
                                        TypeUtilities::getTypeString(src.getType()) + " to SecureFloat.");


    }
}


SecureField FieldFactory<SecureBoolField>::getMin(const FieldType & type) {
    switch(type) {
        case FieldType::SECURE_BOOL:
            return SecureBoolField(false);
        case FieldType::SECURE_INT:
            return SecureIntField(INT_MIN);
        case FieldType::SECURE_LONG:
            return  SecureLongField(LONG_MIN);
        case FieldType::SECURE_FLOAT:
            return SecureFloatField(FLT_MIN);
        default:
            throw std::invalid_argument("Type " + TypeUtilities::getTypeString(type) + " not supported by FieldFactory<SecureBoolField>::getMin()");
    }

}


SecureField FieldFactory<SecureBoolField>::getMax(const FieldType & type) {
    switch(type) {
        case FieldType::SECURE_BOOL:
            return SecureBoolField(true);
        case FieldType::SECURE_INT:
            return SecureIntField(INT_MAX);
        case FieldType::SECURE_LONG:
            return  SecureLongField(LONG_MAX);
        case FieldType::SECURE_FLOAT:
            return SecureFloatField(FLT_MAX);
        default:
            throw std::invalid_argument("Type " + TypeUtilities::getTypeString(type) + " not supported by FieldFactory<SecureBoolField>::getMax()");
    }


}

