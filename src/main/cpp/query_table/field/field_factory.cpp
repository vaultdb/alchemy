#include "field_factory.h"

#include "query_table/field/secure_bool_field.h"
#include "query_table/field/bool_field.h"
/*
#include "query_table/field/secure_int_field.h"
#include "query_table/field/int_field.h"
#include "query_table/field/secure_long_field.h"
#include "query_table/field/long_field.h"
#include "query_table/field/secure_float_field.h"
#include "query_table/field/float_field.h"
#include "query_table/field/secure_string_field.h"
#include "query_table/field/string_field.h"*/
#include "boost/date_time/gregorian/gregorian.hpp"
#include <util/type_utilities.h>


using namespace vaultdb;


Field FieldFactory::getFieldFromString(const FieldType &type, const size_t &strLength, const std::string &src) {
    switch (type) {
        case FieldType::BOOL: {
            bool boolValue = (src == "1") ? true : false;
            return Field::createBool(boolValue);
        }

        case FieldType::INT32: {
            int32_t intValue = std::atoi(src.c_str());
            return Field::createInt32( intValue);
        }
        case FieldType::INT64: {
            int64_t intValue = std::atol(src.c_str());
            return Field::createInt64(intValue);
        }
        case FieldType::STRING: {
            std::string fieldStr = src;
            while(fieldStr.length() < strLength) {
                fieldStr += " ";
            }
            return Field::createString(fieldStr);
        }
        case FieldType::FLOAT32: {
            float_t floatValue = std::atof(src.c_str());
            return Field::createFloat32(floatValue);
        }
        case FieldType::DATE: {
            boost::gregorian::date date(boost::gregorian::from_string(src));
            boost::gregorian::date epochStart(1970, 1, 1);
            int64_t epochTime = (date - epochStart).days() * 24 * 3600;
            return Field::createInt64(epochTime);
        }
        default:
            throw std::invalid_argument("Unsupported type for string decoding: " + TypeUtilities::getTypeString(type) + "\n");


    };
}




