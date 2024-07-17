#include "field.h"
#include "util/utilities.h"
#include <util/system_configuration.h>

using namespace vaultdb;


std::ostream &vaultdb::operator<<(std::ostream &os, const Field<bool> &val) {
    return os << val.toString();
}

std::ostream &vaultdb::operator<<(std::ostream &os, const Field<Bit> &val) {
    return os << val.toString();
}





template<typename B>
B Field<B>::operator<(const Field &rhs) const {
    B geq = (*this >= rhs);
    return  !geq;
}

template<typename B>
B Field<B>::operator<=(const Field &rhs) const {
    return rhs >= *this;
}

template<typename B>
B Field<B>::operator>(const Field &rhs) const {
    return  !(rhs >= *this);
}



// secret sharing as public
template<typename B>
SecureField Field<B>::secret_share() const {
    QueryFieldDesc anon(0, "anon", "", type_, 0);
    if(type_ == TypeUtilities::toSecure(type_))
        return *((SecureField *) this);
    return secretShareHelper((PlainField &) *this, anon, 0, true);
}




template<typename B>
SecureField Field<B>::secret_share_send(const PlainField &src, const QueryFieldDesc &field_desc, const int &src_party) {
    return  secretShareHelper(src, field_desc, src_party, true);
}



template<typename B>
SecureField
Field<B>::secret_share_recv(const QueryFieldDesc &field_desc, const int &src_party) {
    assert(TypeUtilities::isSecretShared(field_desc.getType()));

    FieldType plain_type = TypeUtilities::toPlain(field_desc.getType());

    PlainField p(plain_type);
    return secretShareHelper(p, field_desc, src_party, false);


}

template<typename B>
string Field<B>::toString() const {

    switch(type_) {
        case FieldType::BOOL:
            return getValue<bool>() ? "true" : "false";
        case FieldType::INT:
            return std::to_string(getValue<int32_t>());
        case FieldType::LONG:
            return std::to_string(getValue<int64_t>());
        case FieldType::FLOAT:
            return std::to_string(getValue<float_t>());
        case FieldType::STRING:
            return this->getString();
        case FieldType::SECURE_BOOL:
            return "SECRET BIT";
        case FieldType::SECURE_INT:
            return "SECRET INT";
        case FieldType::SECURE_LONG:
            return "SECRET LONG";
        case FieldType::SECURE_FLOAT:
            return "SECRET FLOAT";
        case FieldType::SECURE_STRING:
            return "SECRET STRING";
        default:
            return "INVALID";
    }
}


template<>
bool Field<bool>::operator>=(const Field<bool> &r) const {

    switch(type_) {
        case FieldType::BOOL:
             return getValue<bool>() >= r.getValue<bool>();
        case FieldType::INT:
            return getValue<int32_t>() >= r.getValue<int32_t>();
        case FieldType::LONG:
            return getValue<int64_t>() >= r.getValue<int64_t>();
        case FieldType::FLOAT:
            return (getValue<float_t>()>= r.getValue<float_t>());
            // TODO: consider adding manual equality check, like "epsilon" in operator==
        case FieldType::STRING:
            return getString() >= r.getString();
        default:
            throw;
    }
}


template<>
Bit Field<Bit>::operator>=(const Field<Bit> &r) const {

    Bit lhs, rhs, gt, eq;
    Float fl, fr;
    Integer li, ri;

    switch(type_) {
        case FieldType::SECURE_BOOL: {
            lhs = *((Bit *) payload_.data());
            rhs = *((Bit *) r.payload_.data());
            gt = lhs & (!rhs);
            eq = (lhs == rhs);
            return eq | gt;
        }
        case FieldType::SECURE_INT:
        case FieldType::SECURE_LONG:
        case FieldType::SECURE_STRING: {
            li = this->getInt();
            ri = r.getInt();
            return (li >= ri);
        }
        case FieldType::SECURE_FLOAT:
            memcpy(fl.value.data(), payload_.data(), 32 * sizeof(Bit));
            memcpy(fr.value.data(), r.payload_.data(), 32 * sizeof(Bit));
            return  !(fl.less_than(fr));
        default:
            throw;
    }
}


template<>
PlainField Field<bool>::reveal(const QueryFieldDesc &desc, const int &party) const {
    // already plain
    return *this;
}


