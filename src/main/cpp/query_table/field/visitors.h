#ifndef _ARITHMETIC_VISITORS_H
#define _ARITHMETIC_VISITORS_H

#include <boost/variant.hpp>
#include <query_table/field/field_type.h>
#include <util/utilities.h>

namespace vaultdb {


    struct ToStringVisitor : public boost::static_visitor<std::string> {
        std::string operator()(bool b) const { return b ? "true" : "false"; }

        std::string operator()(int32_t i) const { return std::to_string(i); }

        std::string operator()(int64_t i) const { return std::to_string(i); }

        std::string operator()(float_t f) const { return std::to_string(f); }

        std::string operator()(std::string s) const { return s; }

        std::string operator()(emp::Bit b) const { return "SECRET BIT"; }

        std::string operator()(emp::Float i) const { return "SECRET FLOAT"; }

        std::string operator()(emp::Integer i) const {
            switch (type) {
                case FieldType::SECURE_INT:
                    return "SECRET INT";
                case FieldType::SECURE_LONG:
                    return "SECRET LONG";
                case FieldType::SECURE_STRING:
                    return "SECRET STRING";
                default:
                    throw;
            }
        }

        FieldType type = FieldType::INVALID;

    };


    struct GreaterThanEqVisitor : public boost::static_visitor<Value> {
        Value operator()(bool b) const {  return b >= boost::get<bool>(rhs); }

        Value operator()(int32_t i) const { return i >= boost::get<int32_t>(rhs); }

        Value operator()(int64_t i) const { return i >= boost::get<int64_t>(rhs); }

        Value operator()(float_t f) const {  return f >= boost::get<float_t>(rhs); }

        Value operator()(std::string s) const {  return s >= boost::get<std::string>(rhs); }

        Value operator()(emp::Bit l) const {
            emp::Bit r = boost::get<emp::Bit>(rhs);
            emp::Bit gt = (l == emp::Bit(true)) & (r == emp::Bit(false));
            emp::Bit eq = (l == r);
            return eq | gt;
        }

        Value operator()(emp::Integer l) const {
            emp::Integer r = boost::get<emp::Integer>(rhs);
            return l >= r;
        }

        Value operator()(emp::Float l) const {
            emp::Float r = boost::get<emp::Float>(rhs);
            return r.less_equal(l);
        }

        Value rhs;

    };

    struct EqualityVisitor : public boost::static_visitor<Value> {
        Value operator()(bool b) const { return b == boost::get<bool>(rhs); }

        Value operator()(int32_t i) const { return i == boost::get<int32_t>(rhs); }

        Value operator()(int64_t i) const { return i == boost::get<int64_t>(rhs); }

        Value operator()(float_t f) const { return f == boost::get<float_t>(rhs); }

        Value operator()(std::string s) const { return s == boost::get<std::string>(rhs); }

        Value operator()(emp::Bit l) const {
            emp::Bit r = boost::get<emp::Bit>(rhs);
            return l == r;
        }

        Value operator()(emp::Integer l) const {
            emp::Integer r = boost::get<emp::Integer>(rhs);
            return l == r;
        }

        Value operator()(emp::Float l) const {
            emp::Float r = boost::get<emp::Float>(rhs);
            return r.equal(l);
        }

        Value rhs;

    };


    struct RevealVisitor : public boost::static_visitor<Value> {
        Value operator()(bool b) const { return Value(b); }

        Value operator()(int32_t i) const { return Value(i); }

        Value operator()(int64_t i) const { return Value(i); }

        Value operator()(float_t f) const { return Value(f); }

        Value operator()(std::string s) const { return Value(s); }

        Value operator()(emp::Bit l) const {
            return l.reveal(party);
        }

        Value operator()(emp::Integer l) const {
            switch (type) {
                case FieldType::SECURE_INT:
                    return l.reveal<int32_t>(party);
                case FieldType::SECURE_LONG:
                    return l.reveal<int64_t>();
                case FieldType::SECURE_STRING:
                    return revealString(l, party);
                default:
                    throw;

            }
        }

