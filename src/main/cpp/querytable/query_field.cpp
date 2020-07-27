#include "query_field.h"
using namespace vaultdb;

QueryField::QueryField(const QueryField &qf)
    : ordinal(qf.ordinal), value_(qf.value_) {}

QueryField::QueryField(int fn, int64_t val)
    : ordinal(fn), value_(val) {}

QueryField::QueryField(int fn, int32_t val)
    : ordinal(fn), value_(val) {}

QueryField::QueryField(int field_num, emp::Integer val, int length)
    : ordinal(field_num),
      value_(types::TypeId::ENCRYPTED_INTEGER64, val) {}


QueryField::QueryField(int field_num, float val)
        : ordinal(field_num), value_(val) {}


QueryField::QueryField(int field_num, double val)
    : ordinal(field_num), value_(val) {}

QueryField::QueryField(int field_num, std::string varchar) :
        ordinal(field_num),
        value_(varchar) {}


types::Value *QueryField::GetMutableValue() { return &value_; }

types::Value *QueryField::GetValue() const {
    return const_cast<types::Value *>(&value_);
}

QueryField::QueryField() {}

QueryField::QueryField(int fn) : ordinal(fn) {}

void QueryField::SetValue(const types::Value *val) {
    value_.setValue(val);
}


QueryField QueryField::reveal(EmpParty party) const {
    bool resultEncrypted = (party == EmpParty::PUBLIC) ? false  : true;

    types::Value value = this->value_;
    QueryField result(*this);
    types::Value *dstValue;

    switch(value.getType()) {
        case types::TypeId::ENCRYPTED_FLOAT32: // Not yet implemented
        case types::TypeId::INVALID:
        case types::TypeId::BOOLEAN:
        case types::TypeId::INTEGER32:
        case types::TypeId::INTEGER64:
        case types::TypeId::FLOAT32:
        case types::TypeId::FLOAT64:
        case types::TypeId::NUMERIC:
        case types::TypeId::VARCHAR:
            return result; // copy the public field, no need to reveal
        case types::TypeId::ENCRYPTED_BOOLEAN: {
            bool decrypted = value.getEmpBit()->reveal<bool>((int) party); // returns a bool for both XOR and PUBLIC
            dstValue = new types::Value(decrypted);
            result.SetValue(dstValue);
            return result;
        }
        case types::TypeId::ENCRYPTED_INTEGER32: {
            int32_t dst = value.getEmpInt()->reveal<int32_t>((int) party);
            dstValue = new types::Value(dst);
            result.SetValue(dstValue);
            return result;
        }

        case types::TypeId::ENCRYPTED_INTEGER64: {
            int64_t dst = value.getEmpInt()->reveal<int64_t>((int) party);
            dstValue = new types::Value(dst);
            result.SetValue(dstValue);
            return result;
        }


    };


    }

std::ostream &vaultdb::operator<<(std::ostream &strm, const QueryField &aField) {

    return strm << aField.value_.getValueString();
}

void QueryField::initialize(QueryField &field) {
    value_.setValue(field.GetValue());
    ordinal = field.ordinal;
}
