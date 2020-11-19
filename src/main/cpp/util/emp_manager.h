#ifndef EMP_MANAGER_H
#define EMP_MANAGER_H


#include <vaultdb.h>
#include <emp-sh2pc/emp-sh2pc.h>

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

    };

}

#endif //_EMP_UTILITIES_H