        Value operator()(emp::Float l) const {
            float_t revealed = l.reveal<double>(party);
            return Value(revealed);
        }

        int party = emp::PUBLIC;
        FieldType type = FieldType::INVALID;

        std::string revealString(const emp::Integer &src, const int &party) const {
            long bitCount = src.size();
            long byteCount = bitCount / 8;


            bool *bools = new bool[bitCount];
            std::string bitString = src.reveal<std::string>(emp::PUBLIC);
            std::string::iterator strPos = bitString.begin();
            for (int i = 0; i < bitCount; ++i) {
                bools[i] = (*strPos == '1');
                ++strPos;
            }

            vector<int8_t> decodedBytesVector = Utilities::boolsToBytes(bools, bitCount);
            decodedBytesVector.resize(byteCount + 1);
            decodedBytesVector[byteCount] = '\0';
            string dst((char *) decodedBytesVector.data());


            std::reverse(dst.begin(), dst.end());


            delete[] bools;
            return dst;
        }
    };


    struct SerializeVisitor : public boost::static_visitor<> {
        void operator()(bool b) const { *((bool *) dst_) = b; }

        void operator()(int32_t i) const { *((int32_t *) dst_) = i; }

        void operator()(int64_t i) const { *((int64_t *) dst_) = i; }

        void operator()(float_t f) const { *((float_t *) dst_) = f; }

        void operator()(std::string p) const {
            std::reverse(p.begin(), p.end()); // reverse it so we can more easily conver to EMP format
            memcpy(dst_, (int8_t *) p.c_str(), p.size()); // null termination chopped
        }

        void operator()(emp::Bit l) const {
            memcpy(dst_, (int8_t *) &(l.bit), 1);
        }

        void operator()(emp::Integer l) const {
            memcpy(dst_, (int8_t *) l.bits.data(), l.size());
        }

        void operator()(emp::Float l) const {
            memcpy(dst_, (int8_t *) l.value.data(), 4);
        }

        std::int8_t *dst_;
        size_t string_length_;

    };


    struct SecretShareVisitor : public boost::static_visitor<Value> {
        Value operator()(bool b) const { return emp::Bit(b, dstParty); }

        Value operator()(int32_t i) const { return emp::Integer(32, i, dstParty); }

        Value operator()(int64_t i) const { return emp::Integer(64, i, dstParty); }

        Value operator()(float_t f) const { return emp::Float(f, dstParty); }

        Value operator()(std::string s) const {
            std::string input = s;
            std::reverse(input.begin(), input.end());
            bool *bools = Utilities::bytesToBool((int8_t *) input.c_str(), input.size());

            size_t stringBitCount = input.size() * 8;

            emp::Integer payload = emp::Integer(stringBitCount, 0L, dstParty);
            if (myParty == dstParty) {
                emp::ProtocolExecution::prot_exec->feed((emp::block *) payload.bits.data(), dstParty, bools,
                                                        stringBitCount);
            } else {
                emp::ProtocolExecution::prot_exec->feed((emp::block *) payload.bits.data(), dstParty, nullptr,
                                                        stringBitCount);
            }


            delete[] bools;

            return payload;
        }

        Value operator()(emp::Bit l) const { return l; }

        Value operator()(emp::Integer l) const { return l; }

        Value operator()(emp::Float l) const { return l; }

        int dstParty = emp::PUBLIC;
        int myParty = emp::PUBLIC;


    };


    struct SelectVisitor : public boost::static_visitor<Value> {
        Value operator()(bool b) const { return boost::get<bool>(choiceBit) ? b : rhs;  }

        Value operator()(int32_t i) const { return boost::get<bool>(choiceBit) ? i : rhs; }

        Value operator()(int64_t i) const { return boost::get<bool>(choiceBit) ? i : rhs; }

        Value operator()(float_t f) const { return  boost::get<bool>(choiceBit) ? f : rhs; }

        Value operator()(std::string s) const { return boost::get<bool>(choiceBit) ? s : rhs; }

        Value operator()(emp::Bit l) const {
            emp::Bit choice = boost::get<emp::Bit>(choiceBit);
            return emp::If(choice, l, boost::get<emp::Bit>(rhs));
        }

