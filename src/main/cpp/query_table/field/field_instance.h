#ifndef _FIELD_INSTANCE_H
#define _FIELD_INSTANCE_H

#include <string>
// JMR: this is one long, elaborate hack to get around needing the template type (e.g., IntField, FloatField) in Field declaration
// this is necessary to store heterogenous fields in the same container/tuple.
namespace vaultdb {

    class BoolField;
    class SecureBoolField;

    template<typename B>
    class FieldInstance {
    public:
        
        FieldInstance()  {}
        ~FieldInstance () {}

        // this will work if we only return fields
        // field will call plus, minus, etc.

        virtual FieldInstance<B> plus(const FieldInstance<B> &rhs) const  = 0;
        virtual FieldInstance<B> minus(const FieldInstance<B> &rhs) const = 0;
        virtual FieldInstance<B> multiply(const FieldInstance<B> &rhs) const = 0;
        virtual FieldInstance<B> div(const FieldInstance<B> &rhs) const = 0;
        virtual FieldInstance<B> modulus(const FieldInstance<B> &rhs) const = 0;


        // private assignment
        virtual FieldInstance<B> select(const B & choice, const FieldInstance<B> & other) const = 0;

        static FieldInstance<B> If(const B & sel, const FieldInstance<B> & lhs, const FieldInstance<B> & rhs) {
            
            return lhs.select(sel, rhs);
        }


        static void compareAndSwap(const B & select, FieldInstance<B> & lhs, FieldInstance<B> & rhs) {
            FieldInstance<B> t  = If(select, lhs, rhs);
            lhs = If(select, rhs, lhs);
            rhs = t;

        }
        

        // connectors
        virtual FieldInstance<B> operator&(const FieldInstance<B> &right) const  = 0;
        virtual FieldInstance<B> operator^(const FieldInstance<B> &right) const  = 0;
        virtual FieldInstance<B> operator|(const FieldInstance<B> &right) const  = 0;


        // setup for comparators
        //virtual B  geq(const FieldInstance<B> & rhs) const = 0;
        //virtual B equal(const FieldInstance<B> & rhs) const = 0;
        virtual std::string toString() const = 0;

        virtual B operator>=(const FieldInstance<B> & rhs) const = 0;
        virtual B operator==(const FieldInstance<B> & rhs) const = 0;
        virtual B operator!=(const FieldInstance<B> & rhs) const = 0;
        virtual B operator<(const FieldInstance<B> & rhs) const = 0;
        virtual B operator<=(const FieldInstance<B> & rhs) const = 0;
        virtual B operator>(const FieldInstance<B> & rhs) const = 0;

        virtual void serialize(int8_t *dst) const = 0;
        // encrypt and decrypt -- table this for now
        //Field<BoolField> reveal(const int &party) const;
        //static Field<SecureBoolField> secretShare(const Field<BoolField> & field, const FieldType &type, const size_t &strLength, const int &myParty,
         //                               const int &dstParty);




    };

}

template class vaultdb::FieldInstance<vaultdb::BoolField>;
template class vaultdb::FieldInstance<vaultdb::SecureBoolField>;




#endif //_FIELD_INSTANCE_H
