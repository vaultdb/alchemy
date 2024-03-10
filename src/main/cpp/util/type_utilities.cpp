#include "type_utilities.h"

template<>
FieldType TypeUtilities::getBoolType<bool>() {
    return FieldType::BOOL;
}

template<>
FieldType TypeUtilities::getBoolType<emp::Bit>() {
    return FieldType::SECURE_BOOL;
}