template<typename B>
PlainField Field<B>::reveal( const QueryFieldDesc &desc, const int &party) const {


    Bit b;
    Integer i;
    Float f;
    int32_t i32;
    int64_t i64;
    float_t flt;
    string s;

    switch(type_) {
        case FieldType::SECURE_BOOL:
            b =  *((Bit *) payload_.data());
            return Field<bool>(FieldType::BOOL, b.reveal(party));
        case FieldType::SECURE_INT:
            i = getInt();
            i32 =  i.reveal<int32_t>(party);
            if(desc.bitPacked())     i32 += desc.getFieldMin();
            return Field<bool>(FieldType::INT, i32);
        case FieldType::SECURE_LONG:
            i = getInt();
            i64 = i.reveal<int64_t>(party);
            if(desc.bitPacked())   i64 += desc.getFieldMin();
            return Field<bool>(FieldType::LONG, i64);
        case FieldType::SECURE_STRING:
            i = getInt();
            s = revealString(i, party);
            return PlainField(FieldType::STRING, s);
        case FieldType::SECURE_FLOAT:
            f = getFloat();
            flt = (float_t) f.reveal<double>(party);
            return PlainField(FieldType::FLOAT, flt);
        default:
            throw;
    }

}

template<typename B>
string Field<B>::revealString(const Integer &src, const int &party) {
    long bit_cnt = src.size();
    long byte_cnt = bit_cnt / 8;
    assert(bit_cnt % 8 == 0);

    bool *bools = new bool[bit_cnt];
    EmpManager *manager = SystemConfiguration::getInstance().emp_manager_;
    manager->reveal(bools, party, (Bit *) src.bits.data(), bit_cnt);

    bool *byte_cursor = bools;
    string dst(byte_cnt, ' ');
    int write_cursor = byte_cnt - 1;
    for(int i = 0; i < byte_cnt; ++i) {
        dst[write_cursor] =  Utilities::boolsToByte(byte_cursor);
        byte_cursor += 8;
        --write_cursor;
    }

    delete[] bools;
    return dst;
}

template<>
bool Field<bool>::operator==(const Field<bool> &r) const {

    if(type_ != r.type_) return false;
    if(payload_.size() != r.payload_.size()) return false;

    float_t lhs, rhs, epsilon;

    switch(type_) {
        case FieldType::BOOL:
            return getValue<bool>() == r.getValue<bool>();
        case FieldType::INT:
            return getValue<int32_t>() == r.getValue<int32_t>();
        case FieldType::LONG:
            return getValue<int64_t>() == r.getValue<int64_t>();
        case FieldType::FLOAT:
            // approx float equality
            lhs = getValue<float_t>();
            epsilon = std::fabs(lhs *  0.00001);
            rhs = r.getValue<float_t>();
            return std::fabs(rhs - lhs) <=  epsilon;
        case FieldType::STRING:
            return getString() == r.getString();
        default:
            throw;
    }
}


template<>
Bit Field<Bit>::operator==(const Field<Bit> &r) const {

    if(type_ != r.type_) return Bit(false);
    if(payload_.size() != r.payload_.size()) return Bit(false);



    switch(type_) {
        case FieldType::SECURE_BOOL: {
            Bit lhs = *((Bit *) payload_.data());
            Bit rhs = *((Bit *) r.payload_.data());
            return (lhs == rhs);
            }
        case FieldType::SECURE_INT:
        case FieldType::SECURE_LONG:
        case FieldType::SECURE_STRING: {
            Integer lhs = getInt();
            Integer rhs = r.getInt();
            return (lhs == rhs);
        }
        case FieldType::SECURE_FLOAT: {
            return getFloat().equal(r.getFloat());
        }
        default:
            throw;
    }
}

// dst field desc, src_party
template<typename B>
SecureField Field<B>::secretShareHelper(const PlainField &f, const QueryFieldDesc &desc, const int &party, const bool &send) {


    switch (f.type_) {
        case FieldType::BOOL: {
            bool src = (send) ? f.getValue<bool>() : false;
            Bit b = Bit(src, party);
            return SecureField(FieldType::SECURE_BOOL, b);
        }
        case FieldType::INT: {
            int32_t val = (send) ? f.getValue<int32_t>() : 0;
            if (desc.bitPacked() && send) val = val - desc.getFieldMin();
            // + bit_packed to add one bit if we are padding it for two's complement
            Integer v(desc.size() + desc.bitPacked(), val, party);
            return SecureField(FieldType::SECURE_INT, v);
            break;
        }
        case FieldType::LONG: {
            int64_t val = (send) ? f.getValue<int64_t>() : 0L;
            if (desc.bitPacked() && send) val = val - desc.getFieldMin();
            Integer v(desc.size() + desc.bitPacked(), val, party);
            return SecureField(FieldType::SECURE_LONG, v);
        }
        case FieldType::STRING: {
            string s = (send) ? f.getString() : "";
            Integer v = secretShareString(s, send, party, desc.getStringLength());
            return SecureField(FieldType::SECURE_STRING, v);
        }
        case FieldType::FLOAT: {
            float val = (send) ? f.getValue<float_t>() : 0.0;
            Float v(val, party);
            return SecureField(FieldType::SECURE_FLOAT, v);
        }
        default: // type alignment error
            throw ;
    }

}