        Value operator()(emp::Integer l) const {
            emp::Bit choice = boost::get<emp::Bit>(choiceBit);
            return emp::If(choice, l, boost::get<emp::Integer>(rhs));
        }

        Value operator()(emp::Float l) const {
            emp::Bit choice = boost::get<emp::Bit>(choiceBit);
            return emp::If(choice, l, boost::get<emp::Float>(rhs));

        }

        Value choiceBit;
        Value rhs;


    };


    struct PlusVisitor : public boost::static_visitor<Value> {
        Value operator()(bool b) const { throw;  }

        Value operator()(int32_t i) const { return i + boost::get<int32_t>(rhs); }

        Value operator()(int64_t i) const { return i + boost::get<int64_t>(rhs); }

        Value operator()(float_t f) const { return  f + boost::get<float_t>(rhs); }

        Value operator()(std::string s) const { throw;  } // can't add here right now without throwing off the schema's string length -- this type of expression not yet implemented

        Value operator()(emp::Bit l) const { throw; }

        Value operator()(emp::Integer l) const { return  l + boost::get<emp::Integer>(rhs); }

        Value operator()(emp::Float l) const { return  l + boost::get<emp::Float>(rhs);  }

        Value rhs;


    };

    struct MinusVisitor : public boost::static_visitor<Value> {
        Value operator()(bool b) const { throw;  }

        Value operator()(int32_t i) const { return i - boost::get<int32_t>(rhs); }

        Value operator()(int64_t i) const { return i - boost::get<int64_t>(rhs); }

        Value operator()(float_t f) const { return  f - boost::get<float_t>(rhs); }

        Value operator()(std::string s) const { throw; }

        Value operator()(emp::Bit l) const { throw; }

        Value operator()(emp::Integer l) const { return  l - boost::get<emp::Integer>(rhs); }

        Value operator()(emp::Float l) const { return  l - boost::get<emp::Float>(rhs);  }

        Value rhs;


    };


    struct MultiplyVisitor : public boost::static_visitor<Value> {
        Value operator()(bool b) const { throw;  }

        Value operator()(int32_t i) const { return i * boost::get<int32_t>(rhs); }

        Value operator()(int64_t i) const { return i * boost::get<int64_t>(rhs); }

        Value operator()(float_t f) const { return  f * boost::get<float_t>(rhs); }

        Value operator()(std::string s) const { throw; }

        Value operator()(emp::Bit l) const { throw; }

        Value operator()(emp::Integer l) const { return  l * boost::get<emp::Integer>(rhs); }

        Value operator()(emp::Float l) const { return  l * boost::get<emp::Float>(rhs);  }

        Value rhs;


    };


    struct DivisionVisitor : public boost::static_visitor<Value> {
        Value operator()(bool b) const { throw;  }

        Value operator()(int32_t i) const { return i / boost::get<int32_t>(rhs); }

        Value operator()(int64_t i) const { return i / boost::get<int64_t>(rhs); }

        Value operator()(float_t f) const { return  f / boost::get<float_t>(rhs); }

        Value operator()(std::string s) const { throw; }

        Value operator()(emp::Bit l) const { throw; }

        Value operator()(emp::Integer l) const { return  l / boost::get<emp::Integer>(rhs); }

        Value operator()(emp::Float l) const { return  l / boost::get<emp::Float>(rhs);  }

        Value rhs;


    };

    struct ModulusVisitor : public boost::static_visitor<Value> {
        Value operator()(bool b) const { throw;  }

        Value operator()(int32_t i) const { return i % boost::get<int32_t>(rhs); }

        Value operator()(int64_t i) const { return i % boost::get<int64_t>(rhs); }

        Value operator()(float_t f) const { throw; }

        Value operator()(std::string s) const { throw;  }

        Value operator()(emp::Bit l) const { throw; }

        Value operator()(emp::Integer l) const { return  l % boost::get<emp::Integer>(rhs); }

        Value operator()(emp::Float l) const { throw;  }

        Value rhs;

    };

}
#endif //_VISITORS_H