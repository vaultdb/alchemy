#ifndef EMP_MANAGER_H
#define EMP_MANAGER_H


#include <vaultdb.h>
#include <emp-sh2pc/emp-sh2pc.h>
#include "query_table/field/secure_bool_field.h"
#include "query_table/field/bool_field.h"
#include "query_table/field/secure_int_field.h"
#include "query_table/field/int_field.h"
#include "query_table/field/secure_long_field.h"
#include "query_table/field/long_field.h"
#include "query_table/field/secure_float_field.h"
#include "query_table/field/float_field.h"
#include "query_table/field/secure_string_field.h"
#include "query_table/field/string_field.h"


namespace  vaultdb {
    class EmpManager {
    public:

        static std::shared_ptr<QueryTable> secretShareTable(const QueryTable *srcTable, NetIO *netio, int party);

        static QueryTuple
        secretShareTuple(QueryTuple *srcTuple, const QuerySchema *schema, const int &myParty, const int &dstParty);

        static QueryField
        secretShareField(const QueryField *srcField, const types::TypeId &type, const int32_t &ordinal,
                         size_t length, const int &myParty, const int &dstParty);

        static types::Value
        secretShareValue(const types::Value &srcValue, const types::TypeId &type, size_t length, const int &myParty,
                         const int &dstParty);

        static emp::Integer
        encryptVarchar(std::string input, size_t stringBitCount, const int &myParty, const int &dstParty);

        static emp::Float castIntToFloat(const emp::Integer & input);

        static Field *encryptField(const Field * srcField, const int & myParty, const int & dstParty) {
            FieldType type = srcField->getType();
            if(type == FieldType::BOOL) {
                BoolField *src = (BoolField *) srcField;
                bool toEncrypt = src->primitive();
                return new SecureBoolField(toEncrypt, myParty, dstParty);
            }
            else if(type == FieldType::INT32) {
                IntField *src = (IntField *) srcField;
                int32_t toEncrypt = src->primitive();
                return new SecureIntField(toEncrypt, myParty, dstParty);
            }
            else if(type == FieldType::INT64) {
                LongField *src = (LongField *) srcField;
                int64_t toEncrypt = src->primitive();
                return new SecureLongField(toEncrypt, myParty, dstParty);
            }
            else if(type == FieldType::FLOAT32) {
                FloatField *src = (FloatField *) srcField;
                float_t toEncrypt = src->primitive();
                return new SecureFloatField(toEncrypt, myParty, dstParty);
            }


            return nullptr;

        }

        static Field *revealField(const Field * srcField, const int & party) {
            FieldType type = srcField->getType();
            if(type == FieldType::SECURE_BOOL) {
                SecureBoolField *src = (SecureBoolField *) srcField;
                emp::Bit toReveal = src->primitive();
                return new BoolField(toReveal, party);
            }
            else if(type == FieldType::SECURE_INT32) {
                SecureIntField *src = (SecureIntField *) srcField;
                emp::Integer toReveal = src->primitive();
                return new IntField(toReveal, party);
            }
            else if(type == FieldType::SECURE_INT64) {
                SecureLongField *src = (SecureLongField *) srcField;
                emp::Integer toReveal = src->primitive();
                return new LongField(toReveal, party);
            }
            else if(type == FieldType::SECURE_FLOAT32) {
                SecureFloatField *src = (SecureFloatField *) srcField;
                emp::Float toReveal = src->primitive();
                return new FloatField(toReveal, party);
            }
            else if(type == FieldType::SECURE_STRING) {
                SecureStringField *src = (SecureStringField *) srcField;
                emp::Integer toReveal = src->primitive();
                return new StringField(toReveal, party);
            }

            return nullptr;

        }


    };

}

#endif //_EMP_UTILITIES_H
