#include "field.h"
#include "util/utilities.h"
#include "field_factory.h"
#include <util/field_utilities.h>

using namespace vaultdb;


std::ostream &vaultdb::operator<<(std::ostream &os, const Field<bool> &aValue) {
    return os << aValue.toString();
}

std::ostream &vaultdb::operator<<(std::ostream &os, const Field<emp::Bit> &aValue) {
    return os << aValue.toString();
}

template<typename B>
Field<B>::Field() :  payload_(false), type_(FieldType::INVALID), string_length_(0) {
    assert(FieldUtilities::validateTypeAlignment(*this));

}

template<typename B>
Field<B>::Field(const FieldType &fieldType, const Value &val, const int &strLength) : payload_(val), type_(fieldType), string_length_(strLength) {
    assert(FieldUtilities::validateTypeAlignment(*this));
    if(type_ == FieldType::STRING || type_ == FieldType::SECURE_STRING)
        assert(string_length_ > 0);
}


template<typename B>
Field<B>::Field(const Field<B> &field) :  payload_(field.payload_), type_(field.type_), string_length_(field.string_length_) {
    assert(FieldUtilities::validateTypeAlignment(*this));
    if(type_ == FieldType::STRING || type_ == FieldType::SECURE_STRING)
        assert(string_length_ > 0);
}

template<typename B>
Field<B>::Field(const B &value) : string_length_(0) {
    payload_ = value;
    type_ = (std::is_same_v<bool, B>)  ? FieldType::BOOL : FieldType::SECURE_BOOL;

}


template<typename B>
Field<B> &Field<B>::operator=(const Field<B> &other) {

    if(type_ == FieldType::INVALID && other.type_ == FieldType::INVALID) return *this; // nothing to do here
    assert(other.type_ != FieldType::INVALID);

    if(other.type_ == FieldType::STRING)
        assert(other.string_length_ > 0);

    if(&other == this)
        return *this;

    payload_ = other.payload_;
    string_length_ = other.string_length_;
    type_ = other.type_;
    return *this;

}

template<typename B>
FieldType Field<B>::getType() const {
    return type_;
}

template<typename B>
size_t Field<B>::getSize() const {

    // serialization size
    if(type_ == FieldType::SECURE_INT || type_ == FieldType::SECURE_LONG) {
        emp::Integer i = boost::get<emp::Integer>(payload_);
        return i.size();
    }

    return FieldUtilities::getPhysicalSize(type_, string_length_);

}







template<typename B>
B Field<B>::operator!=(const Field &cmp) const {
    B eq = *this == cmp;
    return !eq;
}

// only applicable to bool/Bit types
template<typename B>
B Field<B>::operator!() const {
    return !(boost::get<B>(payload_));
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
    Value result = secretShareHelper((PlainField &) *this, anon, 0, true);
    FieldType resType = TypeUtilities::toSecure(type_);
    return SecureField(resType, result, string_length_);
}




template<typename B>
SecureField Field<B>::secret_share_send(const PlainField &src, const QueryFieldDesc &field_desc, const int &src_party) {
    Value result = secretShareHelper(src, field_desc, src_party, true);
    FieldType resType = TypeUtilities::toSecure(src.type_);

    return SecureField(resType, result, src.string_length_);
}

template<typename B>
SecureField
Field<B>::secret_share_recv(const FieldType &type, const QueryFieldDesc &&field_desc, const int &src_party) {
    assert(TypeUtilities::isEncrypted(type));

    FieldType plain_type = TypeUtilities::toPlain(type);
    Value input = FieldFactory<bool>::getZero(plain_type).payload_;
    PlainField p(plain_type, input, field_desc.getStringLength());
    Value result = secretShareHelper(p, field_desc, src_party, false);
    return SecureField(type, result, field_desc.getStringLength());


}

