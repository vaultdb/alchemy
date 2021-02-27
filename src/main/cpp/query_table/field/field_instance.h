#ifndef _QUERYFIELD_H
#define _QUERYFIELD_H

// generic query field API for all field types, e.g., ints or floats
// analogous to emp's Comparable
// formerly known as Value

#include "field.h"





namespace vaultdb {


    // This curiously recurring template pattern class implements clone() for derived types
    // T = derived class
    // P = underlying primitive of T
    // R = revealed class type - after decrypting
    // B = boolean type, bool for plaintext, emp::Bit for secure
    template<typename T, typename P, typename  R, typename B>
    class FieldInstance : public Field {
    public:


        std::unique_ptr<Field> clone() const override {
            return std::make_unique<T>(static_cast<T const &>(*this));
        }

        P getValue() const {
            return static_cast<T const &>(*this).primitive();
        }

        std::string toString() const override {
            return static_cast<T const &>(*this).str();
        }

        R reveal() const {
            return static_cast<T const &>(*this).decrypt();
        }

    protected:
        void copyTo(const Field & other) override {
            const T otherObj = static_cast<const T &>(other);
            static_cast<T &>(*this).copy(otherObj);
        }

       // assignment
        Field & operator=(const Field& other) override {
            copyTo(other);
            return *this;
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

        Field operator+(const Field &rhs) const {
            return static_cast<const T*>(this) + static_cast<const T>(rhs);
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