template<typename B>
Integer Field<B>::secretShareString(const string &s, const bool &to_send, const int &src_party, const int &str_length) {
    assert(str_length > 0); // true length, s passed in might be empty for non-sending parties

    size_t bit_cnt = str_length * 8;

    Integer payload = Integer(bit_cnt, 0L, src_party);
    EmpManager *manager = SystemConfiguration::getInstance().emp_manager_;


    if (to_send) {
        string input = s;
        Utilities::reverseString(input.data(), input.size());
        bool *bools = Utilities::bytesToBool((int8_t *) input.c_str(), str_length);
        manager->feed(payload.bits.data(), src_party, bools, bit_cnt);
        delete[] bools;

    } else {
        bool bools[bit_cnt];
        memset(bools, 0, bit_cnt);
        manager->feed(payload.bits.data(), src_party, bools, bit_cnt);
    }

    return payload;
}

template<>
Field<bool> Field<bool>::If(const bool &choice, const Field<bool> &l, const Field<bool> &r) {
    assert(l.getType() == r.getType());
    assert(l.payload_.size() == r.payload_.size());

    return choice ? l : r;
}


template<>
Field<Bit> Field<Bit>::If(const Bit &choice, const Field<Bit> &l, const Field<Bit> &r) {
    assert(l.getType() == r.getType());
    assert(l.payload_.size() == r.payload_.size());

    switch (l.type_) {
        case FieldType::SECURE_BOOL: {
            Bit v  = emp::If(choice, l.getValue<Bit>(), r.getValue<Bit>());
            return SecureField(FieldType::SECURE_BOOL, v);
        }
        case FieldType::SECURE_INT:
        case FieldType::SECURE_LONG:
        case FieldType::SECURE_STRING: {
            auto lhs =  l.getInt();
            auto rhs =  r.getInt();
            Integer v = emp::If(choice, lhs, rhs);
            return SecureField(l.getType(), v);
        }
        case FieldType::SECURE_FLOAT: {
            Float lhs = l.getFloat();
            Float rhs = r.getFloat();
            Float v = emp::If(choice, lhs, rhs);
            return SecureField(l.getType(), v);
        }
        default:
            throw;

    }
}


template<>
void Field<bool>::compareSwap(const bool & choice, Field<bool> & l, Field<bool> & r) {
    assert(l.getType() == r.getType());
    assert(l.payload_.size() == r.payload_.size());

    switch (l.type_) {
        case FieldType::BOOL:
        case FieldType::INT:
        case FieldType::LONG:
        case FieldType::STRING:
        case FieldType::FLOAT:
           if(choice) {
               swap(l.payload_, r.payload_);
           }
        default:
            throw;
    }
}


template<>
void Field<Bit>::compareSwap(const Bit & choice, Field<Bit> & l, Field<Bit> & r) {
    assert(l.getType() == r.getType());
    assert(l.payload_.size() == r.payload_.size());
    switch (l.type_) {
        case FieldType::SECURE_BOOL: {
            Bit lhs = l.getValue<Bit>();
            Bit rhs = r.getValue<Bit>();
            emp::swap(choice, lhs, rhs);
            break;
        }
        case FieldType::SECURE_INT:
        case FieldType::SECURE_LONG:
        case FieldType::SECURE_STRING: {
            Integer lhs = l.getInt();
            Integer rhs = r.getInt();
            emp::swap(choice, lhs, rhs);
            break;
        }
        case FieldType::SECURE_FLOAT: {
            auto lhs = l.getFloat();
            auto rhs = r.getFloat();
            emp::swap(choice, lhs, rhs);
            break;
        }
        default:
            throw;
    }
}


