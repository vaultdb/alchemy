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

        static Field projectSecureAgeStrata(const QueryTuple & aTuple);

        static Field projectPlainAgeStrata(const QueryTuple &aTuple);

        static Field projectAgeStrata(const QueryTuple &aTuple);


        static emp::Integer getEmpInt(const int32_t &value);

    static Field projectMultisite(const QueryTuple &aTuple);

    static Field projectNumeratorMultisite(const QueryTuple &aTuple);

    static QuerySchema getPatientSchema();

    static QuerySchema getPatientInclusionSchema();
};



class FilterExcludedPatients : public Predicate {

    Field cmp;
public:
    explicit FilterExcludedPatients(const bool & isEncrypted) {
        cmp = isEncrypted ? TypeUtilities::getZero(FieldType::SECURE_INT) : TypeUtilities::getZero(FieldType::INT);
    }

    ~FilterExcludedPatients() = default;
    [[nodiscard]] Field predicateCall(const QueryTuple & aTuple) const override {

        QueryTuple decrypted = aTuple.reveal();
        Field field = aTuple.getFieldPtr(8)->getValue();
        Field res = (field == cmp);
        return  res;
    }


};


#endif //_ENRICH_TEST_SUPPORT_H