template<typename B>
std::string Field<B>::toString() const {

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
            return getValue<std::string>();
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


template<typename B>
B Field<B>::operator>=(const Field &r) const {

    B lhs, rhs, gt, eq;
    Value v;
    emp::Float fl, fr;

    emp::Integer li, ri;

    switch(type_) {
        case FieldType::BOOL:
             return getValue<bool>() >= r.getValue<bool>();
        case FieldType::INT:
            return getValue<int32_t>() >= r.getValue<int32_t>();
        case FieldType::LONG:
            return getValue<int64_t>() >= r.getValue<int64_t>();
        case FieldType::FLOAT:
            return (getValue<float_t>()>= r.getValue<float_t>());
            // TODO: consider adding manual equality check, like "epsilon" in operator===
        case FieldType::STRING:
            return getValue<string>() >= r.getValue<string>();
        case FieldType::SECURE_BOOL:
            lhs = getValue<B>();
            rhs = r.getValue<B>();
            gt = lhs & (!rhs);
            eq = (lhs == rhs);
            return eq | gt;
        case FieldType::SECURE_INT:
        case FieldType::SECURE_LONG:
        case FieldType::SECURE_STRING:
            li = boost::get<emp::Integer>(payload_);
            ri = boost::get<emp::Integer>(r.payload_);

            li = FieldUtilities::addSignBit(li);
            ri = FieldUtilities::addSignBit(ri);

            v = (li >= ri);
            return boost::get<B>(v);
        case FieldType::SECURE_FLOAT:
            fl = getValue<emp::Float>();
            fr = r.getValue<emp::Float>();
            v = !(fl.less_than(fr));
            return boost::get<B>(v);
        default:
            throw;
    }
}




template<typename B>
PlainField Field<B>::reveal(const QueryFieldDesc &schema, const int &party) const {

    FieldType resType = TypeUtilities::toPlain(type_);
    size_t strLength = 0;
    Value v;

    emp::Bit b;
    emp::Integer i;
    emp::Float f;

    switch(type_) {
        case FieldType::BOOL:
        case FieldType::INT:
        case FieldType::LONG:
        case FieldType::FLOAT:
        case FieldType::STRING:
            v = payload_;
            strLength = string_length_;
            break;
        case FieldType::SECURE_BOOL:
            b = getValue<emp::Bit>();
            v =  b.reveal(party);
            break;
        case FieldType::SECURE_INT:
            i = getValue<emp::Integer>();
            v =  (int32_t) (i.reveal<int32_t>(party) + schema.getFieldMin());
            break;
        case FieldType::SECURE_LONG:
            i = getValue<emp::Integer>();
            v = i.reveal<int64_t>(party) + schema.getFieldMin();
            break;
        case FieldType::SECURE_STRING:
            i = getValue<emp::Integer>();
            v = revealString(i, party);
            strLength = i.size() / 8;
            assert(strLength > 0);
            break;
        case FieldType::SECURE_FLOAT:
            f = getValue<emp::Float>();
            v = (float_t) f.reveal<double>(party);
            break;
        default:
            throw;
    }

    return PlainField(resType, v, strLength);
}

template<typename B>
std::string Field<B>::revealString(const emp::Integer &src, const int &party) {
    long bitCount = src.size();

    bool *bools = new bool[bitCount];
    std::string bitString = src.reveal<std::string>(party);

    std::string::iterator strPos = bitString.begin();
    for (int i = 0; i < bitCount; ++i) {
        bools[i] = (*strPos == '1');
        ++strPos;
    }

    vector<int8_t> decodedBytesVector = Utilities::boolsToBytes(bools, bitCount);

    string dst((char *) decodedBytesVector.data(), decodedBytesVector.size());


    std::reverse(dst.begin(), dst.end());


    delete[] bools;
    return dst;
}

template<typename B>
B Field<B>::operator==(const Field<B> &r) const {

    if(type_ != r.type_) return B(false);
    if(getSize() != r.getSize()) return B(false);

    float_t lhs, rhs, epsilon;
    Value v;

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
            epsilon = std::fabs(lhs * 0.00001);
            rhs = r.getValue<float_t>();
            return std::fabs(rhs - lhs) <=  epsilon;
        case FieldType::STRING:
            return getValue<string>() == r.getValue<string>();
        case FieldType::SECURE_BOOL:
            v = (getValue<emp::Bit>() == r.getValue<emp::Bit>());
            return boost::get<B>(v);
        case FieldType::SECURE_INT:
        case FieldType::SECURE_LONG:
        case FieldType::SECURE_STRING:
            v = (getValue<emp::Integer>() == r.getValue<emp::Integer>());
            return boost::get<B>(v);
        case FieldType::SECURE_FLOAT:
            v = (getValue<emp::Float>()).equal(r.getValue<emp::Float>());
            return boost::get<B>(v);
        default:
            throw;
    }
}