template<typename B>
Field<B> Field<B>::operator+(const Field<B> &rhs) const {
    assert(type_ == rhs.getType());

    switch(type_) {
        case FieldType::INT:
            return Field<B>(type_, plusHelper<int32_t>(rhs));
        case FieldType::LONG:
            return Field<B>(type_, plusHelper<int64_t>(rhs));
        case FieldType::FLOAT:
            return Field<B>(type_, plusHelper<float_t>(rhs));
        case FieldType::SECURE_INT:
        case FieldType::SECURE_LONG: {
            auto l = getInt();
            auto r = rhs.getInt();
            if (l.size() == r.size()) {
                return Field<B>(type_, l + r);
            }
            // else pad to the longer size
            auto max_bit_sz = std::max<size_t>(l.size(), r.size());
            l.resize(max_bit_sz);
            r.resize(max_bit_sz);
            return Field<B>(type_, l + r);
        }
        case FieldType::SECURE_FLOAT: {
            auto l = getFloat();
            auto r = rhs.getFloat();
            return Field<B>(type_,l + r);
        }
        default: // remaining types (BOOL, STRING, etc) go here
            throw;
    }

}


template<typename B>
Field<B> Field<B>::operator-(const Field<B> &rhs) const {
    assert(type_ == rhs.getType());

    switch(type_) {
        case FieldType::INT:
            return Field<B>(type_, minusHelper<int32_t>(rhs));
        case FieldType::LONG:
            return Field<B>(type_, minusHelper<int64_t>(rhs));
        case FieldType::FLOAT:
            return Field<B>(type_, minusHelper<float_t>(rhs));
        case FieldType::SECURE_INT:
        case FieldType::SECURE_LONG: {
            auto l = getInt();
            auto r = rhs.getInt();
            if (l.size() == r.size()) {
                return Field<B>(type_, l - r);
            }
            // else pad to the longer size
            auto max_bit_sz = std::max<size_t>(l.size(), r.size());
            l.resize(max_bit_sz);
            r.resize(max_bit_sz);
            return Field<B>(type_, l - r);
        }
        case FieldType::SECURE_FLOAT: {
            auto l = getFloat();
            auto r = rhs.getFloat();
            return Field<B>(type_,l - r);
        }
        default: // remaining types (BOOL, STRING, etc) go here
            throw;
    }

}


template<typename B>
Field<B> Field<B>::operator*(const Field<B> &rhs) const {
    assert(type_ == rhs.getType());

    switch(type_) {
        case FieldType::INT:
            return Field<B>(type_, timesHelper<int32_t>(rhs));
        case FieldType::LONG:
            return Field<B>(type_, timesHelper<int64_t>(rhs));
        case FieldType::FLOAT:
            return Field<B>(type_, timesHelper<float_t>(rhs));
        case FieldType::SECURE_INT:
        case FieldType::SECURE_LONG: {
            auto l = getInt();
            auto r = rhs.getInt();
            if (l.size() == r.size()) {
                return Field<B>(type_, l * r);
            }
            // else pad to the longer size
            auto max_bit_sz = std::max<size_t>(l.size(), r.size());
            l.resize(max_bit_sz);
            r.resize(max_bit_sz);
            return Field<B>(type_, l * r);
        }
        case FieldType::SECURE_FLOAT: {
            auto l = getFloat();
            auto r = rhs.getFloat();
            return Field<B>(type_,l * r);
        }
        default: // remaining types (BOOL, STRING, etc) go here
            throw;
    }

}


template<typename B>
Field<B> Field<B>::operator/(const Field<B> &rhs) const {
    assert(type_ == rhs.getType());

    switch(type_) {
        case FieldType::INT:
            return Field<B>(type_,
                            (rhs.getValue<int32_t>() == 0)
                            ?  (int32_t) 0 :  divHelper<int32_t>(rhs));
        case FieldType::LONG:
            return Field<B>(type_,
                            (rhs.getValue<int64_t>() == 0)
                            ?  (int64_t) 0L :  divHelper<int64_t>(rhs));

        case FieldType::FLOAT:
            return Field<B>(type_,
                            (rhs.getValue<float_t>() == 0)
                            ?  (float_t) 0.0 :  divHelper<float_t>(rhs));
        case FieldType::SECURE_INT:
        case FieldType::SECURE_LONG: {
            auto l = getInt();
            auto r = rhs.getInt();
            if (l.size() == r.size()) {
                return Field<B>(type_, l / r);
            }
            // else pad to the longer size
            auto max_bit_sz = std::max<size_t>(l.size(), r.size());
            l.resize(max_bit_sz);
            r.resize(max_bit_sz);
            return Field<B>(type_, l / r);
        }
        case FieldType::SECURE_FLOAT: {
            auto l = getFloat();
            auto r = rhs.getFloat();
            return Field<B>(type_, l / r);
        }
        default: // remaining types (BOOL, STRING, etc) go here
            throw;
    }

}


