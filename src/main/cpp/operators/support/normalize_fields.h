#ifndef _NORMALIZE_FIELDS_H_
#define _NORMALIZE_FIELDS_H_

#include <query_table/field/field.h>
#include <util/data_utilities.h>
#include "util/type_utilities.h"
#include "util/field_utilities.h"

namespace vaultdb {
    class NormalizeFields {
    public:

        static PlainField normalize(const  PlainField &field, const SortDirection & dir, const bool & packed = false) {
            switch(field.getType()) {
                case FieldType::BOOL:
                    return (dir == SortDirection::ASCENDING) ? field : !field;
                case FieldType::INT:
                    return normalizeInt<int32_t>(field, dir);
                case FieldType::LONG:
                    return normalizeInt<int64_t>(field, dir);
                case FieldType::FLOAT: {
                    auto tmp = normalizeFloat(field, dir);
                    cout << "Normalized float " << FieldUtilities::printField(field) << "\n"
                         << "              to " << FieldUtilities::printField(tmp) << endl;
                    return tmp;
                    // return normalizeFloat(field, dir);
                }
                case FieldType::STRING:
                    return normalizeString(field, dir);
                default:
                        throw std::runtime_error("Unsupported field type");
                }
            }


        // reverse the process
        static PlainField denormalize(const  PlainField &field, const FieldType & dst_type, const SortDirection & dir, const bool & packed = false) {
            switch (dst_type) {
                case FieldType::BOOL:
                    return (dir == SortDirection::ASCENDING) ? field : !field;
                case FieldType::INT:
                    return denormalizeInt<int32_t>(field, dir);
                case FieldType::LONG:
                    return denormalizeInt<int64_t>(field, dir);
                case FieldType::FLOAT:
                    return denormalizeFloat(field, dir);
                case FieldType::STRING:
                    return normalizeString(field, dir); // reuse normalizeString - it does the same thing here
                default:
                    throw std::runtime_error("Unsupported field type");
            }
        }

        static SecureField normalize(const  SecureField &field, const SortDirection & dir, const bool & packed = false) {
            switch(field.getType()) {
                case FieldType::SECURE_BOOL:
                    return (dir == SortDirection::ASCENDING) ? field : !field;
                case FieldType::SECURE_INT:
                case FieldType::SECURE_LONG:{
//                    auto tmp =  normalizeInt(field, dir, packed);
//                    cout << "Normalized int: " << FieldUtilities::printInt(field.getValue<Integer>()) << "\n"
//                        <<  " to             " << FieldUtilities::printInt(tmp.getValue<Integer>()) << ", dir=" << DataUtilities::printSortDirection(dir) <<  endl;
//                    return tmp;
                    return normalizeInt(field, dir, packed);
                }
                case FieldType::SECURE_FLOAT: {
                    auto tmp = normalizeFloat(field, dir);
                    cout << "Normalized float " << FieldUtilities::printField(field) << "\n"
                         << "              to " << FieldUtilities::printField(tmp) << endl;
                    return tmp;
//                    return normalizeFloat(field, dir);
                }
                case FieldType::SECURE_STRING:
                    return normalizeString(field, dir);
                default:
                    throw std::runtime_error("Unsupported field type");
            }
        }

        static SecureField denormalize(const  SecureField &field,  const FieldType & dst_type, const SortDirection & dir, const bool & packed = false) {
            switch(dst_type) {
                case FieldType::SECURE_BOOL:
                    return (dir == SortDirection::ASCENDING) ? field : !field;
                case FieldType::SECURE_INT:
                case FieldType::SECURE_LONG:
                    return denormalizeInt(field, dir, packed);
                case FieldType::SECURE_FLOAT: {
                    auto tmp = denormalizeFloat(field, dir);
                    cout << "Denormalized float " << FieldUtilities::printField(field) << "\n"
                         << "                to " << FieldUtilities::printField(tmp) << endl;
                    return tmp;
//                    return denormalizeFloat(field, dir);
                }
                case FieldType::SECURE_STRING:
                    return normalizeString(field, dir); // reuse normalizeString - it does the same thing here
                default:
                    throw std::runtime_error("Unsupported field type");
            }
        }

        private:

            template<typename T>
            static PlainField normalizeInt(const  PlainField &field, const SortDirection & dir) {
               T src = field.getValue<T>();
               T dst = src;
               if(dir == SortDirection::DESCENDING) {
                     // invert the sign bit
                     dst = -dst;
            }

               FieldType dst_type = (field.getType() == FieldType::LONG) ? FieldType::LONG : FieldType::INT;
               return PlainField(dst_type, dst);
            }

        template<typename T>
        static PlainField denormalizeInt(const  PlainField &field, const SortDirection & dir) {
            T src = field.getValue<T>();
            T dst = src;
            if(dir == SortDirection::DESCENDING) {
                // invert the sign bit
                dst = -dst;
            }

            FieldType dst_type = (field.getType() == FieldType::LONG) ? FieldType::LONG : FieldType::INT;
            return PlainField(dst_type, dst);
        }

