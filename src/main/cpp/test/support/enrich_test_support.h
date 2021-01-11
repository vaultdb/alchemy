#ifndef _ENRICH_TEST_SUPPORT_H
#define _ENRICH_TEST_SUPPORT_H

#include <operators/project.h>
#include <query_table/types/value.h>
#include <operators/support/predicate.h>

using namespace vaultdb;
using namespace vaultdb::types;

// container for projection expressions, filter expressions, etc.

class EnrichTestSupport {

public:

        static Value projectSecureAgeStrata(const QueryTuple & aTuple);

        static Value projectPlainAgeStrata(const QueryTuple &aTuple);

        static Value projectAgeStrata(const QueryTuple &aTuple);


        static emp::Integer getEmpInt(const int32_t &value);

    static Value projectMultisite(const QueryTuple &aTuple);

    static Value projectNumeratorMultisite(const QueryTuple &aTuple);

    static QuerySchema getPatientSchema();

    static QuerySchema getPatientInclusionSchema();
};



class FilterExcludedPatients : public Predicate {

    Value cmp;
public:
    explicit FilterExcludedPatients(const bool & isEncrypted) {
        cmp = isEncrypted ? TypeUtilities::getZero(TypeId::ENCRYPTED_INTEGER32) : TypeUtilities::getZero(TypeId::INTEGER32);
    }

    ~FilterExcludedPatients() = default;
    [[nodiscard]] types::Value predicateCall(const QueryTuple & aTuple) const override {

        QueryTuple decrypted = aTuple.reveal();
        Value field = aTuple.getFieldPtr(8)->getValue();
        Value res = (field == cmp);
        return  res;
    }


};


#endif //_ENRICH_TEST_SUPPORT_H