template<typename B>
Field<B> Field<B>::operator%(const Field<B> &rhs) const {
    assert(type_ == rhs.getType());

    switch(type_) {
        case FieldType::INT:
            return Field<B>(type_, modHelper<int32_t>(rhs));
        case FieldType::LONG:
            return Field<B>(type_, modHelper<int64_t>(rhs));
        case FieldType::SECURE_INT:
        case FieldType::SECURE_LONG: {
            auto l = getInt();
            auto r = rhs.getInt();
            if (l.size() == r.size()) {
                return Field<B>(type_, l % r);
            }
            // else pad to the longer size
            auto max_bit_sz = std::max<size_t>(l.size(), r.size());
            l.resize(max_bit_sz);
            r.resize(max_bit_sz);
            return Field<B>(type_, l % r);
        }

        default: // remaining types (BOOL, STRING, etc) go here
            throw;
    }

}


template<typename B>
Field<B> Field<B>::operator^(const Field<B> &rhs) const {
    assert(type_ == rhs.getType());
    string l, r;

    switch(type_) {
        case FieldType::BOOL:
            return Field<B>(type_, xorHelper<bool>(rhs));
        case FieldType::INT:
            return Field<B>(type_, xorHelper<int32_t>(rhs));
        case FieldType::LONG:
            return Field<B>(type_, xorHelper<int64_t>(rhs));
        case FieldType::FLOAT:
            return Field<B>(type_, bitwiseXor<float>(rhs));
        case FieldType::STRING: {
            l = getString();
            r = rhs.getString();
            assert(l.size() == r.size());
            return Field<B>(type_, xorStrings(l, r));
        }
        case FieldType::SECURE_BOOL:
            return Field<B>(type_, xorHelper<Bit>(rhs));
        case FieldType::SECURE_INT:
        case FieldType::SECURE_LONG:
        case FieldType::SECURE_STRING:
            return Field<B>(type_, xorHelper<Integer>(rhs));
        case FieldType::SECURE_FLOAT:
            return Field<B>(type_, xorHelper<Float>(rhs));
        default: // remaining types (BOOL, STRING, FLOAT) go here
            throw;
    }
}



template<typename B>
Field<B> Field<B>::deserialize(const QueryFieldDesc &desc, const int8_t *src) {
    FieldType type = desc.getType();
    switch (type) {
        case FieldType::BOOL: {
            bool val = *((bool *) src);
            return Field<B>(type, val);
        }
        case FieldType::INT: {
            int32_t val = *((int32_t *) src);
            return Field<B>(type, val);
        }
        case FieldType::DATE:
        case FieldType::LONG: {
            int64_t val = *((int64_t *) src);
            return Field<B>(type, val);
        }
        case FieldType::FLOAT: {
            float_t val = *((float_t *) src);
            return Field<B>(type, val);
        }

        case FieldType::STRING: {
            char *val = (char *) src;
            int len = desc.getStringLength();
            string str(val, len);
            std::reverse(str.begin(), str.end());
            return Field<B>(type, str);

        }
        case FieldType::SECURE_BOOL: {
            Bit my_bit(0, PUBLIC);
            memcpy(&my_bit, src, sizeof(Bit));
            return Field<B>(type, my_bit);
        }
        case FieldType::SECURE_INT:
        case FieldType::SECURE_LONG: {
            // add one more bit for two's complement
            Integer payload(desc.size() + desc.bitPacked(), 0);
            memcpy(payload.bits.data(), src, desc.size() * sizeof(Bit));
            return Field<B>(type, payload);
        }
        case FieldType::SECURE_FLOAT: {
            Float v(0, emp::PUBLIC);
            memcpy(v.value.data(), src, 32 * sizeof(Bit));
            return Field<B>(type, v);
        }

        case FieldType::SECURE_STRING: {
            size_t bit_cnt = desc.getStringLength() * 8;
            Integer v(bit_cnt, 0, emp::PUBLIC);
            memcpy(v.bits.data(), src, bit_cnt * sizeof(Bit));
            return Field<B>(type, v);
        }
        default:
            throw std::invalid_argument("Field type " + TypeUtilities::getTypeName(type) + " not supported by deserialize()!");

    }

}

template class vaultdb::Field<bool>;
template class vaultdb::Field<Bit>;







