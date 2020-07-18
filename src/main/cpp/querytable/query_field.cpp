//
// Created by madhav on 1/13/20.
//

#include "query_field.h"
using namespace vaultdb;

QueryField::QueryField(const QueryField &qf)
    : field_num_(qf.field_num_), value_(qf.value_) {}

QueryField::QueryField(int64_t val, int fn)
    : field_num_(fn), value_(types::TypeId::INTEGER64, val) {}

QueryField::QueryField(int32_t val, int fn)
    : field_num_(fn), value_(types::TypeId::INTEGER32, val) {}

QueryField::QueryField(emp::Integer val, int length, int field_num)
    : field_num_(field_num),
      value_(types::TypeId::ENCRYPTED_INTEGER64, val, length) {}

QueryField::QueryField(double val, int field_num)
    : field_num_(field_num), value_(types::TypeId::FLOAT32, val) {}

types::Value *QueryField::GetMutableValue() { return &value_; }

const types::Value *QueryField::GetValue() const { return &value_; }

QueryField::QueryField() {}

void QueryField::SetValue(const types::Value *val) { value_.SetValue(val); }


QueryField QueryField::reveal(EmpParty party) const {
    bool resultEncrypted = (party == EmpParty::PUBLIC) ? false  : true;

    types::Value value = this->value_;
    QueryField result(*this);
    types::Value *dstValue;

    switch(value.GetType()) {
        case types::TypeId::ENCRYPTED_FLOAT32: // Not yet implemented
        case types::TypeId::INVALID:
        case types::TypeId::VAULT_DOUBLE: // what is this?  Is it encrypted?
            throw;
        case types::TypeId::BOOLEAN:
        case types::TypeId::INTEGER32:
        case types::TypeId::INTEGER64:
        case types::TypeId::FLOAT32:
        case types::TypeId::FLOAT64:
        case types::TypeId::NUMERIC:
        case types::TypeId::TIMESTAMP:
        case types::TypeId::TIME:
        case types::TypeId::DATE:
        case types::TypeId::VARCHAR:
            return result; // copy the public one, no need to reveal
        case types::TypeId::ENCRYPTED_BOOLEAN: {
            bool decrypted = value.GetEmpBit()->reveal<bool>((int) party); // returns a bool for both XOR and PUBLIC
            dstValue = new types::Value(types::TypeId::BOOLEAN, decrypted);
            result.SetValue(dstValue);
            return result;
        }
        case types::TypeId::ENCRYPTED_INTEGER32: {
            int32_t dst = value.GetEmpInt()->reveal<int32_t>((int) party);
            dstValue = new types::Value(types::TypeId::ENCRYPTED_INTEGER32, dst);
            result.SetValue(dstValue);
            return result;
        }

        case types::TypeId::ENCRYPTED_INTEGER64: {
            int64_t dst = value.GetEmpInt()->reveal<int64_t>((int) party);
            dstValue = new types::Value(types::TypeId::ENCRYPTED_INTEGER64, dst);
            result.SetValue(dstValue);
            return result;
        }


    };


    }