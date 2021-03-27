#include "enrich_test_support.h"

using namespace emp;




// Project #1
// CASE WHEN age_days <= 28*365 THEN 0
//                WHEN age_days > 28*365 AND age_days <= 39*365 THEN 1
//              WHEN age_days > 39*365  AND age_days <= 50*365 THEN 2
//              WHEN age_days > 50*365 AND age_days <= 61*365 THEN 3
//              WHEN age_days > 61*365 AND age_days <= 72*365 THEN 4
//                WHEN age_days > 72*365  AND age_days <= 83*365 THEN 5
//                ELSE 6 END age_strata

template<typename B>
Field<B> EnrichTestSupport<B>::projectAgeStrata(const QueryTuple<B> & aTuple) {

    Field<B> ageDays = aTuple[2]; // age_days
    // either an IntField or SecureIntField

    Field<B> ageStrata = Field<B>::If(ageDays <= FieldFactory<B>::getInt(28*365), FieldFactory<B>::getInt(0),
                                      Field<B>::If(ageDays <= FieldFactory<B>::getInt(39*365), FieldFactory<B>::getInt(1),
                                                   Field<B>::If(ageDays <= FieldFactory<B>::getInt(50*365), FieldFactory<B>::getInt(2),
                                                                Field<B>::If(ageDays <= FieldFactory<B>::getInt(61*365), FieldFactory<B>::getInt(3),
                                                                             Field<B>::If(ageDays <= FieldFactory<B>::getInt(72*365), FieldFactory<B>::getInt(4),
                                                                                          Field<B>::If(ageDays <= FieldFactory<B>::getInt(83*365), FieldFactory<B>::getInt(5), FieldFactory<B>::getInt(6)))))));

    return ageStrata;
}


// Project #2
//     CASE WHEN count(*) > 1 THEN 1 else 0 END AS multisite
template<typename B>
Field<B> EnrichTestSupport<B>::projectMultisite(const QueryTuple<B> & aTuple) {
    // int32_t
    Field<B> siteCount = aTuple[7];
    Field<B> zero =  FieldFactory<B>::getInt(0);
    Field<B> one =  FieldFactory<B>::getInt(1);

    B cmp = siteCount > FieldFactory<B>::getOne(siteCount.getType());

    return Field<B>::If(cmp, one, zero);

}


//    CASE WHEN MAX(numerator)=1 ^ COUNT(*) > 1 THEN 1 ELSE 0 END AS numerator_multisite
template<typename B>
Field<B> EnrichTestSupport<B>::projectNumeratorMultisite(const QueryTuple<B> & aTuple) {

    Field<B> inNumerator = aTuple[6];
    Field<B> siteCount = aTuple[7];
    Field<B> zero = FieldFactory<B>::getInt(0);
    Field<B> one = FieldFactory<B>::getInt(1);


    B multisite = (siteCount > FieldFactory<B>::getOne(siteCount.getType()));
    // only 0 || 1
    B numeratorTrue = inNumerator > FieldFactory<B>::getZero(inNumerator.getType());
    B condition = multisite & numeratorTrue;

    return Field<B>::If(condition, one, zero);

}


// patients(patid int, zip_marker varchar(3), age_days integer, sex varchar(1), ethnicity bool, race int, numerator int default null)
template<typename B>
QuerySchema EnrichTestSupport<B>::getPatientSchema() {
    QuerySchema patientSchema(7);
    patientSchema.putField(QueryFieldDesc(0, "patid", "patient", FieldType::INT));
    patientSchema.putField(QueryFieldDesc(1, "zip_marker", "patient", FieldType::STRING, 3));
    patientSchema.putField(QueryFieldDesc(2, "age_days", "patient", FieldType::INT));
    patientSchema.putField(QueryFieldDesc(3, "sex", "patient", FieldType::STRING, 1));
    patientSchema.putField(QueryFieldDesc(4, "ethnicity", "patient", FieldType::BOOL));
    patientSchema.putField(QueryFieldDesc(5, "race", "patient", FieldType::INT));
    // numerator: null = false, 1 = true
    patientSchema.putField(QueryFieldDesc(6, "numerator", "patient", FieldType::INT));

    return patientSchema;
}

// patient_inclusion(patid int, numerator int, denom_incl int)
template<typename B>
QuerySchema EnrichTestSupport<B>::getPatientInclusionSchema() {
    QuerySchema patientInclusionSchema(3);
    patientInclusionSchema.putField(QueryFieldDesc(0, "patid", "patient_inclusion", FieldType::INT));
    // numerator: null = false, 1 = true
    patientInclusionSchema.putField(QueryFieldDesc(1, "numerator", "patient_inclusion", FieldType::INT));
    // denom_excl: null = false, 1 = true
    patientInclusionSchema.putField(QueryFieldDesc(2, "denom_incl", "patient_inclusion", FieldType::INT));
    return patientInclusionSchema;
}

template class vaultdb::FilterExcludedPatients<bool>;
template class vaultdb::FilterExcludedPatients<emp::Bit>;

template class vaultdb::EnrichTestSupport<bool>;
template class vaultdb::EnrichTestSupport<emp::Bit>;