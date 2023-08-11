#ifndef _NORMALIZE_FIELDS_H_
#define _NORMALIZE_FIELDS_H_

#include <query_table/field/field.h>
#include <util/data_utilities.h>
#include "util/type_utilities.h"
#include "util/field_utilities.h"

#ifndef FLOAT_LEN
#define FLOAT_LEN 32
#endif

// TODO: move this into Field.h/cpp or utils? or Sort.h/cpp?
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
                case FieldType::FLOAT:
                     return normalizeFloat(field, dir);
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
                case FieldType::SECURE_LONG:
                    return normalizeInt(field, dir, packed);
                case FieldType::SECURE_FLOAT:
                    return normalizeFloat(field, dir);
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
                case FieldType::SECURE_FLOAT:
                    return denormalizeFloat(field, dir);
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

            if(dir == SortDirection::DESCENDING) {
                    f = -f;
                }

            memcpy(&bits, &f, sizeof(float_t));


            const uint32_t sign_bit = bits & 0x80000000ul;

            if (sign_bit) {
                bits = (0x7FFFFFF  | 0x80000000ul) - bits;
            }


            return PlainField(FieldType::INT, (int32_t) bits);
        }

        static PlainField denormalizeFloat(const PlainField & field, const SortDirection & dir) {
            int32_t bits = field.getValue<int32_t>();
            const bool sign_bit = (bits & 0x80000000ul); // last bit is 1

            if (sign_bit) {
                // OR with sign bit to retain sign bit
                // this results in a minor loss of precision for the sort comparisons alone
                // but it is needed to reverse the process later
                bits = (0x7FFFFFF  | 0x80000000ul) - bits;
            }

            float_t dst;
            memcpy(&dst, &bits, sizeof(float_t));

            if(dir == SortDirection::DESCENDING) {
                dst = -dst;
            }

            return PlainField(FieldType::FLOAT, dst);
        }

       static PlainField normalizeString(const PlainField & field, const SortDirection & dir) {
            std::string src = field.getValue<std::string>();
            std::string dst = src;

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
//            cout << "Normalizing float "  << FieldUtilities::printFloat(f) << endl;
            if(dir == SortDirection::DESCENDING) {
                f = -f;
            }
//            cout << "After negation: " << FieldUtilities::printFloat(f) << endl;
            Bit sign_bit = f[FLOAT_LEN - 1];
//            cout << "sign bit: " << sign_bit.reveal() << endl;

            Integer ones(32, (0x7FFFFFF  | 0x80000000ul));
            Integer bits(ones);
            memcpy(bits.bits.data(), f.value.data(), FLOAT_LEN * TypeUtilities::getEmpBitSize());
//            cout << "bits: " << FieldUtilities::printInt(bits) << endl;
            bits = emp::If(sign_bit, ones - bits, bits);
//            cout << " after flipping bits: " << FieldUtilities::printInt(bits) << endl;

            return SecureField (FieldType::SECURE_INT, bits);
        }

        static SecureField denormalizeFloat(const SecureField & field, const SortDirection & dir) {
            Integer bits = field.getValue<Integer>();
//            cout << "Denormalizing int: " << FieldUtilities::printInt(bits) << endl;

            const Bit sign_bit = (bits.bits[FLOAT_LEN - 1]); // first bit is zero
//            cout << "Sign bit: " << sign_bit.reveal() << endl;

            Integer ones(32, (0x7FFFFFF  | 0x80000000ul));
            bits = emp::If(sign_bit, ones - bits, bits);
//            cout << "After flipping bits: " << FieldUtilities::printInt(bits) << endl;

            Float dst;
            memcpy(dst.value.data(), bits.bits.data(), FLOAT_LEN * TypeUtilities::getEmpBitSize());
//            cout << "To float: " << FieldUtilities::printFloat(dst) << endl;
            if(dir == SortDirection::DESCENDING) {
                dst = -dst;
            }

//            cout << "After negation: " << FieldUtilities::printFloat(dst) << endl;
            return SecureField (FieldType::SECURE_FLOAT, dst);

        }

        static SecureField normalizeString(const SecureField & field, const SortDirection & dir) {
            Integer dst = field.getValue<Integer>();

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

