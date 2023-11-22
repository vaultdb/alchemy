#include "field.h"

using namespace vaultdb;
#if __has_include("emp-sh2pc/emp-sh2pc.h")
static EmpMode _emp_mode_ = EmpMode::SH2PC;
template<typename B>
void Field<B>::serialize(int8_t *dst, const QueryFieldDesc &schema) const {
    assert(dst != nullptr);

    string s;
    emp::Integer si;
    emp::Float sf;
    emp::Bit sb;

    switch (type_) {
        case FieldType::BOOL:
            *((bool *) dst) = getValue<bool>();
            break;
        case FieldType::INT:
            *((int32_t *) dst) = getValue<int32_t>();
            break;
        case FieldType::LONG:
            *((int64_t *) dst) = getValue<int64_t>();
            break;
        case FieldType::FLOAT:
            *((float_t *) dst) = getValue<float_t>();
            break;
        case FieldType::STRING:
            s = getValue<string>();
            std::reverse(s.begin(), s.end()); // reverse it so we can more easily convert to EMP format
            memcpy(dst, (int8_t *) s.c_str(), s.size()); // null termination chopped
            break;

        case FieldType::SECURE_BOOL:
            sb = boost::get<emp::Bit>(payload_);
            memcpy(dst, (int8_t *) &(sb.bit), TypeUtilities::getEmpBitSize());
            break;
        case FieldType::SECURE_INT:
        case FieldType::SECURE_LONG:
        case FieldType::SECURE_STRING:
            si = boost::get<emp::Integer>(payload_);
            if(schema.getFieldMin() != 0) {
                si = si - schema.getSecureFieldMin();
            }
            memcpy(dst, (int8_t *) si.bits.data(), schema.size() * TypeUtilities::getEmpBitSize());
            break;
        case FieldType::SECURE_FLOAT:
            sf = boost::get<emp::Float>(payload_);
            memcpy(dst, (int8_t *) sf.value.data(), 32* TypeUtilities::getEmpBitSize());
            break;
        default:
            throw;
    }
}

// these methods are only needed for RESCU/outsourced MPC
template<typename B>
void Field<B>::serializeSecureString(const Field<B> &src, int8_t *dst) {
    throw;
}

template<typename B>
Field<B> Field<B>::deserializeSecureString(OMPCPackedWire *src, const int & str_length) {
    throw;
}


#endif