template<typename B>
void Field<B>::serialize(int8_t *dst) const {
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
            std::reverse(s.begin(), s.end()); // reverse it so we can more easily conver to EMP format
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
            memcpy(dst, (int8_t *) si.bits.data(), si.size() * TypeUtilities::getEmpBitSize());
            break;
        case FieldType::SECURE_FLOAT:
            sf = boost::get<emp::Float>(payload_);
            memcpy(dst, (int8_t *) sf.value.data(), sf.size() * TypeUtilities::getEmpBitSize());
            break;
        default:
            throw;
    }
}

template<typename B>
Value
Field<B>::secretShareHelper(const PlainField &f, const QueryFieldDesc &field_desc, const int &party, const bool &send) {


    switch (f.type_) {
        case FieldType::BOOL:
            return emp::Bit(f.getValue<bool>(), party);
        case FieldType::INT:
            return emp::Integer(field_desc.size(),  (send) ?  boost::get<int32_t>(f.payload_) - field_desc.getFieldMin()
                                                           : boost::get<int32_t>(f.payload_), party);

        case FieldType::LONG:
            return emp::Integer(field_desc.size(),  (send) ?  boost::get<int64_t>(f.payload_) - field_desc.getFieldMin()
                                                           : boost::get<int64_t>(f.payload_), party);
        case FieldType::STRING:
            return secretShareString(boost::get<string>(f.payload_), send, f.string_length_, party);
        case FieldType::FLOAT:
            return emp::Float(f.getValue<float_t>(), party);
        case FieldType::SECURE_BOOL:
            return f.getValue<emp::Bit>();
        case FieldType::SECURE_INT:
        case FieldType::SECURE_LONG:
        case FieldType::SECURE_STRING:
            return f.getValue<emp::Integer>();
        case FieldType::SECURE_FLOAT:
            return f.getValue<emp::Float>();

        default:
            throw;
    }

}

template<typename B>
emp::Integer Field<B>::secretShareString(const string &s, const bool &to_send, const size_t &str_length, const int
&party) {
    assert(str_length > 0);

    size_t string_bit_count = str_length * 8;

    emp::Integer payload = emp::Integer(string_bit_count, 0L, party);
    if (to_send) {
        std::string input = s;
        std::reverse(input.begin(), input.end());
        bool *bools = Utilities::bytesToBool((int8_t *) input.c_str(), str_length);

        emp::ProtocolExecution::prot_exec->feed((emp::block *) payload.bits.data(), party, bools,
                                                string_bit_count);
        delete[] bools;

    } else {
        emp::ProtocolExecution::prot_exec->feed((emp::block *) payload.bits.data(), party, nullptr,
                                                string_bit_count);
    }



    return payload;
}

