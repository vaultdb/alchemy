#ifndef _FIELD_INSTANCE_H
#define _FIELD_INSTANCE_H

// JMR: this is one long, elaborate hack to get around needing the template type (e.g., IntField, FloatField) in Field declaration
// this is necessary to store heterogenous fields in the same container/tuple.
namespace vaultdb {
    template<typename B>
    class Field;

    class BoolField;
    class SecureBoolField;

    // FieldInstance, and the classes that inherit from it, contain no state of their own
    // decorators to add operator overloads
    template<typename B>
    class FieldInstance {
    public:

        // stateless, so this does not matter
        FieldInstance & operator=(const FieldInstance & other) { return *this;  }

        FieldInstance(Field <B> *src)  {}
        virtual ~FieldInstance () = default;

        // this will work if we only return fields
        // field will call plus, minus, etc.

        virtual Field<B> plus(const Field<B> &rhs) const  = 0;
        virtual Field<B> minus(const Field<B> &rhs) const = 0;
        virtual Field<B> multiply(const Field<B> &rhs) const = 0;
        virtual Field<B> div(const Field<B> &rhs) const = 0;
        virtual Field<B> modulus(const Field<B> &rhs) const = 0;


        // private assignment
        virtual Field<B> select(const B & choice, const Field<B> & other) const = 0;

        static Field<B> If(const B & sel, const Field<B> & lhs, const Field<B> & rhs) {

            FieldInstance<B> *lInstance = lhs.getInstance();

            return lInstance->select(sel, rhs);
        }


        static void compareAndSwap(const B & select, Field<B> & lhs, Field<B> & rhs) {
            Field<B> t  = If(select, lhs, rhs);
            lhs = If(select, rhs, lhs);
            rhs = t;

        }

        // connectors
        virtual Field<B> operator&(const Field<B> &right) const  = 0;
        virtual Field<B> operator^(const Field<B> &right) const  = 0;
        virtual Field<B> operator|(const Field<B> &right) const  = 0;


        // setup for comparators
        virtual B  geq(const Field<B> & rhs) const = 0;
        virtual B equal(const Field<B> & rhs) const = 0;
        virtual B negate() const = 0;
        virtual std::string str() const = 0;

        B operator !() const  { return this->negate(); }
        B operator>=(const Field<B> & rhs) const { return this->geq(rhs);  }
        B operator==(const Field<B> & rhs) const { return this->equal(rhs); }
        B operator!=(const Field<B> & rhs) const { return !(*this == rhs); }
        B operator<(const Field<B> & rhs) const { return !(*this >= rhs); }
        B operator<=(const Field<B> & rhs) const { return (*this < rhs) | (*this == rhs);  }
        B operator>(const Field<B> & rhs) const {
            B eq = this->equal(rhs);
            B geq = this->geq(rhs);
            return  geq & eq.neg();
        }

        virtual void serialize(int8_t *dst) const = 0;
        // encrypt and decrypt -- table this for now
        /*Field<BoolField> reveal(const int &party) const;
        static Field<SecureBoolField> secretShare(const Field<BoolField> & field, const FieldType &type, const size_t &strLength, const int &myParty,
                                        const int &dstParty); */




    };

}





#endif //_FIELD_INSTANCE_H
