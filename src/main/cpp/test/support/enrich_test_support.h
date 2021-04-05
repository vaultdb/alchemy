#ifndef _ENRICH_TEST_SUPPORT_H
#define _ENRICH_TEST_SUPPORT_H

#include <operators/project.h>
#include <operators/support/predicate.h>
#include <query_table/field/field_factory.h>
#include <query_table/plain_tuple.h>
#include <query_table/secure_tuple.h>

using namespace vaultdb;

// container for projection expressions, filter expressions, etc.

namespace vaultdb {
    template<typename B>
    class EnrichTestSupport {

    public:


        static Field<B> projectAgeStrata(const QueryTuple<B> &aTuple);


        static Field<B> projectMultisite(const QueryTuple<B> &aTuple);

        static Field<B> projectNumeratorMultisite(const QueryTuple<B> &aTuple);

        static QuerySchema getPatientSchema();

        static QuerySchema getPatientInclusionSchema();
    };


    template<typename B>
    class FilterExcludedPatients : public Predicate<B> {

        Field<B> cmp;
    public:
        explicit FilterExcludedPatients(const bool &isEncrypted) {
            cmp = isEncrypted ? FieldFactory<B>::getZero(FieldType::SECURE_INT) : FieldFactory<B>::getZero(
                    FieldType::INT);
        }

        ~FilterExcludedPatients() = default;

        [[nodiscard]] B predicateCall(const QueryTuple<B> &aTuple) const override {
            const Field<B> field = aTuple.getField(8);
            return  (field == cmp);
        }


    };

}

#endif //_ENRICH_TEST_SUPPORT_H
