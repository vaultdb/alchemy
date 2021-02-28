#ifndef BOOL_FIELD_H
#define BOOL_FIELD_H

#include "field_instance.h"


namespace vaultdb {


        class BoolField : public FieldInstance<BoolField, BoolField, bool, bool> {

        public:

            BoolField() : FieldInstance<BoolField, BoolField, bool, bool>() {}
            BoolField(const BoolField & src) : FieldInstance<BoolField, BoolField, bool, bool>(src)
            { payload = src.payload; }
            BoolField(const int32_t & src) : FieldInstance<BoolField, BoolField, bool, bool>() { payload = src; }

            BoolField& operator=(const BoolField& other) {
                this->payload = other.payload;
                return *this;
            }


            void copy(const BoolField & src) {payload = src.payload; }
            void assign(const int32_t & src) {payload = src; }

            static FieldType type() { return FieldType::BOOL; }
            size_t size() const { return 8; }

            Field *decrypt() const { return new BoolField(*this); }


            static BoolField deserialize(const int8_t *cursor) {
                BoolField ret;
                memcpy((int8_t *) &ret.payload, cursor, ret.size()/8);
                return ret;

            }

            int32_t primitive() const { return payload; }
            std::string str() const { return std::to_string(payload); }


            BoolField  operator!() const {
                return !payload;
            }


            BoolField & geq(const BoolField & rhs) const {

                return BoolField(payload >= rhs.payload);
            }


            BoolField & equal(const BoolField & rhs) const {
                return  BoolField(payload == rhs.payload);
            }

        protected:
            bool payload;

        };
}
#endif //BOOL_FIELD_H
