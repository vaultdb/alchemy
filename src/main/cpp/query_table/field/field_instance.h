#ifndef _QUERYFIELD_H
#define _QUERYFIELD_H

// generic query field API for all field types, e.g., ints or floats
// analogous to emp's Comparable
// formerly known as Value

#include "field.h"
#include "field_factory.h"


namespace vaultdb {


    // This curiously recurring template pattern class implements clone() for derived types
    //  need two of these: field instance, field primitive to make this more manageable
    // will have:
    /*
     *         Field
     *           |
     *           v
     *        FieldInstance<Derived,Plaintext>        FieldPrimitive<Primitive,Revealed,Bool>
     *                  \                                    /
     *                   \                                 /
     *                           IntField, FloatField
     *
     *     Do we really need FieldPrimitive yet?  Not yet, but will eventually
     *     Taking out:
     *          P = underlying primitive of T
                R = revealed primitive type - after decrypting
                B = boolean type, bool for plaintext, emp::Bit for secure

     */
    // T = derived field
    // R = revealed field
    // B = boolean field result
    // P = primitive / payload of field, needed for serialize
    template<typename T, typename R, typename B, typename P>
    class FieldInstance : public Field {
    public:


        std::unique_ptr<Field> clone() const override {
            return std::make_unique<T>(static_cast<T const &>(*this));
        }

        P getValue() const {
            return static_cast<T const &>(*this).primitive();
        }

        // TODO: these methods need to be in header file to compile
        // figure out why later - may need inline?
        std::string toString() const override {
            return static_cast<T const &>(*this).str();
        }

        void serialize(int8_t *dst) const override {
            P payload = getValue();
            size_t size  = static_cast<T const &>(*this).size() / 8; // bits --> bytes
            memcpy(dst, (int8_t *) payload, size);

        }

        Field  *reveal() const override {
            return static_cast<T const &>(*this).decrypt();
        }






        // assignment
        FieldInstance & operator=(const FieldInstance & other)  {
            copyTo(other);
            return *this;
        }

        Field & operator+(const Field &rhs) const override {
            const T otherObj = static_cast<const T &>(rhs);
            const T impl = static_cast<T const &>(*this);

            return impl + otherObj;
        }

         Field & geq(const Field & rhs) const override {
            return static_cast<T const &>(*this) >= (static_cast<const T &>(rhs));
        }

        



    protected:
        void copyTo(const Field & other) override {
            const T otherObj = static_cast<const T &>(other);
            static_cast<T &>(*this).copy(otherObj);
        }

/*
        // handle expressions
        //comparators based on emp-toolkit
        B operator>=(const Field &rhs) const {
            return static_cast<const T*>(this)->geq(static_cast<const T>(rhs));
        }
        B operator<(const Field& rhs) const {
            return !( (*static_cast<const T*>(this))>= static_cast<const T>(rhs) );
        }

        B operator<=(const Field& rhs) const {
            return static_cast<const T>(rhs) >= *static_cast<const T*>(this);
        }

        B operator>(const Field& rhs) const {
            return !(static_cast<const T>(rhs) >= *static_cast<const T*>(this));
        }

        B operator==(const Field& rhs) const {
            return static_cast<const T*>(this)->equal(static_cast<const T>(rhs));
        }

        B operator!=(const Field & rhs) const {
            return !(*static_cast<const T*>(this) == static_cast<const T>(rhs));
        }


        Field operator-(const Field &rhs) const {
            return static_cast<const T*>(this) - static_cast<const T>(rhs);
        }

        Field operator*(const Field &rhs) const {
            return static_cast<const T*>(this) * static_cast<const T>(rhs);
        }

        Field operator/(const Field &rhs) const {
            return static_cast<const T*>(this) / static_cast<const T>(rhs);
        }

        Field operator%(const Field &rhs) const {
            return static_cast<const T*>(this) % static_cast<const T>(rhs);
        }

        Field operator&(const Field &rhs) const {
            return static_cast<const T*>(this) & static_cast<const T>(rhs);
        }

        Field operator|(const Field &rhs) const {
            return static_cast<const T*>(this) | static_cast<const T>(rhs);
        }

        Field operator^(const Field &rhs) const {
            return static_cast<const T*>(this) ^ static_cast<const T>(rhs);
        } */

    protected:
        // set up vtable entries
        FieldInstance() = default;

        FieldInstance(const FieldInstance &) = default;

        FieldInstance(FieldInstance &&) = default;
    };




}


#endif