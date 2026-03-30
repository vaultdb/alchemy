#ifndef EMP_MANAGER_H
#define EMP_MANAGER_H


#include <emp-sh2pc/emp-sh2pc.h>
#include "query_table/query_table.h"
#include "query_table/field/secure_bool_field.h"
#include "query_table/field/bool_field.h"
/*#include "query_table/field/secure_int_field.h"
#include "query_table/field/int_field.h"
#include "query_table/field/secure_long_field.h"
#include "query_table/field/long_field.h"
#include "query_table/field/secure_float_field.h"
#include "query_table/field/float_field.h"
#include "query_table/field/secure_string_field.h"
#include "query_table/field/string_field.h"*/


namespace  vaultdb {
    class EmpManager {
    public:

        static std::shared_ptr<QueryTable> secretShareTable(const QueryTable *srcTable, NetIO *netio, int party);

        static QueryTuple
        secretShareTuple(QueryTuple *srcTuple, const QuerySchema *schema, const int &myParty, const int &dstParty);

      /*  static QueryField
        secretShareField(const QueryField *srcField, const types::TypeId &type, const int32_t &ordinal,
                         size_t length, const int &myParty, const int &dstParty);

        static types::Value
        secretShareValue(const types::Value &srcValue, const types::TypeId &type, size_t length, const int &myParty,
                         const int &dstParty);*/

        static emp::Integer
        encryptVarchar(std::string input, size_t stringBitCount, const int &myParty, const int &dstParty);

        static emp::Float castIntToFloat(const emp::Integer & input);

        static Field *secretShareField(const Field * srcField, const int & myParty, const int & dstParty);

        static Field *revealField(const Field * srcField, const int & party);

    };

}

#endif //_EMP_UTILITIES_H