template<typename B>
Field<B> Field<B>::If(const B &choice, const Field &l, const Field &r) {
    assert(l.getType() == r.getType());
    assert(l.string_length_ == r.string_length_);

    Value choiceBit(choice);
    Value v;


    switch (l.type_) {
        case FieldType::BOOL:
        case FieldType::INT:
        case FieldType::LONG:
        case FieldType::STRING:
        case FieldType::FLOAT:
            v =  (boost::get<bool>(choiceBit)) ? l.payload_ : r.payload_;
            break;
        case FieldType::SECURE_BOOL:
            v  = emp::If(boost::get<emp::Bit>(choiceBit), boost::get<emp::Bit>(l.payload_), boost::get<emp::Bit>(r.payload_));
            break;
        case FieldType::SECURE_INT:
        case FieldType::SECURE_LONG:
        case FieldType::SECURE_STRING:
            v  = emp::If(boost::get<emp::Bit>(choiceBit), boost::get<emp::Integer>(l.payload_), boost::get<emp::Integer>
                    (r.payload_));
            break;
        case FieldType::SECURE_FLOAT:
            v  = emp::If(boost::get<emp::Bit>(choiceBit), boost::get<emp::Float>(l.payload_), boost::get<emp::Float>
                    (r.payload_));
            break;
        default:
            throw;

    }

    return Field(l.getType(), v, l.string_length_);
}






template<typename B>
void Field<B>::compareAndSwap(const B & choice, Field & l, Field & r) {
    assert(l.getType() == r.getType());
    assert(l.string_length_ == r.string_length_);

    Value choiceBit = choice;
    Value lhs  = l.payload_;
    Value rhs = r.payload_;
    Value v;

    Value tmp;
    emp::Bit bl, br;
    emp::Integer il, ir;
    emp::Float fl, fr;


    switch (l.type_) {
        case FieldType::BOOL:
            swap<bool>(&lhs, &rhs);
            break;
        case FieldType::INT:
            swap<int32_t>(&lhs, &rhs);;
            break;
        case FieldType::LONG:
            swap<int64_t>(&lhs, &rhs);;
            break;
        case FieldType::STRING:
        case FieldType::FLOAT:
            if (boost::get<bool>(choiceBit)) {
                tmp = lhs;
                lhs = rhs;
                rhs = tmp;
            }
            break;
        case FieldType::SECURE_BOOL:
            bl = boost::get<emp::Bit>(lhs);
            br = boost::get<emp::Bit>(rhs);
            emp::swap(boost::get<emp::Bit>(choiceBit), bl, br);
            lhs = bl;
            rhs = br;
            break;
        case FieldType::SECURE_INT:
        case FieldType::SECURE_LONG:
        case FieldType::SECURE_STRING:
            il = boost::get<emp::Integer>(lhs);
            ir = boost::get<emp::Integer>(rhs);
            emp::swap(boost::get<emp::Bit>(choiceBit), il, ir);
            lhs = il;
            rhs = ir;
            break;
        case FieldType::SECURE_FLOAT:
            fl = boost::get<emp::Float>(lhs);
            fr = boost::get<emp::Float>(rhs);
            emp::swap(boost::get<emp::Bit>(choiceBit), fl, fr);
            lhs = fl;
            rhs = fr;
            break;
        default:
            throw;
    }
}


template<typename B>
Field<B> Field<B>::operator+(const Field &rhs) const {
    assert(type_ == rhs.getType());

    Value v;

    switch(type_) {
        case FieldType::INT:
            v = plusHelper<int32_t>(rhs);
            break;
        case FieldType::LONG:
            v = plusHelper<int64_t>(rhs);
            break;
        case FieldType::FLOAT:
            v = plusHelper<float_t>(rhs);
            break;
        case FieldType::SECURE_INT:
        case FieldType::SECURE_LONG:
            v = plusHelper<emp::Integer>(rhs);
            break;
        case FieldType::SECURE_FLOAT:
            v = plusHelper<emp::Float>(rhs);
            break;
        default: // remaining types (BOOL, STRING, etc) go here
            throw;
    }
    return Field(type_, v, string_length_);

}

