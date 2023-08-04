#ifndef _NORMALIZE_FIELDS_H_
#define _NORMALIZE_FIELDS_H_

#include <query_table/field/field.h>
#include <util/data_utilities.h>
#include "util/type_utilities.h"

namespace vaultdb {
    class NormalizeFields {
    public:


        static PlainField normalize(const  PlainField &field, const SortDirection & dir) {
            switch(field.getType()) {
                case FieldType::BOOL:
                    return (dir == SortDirection::ASCENDING) ? field : !field;
                case FieldType::INT:
                    return normalizeInt<int32_t>(field, dir);
                case FieldType::LONG:
                    return normalizeInt<int64_t>(field, dir);
                case FieldType::FLOAT:
                   return normalizeFloat(field, dir);
                case FieldType::STRING: {
                    std::string src = field.getValue<std::string>();
                    std::string dst = src;
                    std::reverse(dst.begin(), dst.end());

                    if (dir == SortDirection::DESCENDING) {
                        // invert bytes with XOR
                        for(int i = 0; i < dst.size(); ++i) {
                            dst[i] = dst[i] ^ 0xFF;
                        }
                    }
                    PlainField res = field;
                    res.setValue(dst);
                    return res;
                }
                default:
                        throw std::runtime_error("Unsupported field type");
                }
            }


        // reverse the process
        static PlainField denormalize(const  PlainField &field, const FieldType & dst_type, const SortDirection & dir) {
            switch (dst_type) {
                case FieldType::BOOL:
                    return (dir == SortDirection::ASCENDING) ? field : !field;
                case FieldType::INT:
                    return denormalizeInt<int32_t>(field, dir);
                case FieldType::LONG:
                    return denormalizeInt<int64_t>(field, dir);
                case FieldType::FLOAT:
                    return denormalizeFloat(field, dir);
                case FieldType::STRING: {
                    std::string src = field.getValue<std::string>();
                    std::string dst = src;
                    std::reverse(dst.begin(), dst.end());

                    if (dir == SortDirection::DESCENDING) {
                        for(int i = 0; i < dst.size(); ++i) {
                            dst[i] = dst[i] ^ 0xFF;
                        }

                    }
                    PlainField res = field;
                    res.setValue(dst);
                    return res;
                }
                default:
                    throw std::runtime_error("Unsupported field type");
            }
        }

        static SecureField normalize(const  SecureField &field, const SortDirection & dir) {
            switch(field.getType()) {
                case FieldType::SECURE_BOOL:
                    return (dir == SortDirection::ASCENDING) ? field : !field;
                case FieldType::SECURE_INT:
                case FieldType::SECURE_LONG:
                    return normalizeInt(field, dir);
                case FieldType::SECURE_FLOAT:
                   return normalizeFloat(field, dir);
                case FieldType::SECURE_STRING: {
                    Integer src = field.getValue<Integer>();
                    Integer dst = src;
                    if(dir == SortDirection::DESCENDING) {
                        for(int i = 0; i < src.size(); ++i) {
                            dst[i] = src[src.size() - i - 1];
                        }
                    }
                    SecureField res = field;
                    res.setValue(dst);
                    return res;
                }
                default:
                    throw std::runtime_error("Unsupported field type");
            }
        }

