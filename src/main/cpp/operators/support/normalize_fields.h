#ifndef _NORMALIZE_FIELDS_H_
#define _NORMALIZE_FIELDS_H_

#include <query_table/field/field.h>
#include <util/data_utilities.h>

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
                    if (dir == SortDirection::DESCENDING) {
                        DataUtilities::reverseBytes((int8_t *) src.c_str(), (int8_t *) dst.c_str(), src.size());
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
                    if (dir == SortDirection::DESCENDING) {
                        DataUtilities::reverseBytes((int8_t *) src.c_str(), (int8_t *) dst.c_str(), src.size());
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
                     src = -src;
                     DataUtilities::reverseBytes((int8_t *) &src, (int8_t *)  &dst, sizeof(T));
               }
               FieldType dst_type = (field.getType() == FieldType::LONG) ? FieldType::LONG : FieldType::INT;
               return PlainField(dst_type, dst);
            }

        template<typename T>
        static PlainField denormalizeInt(const  PlainField &field, const SortDirection & dir) {
            T src = field.getValue<T>();
            T dst = src;
            if(dir == SortDirection::DESCENDING) {
                DataUtilities::reverseBytes((int8_t *) &src, (int8_t *)  &dst, sizeof(T));
                // invert the sign bit
                dst = -dst;
            }
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
                bool sign_bit = (f < 0.0);
               //  const uint32_t sign_bit = bits & 0x80000000ul; // collects first bit alone
                cout << "Input bits: " << DataUtilities::printBitArray((int8_t * ) &bits, sizeof(uint32_t)) << ", ";

                cout << "sign bit: " << sign_bit << ", ";
                if (sign_bit) {
                    bits = 0x7FFFFFF - bits;
                }
                cout << "output bits: " << DataUtilities::printBitArray((int8_t * ) &bits, sizeof(uint32_t)) << "\n";

                PlainField res(FieldType::INT, bits);
                PlainField check = denormalizeFloat(res, dir);
                cout << "Original: " << field << " Normalized: " << res << " Denormalized: " << check << endl;
                //assert(check == res);

                return res;
            }


        static PlainField denormalizeFloat(const PlainField & field, const SortDirection & dir) {
            int32_t bits = field.getValue<int32_t>();
            cout << "Input bits: " << DataUtilities::printBitArray((int8_t * ) &bits, sizeof(uint32_t)) << ", ";
            const bool sign_bit = !(bits & 0x80000000ul); // formerly & , 1 should be getting last bit
            cout << "sign bit: " << sign_bit << ", ";
            if (sign_bit) {
                bits = 0x7FFFFFF - bits;
            }
            cout << "output bits: " << DataUtilities::printBitArray((int8_t * ) &bits, sizeof(uint32_t)) << "\n";

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
            Float input = field.getValue<Float>();
            Integer bits(32, 0);
            memcpy(&(bits.bits[0]), &(input.value[0]), 32 * sizeof(Bit));

            Bit sign_bit = bits.bits[31];
            int32_t reset = 0x7FFFFFFF;
            Integer reset_i(32, reset, PUBLIC);
            Integer tmp = reset_i - bits;
            bits = emp::If(sign_bit, tmp, bits);

            SecureField res(FieldType::SECURE_INT, bits);
            return normalizeInt(res, dir);
        }


        static SecureField denormalizeFloat(const SecureField & field, const SortDirection & dir) {
            SecureField denormed_int = denormalizeInt(field, dir);
            Integer bits = denormed_int.getValue<Integer>();

            Bit sign_bit = bits.bits[31];
            int32_t reset = 0x7FFFFFFF;
            Integer reset_i(32, reset, PUBLIC);
            Integer tmp = reset_i - bits;
            bits = emp::If(sign_bit, tmp, bits);

            Float dst;

            memcpy(dst.value.data(), bits.bits.data(), FLOAT_LEN * TypeUtilities::getEmpBitSize());
            return SecureField(FieldType::SECURE_FLOAT, dst);
        }


    };

}

#endif