template<typename B>
Field<B> Field<B>::operator-(const Field &rhs) const {
    assert(type_ == rhs.getType());

    Value v;

    switch(type_) {
        case FieldType::INT:
            v = minusHelper<int32_t>(rhs);
            break;
        case FieldType::LONG:
            v = minusHelper<int64_t>(rhs);
            break;
        case FieldType::FLOAT:
            v = minusHelper<float_t>(rhs);
            break;
        case FieldType::SECURE_INT:
        case FieldType::SECURE_LONG:
            v = minusHelper<emp::Integer>(rhs);
            break;
        case FieldType::SECURE_FLOAT:
            v = minusHelper<emp::Float>(rhs);
            break;
        default: // remaining types (BOOL, STRING, etc) go here
            throw;
    }
    return Field(type_, v, string_length_);
}


template<typename B>
Field<B> Field<B>::operator*(const Field &rhs) const {
    assert(type_ == rhs.getType());

    Value v;

    switch(type_) {
        case FieldType::INT:
            v = timesHelper<int32_t>(rhs);
            break;
        case FieldType::LONG:
            v = timesHelper<int64_t>(rhs);
            break;
        case FieldType::FLOAT:
            v = timesHelper<float_t>(rhs);
            break;
        case FieldType::SECURE_INT:
        case FieldType::SECURE_LONG:
            v = timesHelper<emp::Integer>(rhs);
            break;
        case FieldType::SECURE_FLOAT:
            v = timesHelper<emp::Float>(rhs);
            break;
        default: // remaining types (BOOL, STRING, etc) go here
            throw;
    }
    return Field(type_, v, string_length_);
}

template<typename B>
Field<B> Field<B>::operator/(const Field &rhs) const {   assert(type_ == rhs.getType());

    Value v;

    switch(type_) {
        case FieldType::INT:
             v = (rhs.getValue<int32_t>() == 0)
                     ?  (int32_t) 0 :  divHelper<int32_t>(rhs);
            break;
        case FieldType::LONG:
            v = (rhs.getValue<int64_t>() == 0L)
                ?  (int64_t) 0L :  divHelper<int64_t>(rhs);
            break;
        case FieldType::FLOAT:
            v = (rhs.getValue<float_t>() == 0.0)
                ?  (float_t) 0.0 :  divHelper<float_t>(rhs);
            break;
        case FieldType::SECURE_INT:
        case FieldType::SECURE_LONG:
            v = divHelper<emp::Integer>(rhs);
            break;
        case FieldType::SECURE_FLOAT:
            v = divHelper<emp::Float>(rhs);
            break;
        default: // remaining types (BOOL, STRING, etc) go here
            throw;
    }
    return Field(type_, v, string_length_);
}

template<typename B>
Field<B> Field<B>::operator%(const Field &rhs) const {
    assert(type_ == rhs.getType());

    Value v;

    switch(type_) {
        case FieldType::INT:
            v = modHelper<int32_t>(rhs);
            break;
        case FieldType::LONG:
            v = modHelper<int64_t>(rhs);
            break;
        case FieldType::SECURE_INT:
        case FieldType::SECURE_LONG:
            v = modHelper<emp::Integer>(rhs);
            break;
        default: // remaining types (BOOL, STRING, FLOAT) go here
            throw;
    }
    return Field(type_, v, string_length_);
}


template<typename B>
B Field<B>::operator&&(const Field &cmp) const {
    assert(type_ == cmp.getType());

    if(type_ == FieldType::BOOL || type_ == FieldType::SECURE_BOOL) {
        B lhs = getValue<B>();
        B rhs = cmp.getValue<B>();
        B res = lhs & rhs;

        return res;
    }

    // Not applicable for all other types
    throw;

}

template<typename B>
B Field<B>::operator||(const Field &cmp) const {
    assert(type_ == cmp.getType());

    if(type_ == FieldType::BOOL || type_ == FieldType::SECURE_BOOL) {
        B lhs = getValue<B>();
        B rhs = cmp.getValue<B>();
        return lhs | rhs;
    }

    // Not applicable for all other types
    throw;

}





template class vaultdb::Field<bool>;
template class vaultdb::Field<emp::Bit>;