        static SecureField denormalize(const  SecureField &field,  const FieldType & dst_type, const SortDirection & dir) {
            switch(dst_type) {
                case FieldType::SECURE_BOOL:
                    return (dir == SortDirection::ASCENDING) ? field : !field;
                case FieldType::SECURE_INT:
                case FieldType::SECURE_LONG:
                    return denormalizeInt(field, dir);
                case FieldType::SECURE_FLOAT:
                    return denormalizeFloat(field, dir);
                case FieldType::SECURE_STRING: {
                    Integer src = field.getValue<Integer>();
                    Integer dst = src;
                    if(dir == SortDirection::DESCENDING) {
                        for(int i = 0; i < src.size(); ++i) {
                            dst[i] = src[src.size() - i - 1];
                        }
                    }
                    SecureField res = field;
                    res.setValue(dst);
                    return res;
                }
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
//                int8_t *src_ptr = (int8_t *) &src;
//                int8_t *dst_ptr = (int8_t *) &dst;
//                for(int i = 0; i < sizeof(T); ++i) {
//                    dst_ptr[i] = src_ptr[sizeof(T) - i - 1];
//                }

//               DataUtilities::reverseBytes((int8_t *) &src, (int8_t *)  &dst, sizeof(T));
               FieldType dst_type = (field.getType() == FieldType::LONG) ? FieldType::LONG : FieldType::INT;
               return PlainField(dst_type, dst);
            }

        template<typename T>
        static PlainField denormalizeInt(const  PlainField &field, const SortDirection & dir) {
            T src = field.getValue<T>();
            T dst = src;
            if(dir == SortDirection::DESCENDING) {
              //
                // invert the sign bit
                dst = -dst;
            }
//            int8_t *src_ptr = (int8_t *) &src;
//            int8_t *dst_ptr = (int8_t *) &dst;
//            for(int i = 0; i < sizeof(T); ++i) {
//                dst_ptr[i] = src_ptr[sizeof(T) - i - 1];
//            }

//            DataUtilities::reverseBytes((int8_t *) &src, (int8_t *)  &dst, sizeof(T));
            FieldType dst_type = (field.getType() == FieldType::LONG) ? FieldType::LONG : FieldType::INT;
            return PlainField(dst_type, dst);
        }

        // from https://stackoverflow.com/questions/33678827/compare-floating-point-numbers-as-integers
            static PlainField normalizeFloat(const PlainField & field, const SortDirection & dir) {
                float_t f = field.getValue<float_t>();
                if(dir == SortDirection::DESCENDING) {
                    f = -f;
                }
                int32_t bits;
                memcpy(&bits, &f, sizeof(float_t));
               const uint32_t sign_bit = bits & 0x80000000ul; // collects (sign) first bit alone
                if (sign_bit) {
                    bits = 0x7FFFFFF - bits;
                }
              bits = 0x7FFFFFF - bits; // flip it a second time


                return PlainField(FieldType::INT, bits);
            }


        static PlainField denormalizeFloat(const PlainField & field, const SortDirection & dir) {
            int32_t bits = field.getValue<int32_t>();
            const bool sign_bit = !(bits & 0x80000000ul); // first bit is zero
            if (sign_bit) {
                bits = 0x7FFFFFF - bits;
            }

            float_t dst;
            memcpy(&dst, &bits, sizeof(float_t));

            if(dir == SortDirection::DESCENDING) {
                dst = -dst;
            }

            return PlainField(FieldType::FLOAT, dst);
        }

        // ***END PLAIN FIELD SUPPORT
        // ***START MPC FIELD SUPPORT
        static SecureField normalizeInt(const SecureField & s, const SortDirection & dir) {
            Integer src = s.getValue<Integer>();
            Integer dst = src;
            if(dir == SortDirection::DESCENDING) {
                // invert the sign bit
                src = -src;
                for(int i = 0; i < src.size(); ++i) {
                    dst[i] = src[src.size() - i - 1];
                }
            }
            FieldType dst_type = (s.getType() == FieldType::SECURE_LONG) ? FieldType::SECURE_LONG : FieldType::SECURE_INT;
            return SecureField(dst_type, dst);
        }

        static SecureField denormalizeInt(const SecureField & s, const SortDirection & dir) {
            Integer src = s.getValue<Integer>();
            Integer dst = src;
            if(dir == SortDirection::DESCENDING) {
                for(int i = 0; i < src.size(); ++i) {
                    dst[i] = src[src.size() - i - 1];
                }
                // invert the sign bit
                src = -src;
            }

            FieldType dst_type = (s.getType() == FieldType::SECURE_LONG) ? FieldType::SECURE_LONG : FieldType::SECURE_INT;
            return SecureField(dst_type, dst);

        }

        // same as above, but for secure fields
        static SecureField normalizeFloat(const SecureField & field, const SortDirection & dir) {
            Float f = field.getValue<Float>();
            if(dir == SortDirection::DESCENDING) {
                f = -f;
            }
            vector<Bit> bit_array(32);
            Integer bits(bit_array);
            memcpy(bits.bits.data(), f.value.data(), FLOAT_LEN * TypeUtilities::getEmpBitSize());
            Bit sign_bit = bits.bits[FLOAT_LEN - 1];

            Integer ones(32, 0x7FFFFFF);
            bits = emp::If(sign_bit, ones - bits, bits);
            bits = ones - bits; // flip it a second time


            return SecureField (FieldType::SECURE_INT, bits);
        }


        static SecureField denormalizeFloat(const SecureField & field, const SortDirection & dir) {
            Integer bits = field.getValue<Integer>();
            Bit sign_bit = !bits.bits[FLOAT_LEN - 1]; // bits are inverted for sorting
            Integer ones(32, 0x7FFFFFF);
            bits = emp::If(sign_bit, ones - bits, bits);

            Float dst;
            memcpy(&dst, &bits, FLOAT_LEN * TypeUtilities::getEmpBitSize());

            if(dir == SortDirection::DESCENDING) {
                dst = -dst;
            }

            return SecureField (FieldType::FLOAT, dst);
        }


    };

}

#endif