        // from https://stackoverflow.com/questions/33678827/compare-floating-point-numbers-as-integers
            static PlainField normalizeFloat(const PlainField & field, const SortDirection & dir) {
                float_t f = field.getValue<float_t>();
                uint32_t bits;
            cout << "Normalizing float " << FieldUtilities::printFloat(f) << ": " << endl;
            if(dir == SortDirection::DESCENDING) {
                    f = -f;
                }
            cout << " after DESC: " << FieldUtilities::printFloat(f) << endl;

            memcpy(&bits, &f, sizeof(float_t));


            const uint32_t sign_bit = bits & 0x80000000ul;
            cout << " sign bit: " << sign_bit << endl;

            if (sign_bit) {
                bits = (0x7FFFFFF  | 0x80000000ul) - bits;
                cout << " after flipping bits: " << FieldUtilities::printInt(bits) << endl;
//                int test = (0x7FFFFFFF | 0x80000000ul)  - bits;
//                cout << "  Test: " << FieldUtilities::printInt(test) << endl;
                // stash the sign bit in the last bit - this causes a minor loss in precision for normalized comparisons
//                cout << "  Bit mask: " << FieldUtilities::printInt(0x80000000ul) << endl;
//                bits |=  0x00000001ul;
//                cout << "  After reinstating the sign bit: " << FieldUtilities::printInt(bits) << endl;
            }

            // reverse byte order
//            int32_t dst;
//            int8_t *dst_ptr = (int8_t *) &dst;
//            for(int i = 0; i < 4; ++i) {
//                *dst_ptr =  ((int8_t *) &bits)[3 - i];
//                ++dst_ptr;
//            }
//            cout << " after reversing bytes: " << FieldUtilities::printInt(dst) << endl;

            return PlainField(FieldType::INT, (int32_t) bits);
        }

        static PlainField denormalizeFloat(const PlainField & field, const SortDirection & dir) {
            int32_t bits = field.getValue<int32_t>();
            cout << "Denorming int: " << FieldUtilities::printInt(bits) << ": " << endl;

//            int32_t tmp = bits;
//            // reverse byte order
//            int8_t *bits_ptr = (int8_t *) &bits;
//            for(int i = 0; i < 4; ++i) {
//                *bits_ptr =  ((int8_t *) &tmp)[3 - i];
//                ++bits_ptr;
//            }
//
//            bits = tmp;
//            cout << "After byte reversal: " << FieldUtilities::printInt(bits) << endl;

            const bool sign_bit = (bits & 0x80000000ul); // last bit is 1
            cout << " sign bit: " << sign_bit << endl;


            if (sign_bit) {
//                bits =  0x7FFFFFF  - bits;
                bits = (0x7FFFFFF  | 0x80000000ul) - bits;

                cout << " after flipping bits: " << FieldUtilities::printInt(bits) << endl;
            }

            float_t dst;
            memcpy(&dst, &bits, sizeof(float_t));
            cout << " to float: " << FieldUtilities::printFloat(dst) << endl;
            if(dir == SortDirection::DESCENDING) {
                dst = -dst;
            }
            cout << " after DESC: " << FieldUtilities::printFloat(dst) << endl;


            return PlainField(FieldType::FLOAT, dst);
        }

       static PlainField normalizeString(const PlainField & field, const SortDirection & dir) {
            std::string src = field.getValue<std::string>();
            std::string dst = src;
//            std::reverse(dst.begin(), dst.end());

            if (dir == SortDirection::DESCENDING) {
                // invert bytes with XOR
                for(int i = 0; i < dst.size(); ++i) {
                    dst[i] = dst[i] ^ 0xFF;
                }
            }
            return PlainField(FieldType::STRING, dst, field.string_length_);
        }

        // ***END PLAIN FIELD SUPPORT

        // ***START MPC FIELD SUPPORT
        static SecureField normalizeInt(const SecureField & s, const SortDirection & dir, const bool & packed) {
            Integer dst = s.getValue<Integer>();
            if (dir == SortDirection::DESCENDING) {
                if(packed) {
                    // unsigned --> invert bits
                    // start at 1 to ignore the sign bit
                    for(int i = 0; i < dst.size()-1; ++i) {
                        dst[i] = !dst[i];
                    }
                }
                else { // keep sign bit
                    // invert the sign bit
                    dst = -dst;
                }
            }


            FieldType dst_type = (s.getType() == FieldType::SECURE_LONG) ? FieldType::SECURE_LONG : FieldType::SECURE_INT;
            return SecureField(dst_type, dst);
        }

        static SecureField denormalizeInt(const SecureField & s, const SortDirection & dir, const bool & packed) {
            Integer dst = s.getValue<Integer>();
            if(dir == SortDirection::DESCENDING) {
                if(packed) {
                    // start at 1 to ignore the sign bit
                    for(int i = 0; i < dst.size()-1; ++i) {
                        dst[i] = !dst[i];
                    }
                }
                else {// invert the sign bit
                    dst = -dst;
                }
            }


            FieldType dst_type = (s.getType() == FieldType::SECURE_LONG) ? FieldType::SECURE_LONG : FieldType::SECURE_INT;
            return SecureField(dst_type, dst);

        }


        // same as above, but for secure fields
        static SecureField normalizeFloat(const SecureField & field, const SortDirection & dir) {
            Float f = field.getValue<Float>();
            cout << "Normalizing float " << f.reveal<double>() << endl;

            if(dir == SortDirection::DESCENDING) {
                f = -f;
            }
            cout << " after DESC: " << FieldUtilities::printFloat(f) << endl;

            Bit sign_bit = f[FLOAT_LEN - 1];
            Integer ones(32, 0x7FFFFFF);
            Integer bits(ones);
            memcpy(bits.bits.data(), f.value.data(), FLOAT_LEN * TypeUtilities::getEmpBitSize());

            bits = emp::If(sign_bit, ones - bits, bits);
            cout << " after flipping bits: " << FieldUtilities::printInt(bits) << endl;

//            vector<Bit> tmp(32);
//            // reverse byte order
//            memcpy(tmp.data(), bits.bits.data() + 24, 8 * TypeUtilities::getEmpBitSize());
//            memcpy(tmp.data() + 8, bits.bits.data() + 16, 8 * TypeUtilities::getEmpBitSize());
//            memcpy(tmp.data() + 16, bits.bits.data() + 8, 8 * TypeUtilities::getEmpBitSize());
//            memcpy(tmp.data() + 24, bits.bits.data(), 8 * TypeUtilities::getEmpBitSize());
//
//            Integer dst(tmp);
//            cout << " after reversing bytes: " << FieldUtilities::printInt(dst) << endl;
//            return SecureField (FieldType::SECURE_INT, dst);

    return SecureField (FieldType::SECURE_INT, bits);
        }

        static SecureField denormalizeFloat(const SecureField & field, const SortDirection & dir) {
            Integer bits = field.getValue<Integer>();
            cout << "Denormalizing: " << FieldUtilities::printInt(bits) << endl;
//            // reverse byte order
//            vector<Bit> tmp(32);
//            memcpy(tmp.data(), bits.bits.data() + 24, 8 * TypeUtilities::getEmpBitSize());
//            memcpy(tmp.data() + 8, bits.bits.data() + 16, 8 * TypeUtilities::getEmpBitSize());
//            memcpy(tmp.data() + 16, bits.bits.data() + 8, 8 * TypeUtilities::getEmpBitSize());
//            memcpy(tmp.data() + 24, bits.bits.data(), 8 * TypeUtilities::getEmpBitSize());
//
//            bits.bits = tmp;
//            cout << " after reversing bytes: " << FieldUtilities::printInt(bits) << endl;

            const Bit sign_bit = !(bits.bits[FLOAT_LEN - 1]); // first bit is zero
            cout << " sign bit: " << sign_bit.reveal() << endl;

            Integer ones(32, 0x7FFFFFF);
            bits = emp::If(sign_bit, ones - bits, bits);
            cout << " after flipping bits: " << FieldUtilities::printInt(bits) << endl;


            Float dst;
            memcpy(dst.value.data(), bits.bits.data(), FLOAT_LEN * TypeUtilities::getEmpBitSize());
            cout << " to float: " << FieldUtilities::printFloat(dst) << endl;
            if(dir == SortDirection::DESCENDING) {
                dst = -dst;
            }
            cout << " after DESC: " << FieldUtilities::printFloat(dst) << endl;


            return SecureField (FieldType::SECURE_FLOAT, dst);

        }

        static SecureField normalizeString(const SecureField & field, const SortDirection & dir) {
            Integer src = field.getValue<Integer>();
            Integer dst = src;
            int8_t *src_ptr, *dst_ptr;
            // reverse byte order
//            for(int i = 0; i < src.size()/8; ++i) {
//                dst_ptr = ((int8_t *) dst.bits.data()) + i * 8 * TypeUtilities::getEmpBitSize();
//                src_ptr = ((int8_t *) src.bits.data()) + (src.size() - (i + 1) * 8) * TypeUtilities::getEmpBitSize();
//                memcpy(dst_ptr, src_ptr, 8 * TypeUtilities::getEmpBitSize());
//            }


            if (dir == SortDirection::DESCENDING) {
                // invert the bits
                for(int i = 0; i < dst.size(); ++i) {
                    dst[i] = !dst[i];
                }
            }

            return SecureField(FieldType::SECURE_STRING, dst, field.string_length_);
        }

        // ***END MPC FIELD SUPPORT
    };

}

#endif

