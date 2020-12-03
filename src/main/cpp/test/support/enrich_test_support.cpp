#include "enrich_test_support.h"

using namespace emp;


Integer EnrichTestSupport::getEmpInt(const int32_t & value) {
    return Integer(32, value, PUBLIC);
}


// Project #1
// CASE WHEN age_days <= 28*365 THEN 0
//                WHEN age_days > 28*365 AND age_days <= 39*365 THEN 1
//              WHEN age_days > 39*365  AND age_days <= 50*365 THEN 2
//              WHEN age_days > 50*365 AND age_days <= 61*365 THEN 3
//              WHEN age_days > 61*365 AND age_days <= 72*365 THEN 4
//                WHEN age_days > 72*365  AND age_days <= 83*365 THEN 5
//                ELSE 6 END age_strata

// TODO: set this up to memoize the emp ints so we don't have to generate them every time
Value EnrichTestSupport::projectSecureAgeStrata(const QueryTuple & aTuple) {
    Integer ageDays = aTuple.getField(2).getValue().getEmpInt(); // age_days


    Integer ageStrata = If(ageDays <= getEmpInt(28*365), getEmpInt(0),
                           If(ageDays <= getEmpInt(39*365), getEmpInt(1),
                              If(ageDays <= getEmpInt(50*365), getEmpInt(2),
                                 If(ageDays <= getEmpInt(61*365), getEmpInt(3),
                                    If(ageDays <= getEmpInt(72*365), getEmpInt(4),
                                       If(ageDays <= getEmpInt(83*365), getEmpInt(5), getEmpInt(6)))))));

    return Value(TypeId::ENCRYPTED_INTEGER32, ageStrata);

}

Value EnrichTestSupport::projectPlainAgeStrata(const QueryTuple & aTuple) {
    int32_t ageDays = aTuple.getField(2).getValue().getInt32(); // age_days

    if(ageDays <= 28*365) return  Value(0);
    else if(ageDays <= 39*365) return  Value(1);
    else if(ageDays <= 50*365) return  Value( 2);
    else if(ageDays <= 61*365) return  Value( 3);
    else if(ageDays <=72*365) return  Value(4);
    else if(ageDays <= 83*365) return  Value( 5);

    return Value( 6);

}

Value EnrichTestSupport::projectAgeStrata(const QueryTuple & aTuple) {
    TypeId ageType = aTuple.getFieldPtr(2)->getValue().getType();
    if(TypeUtilities::isEncrypted(ageType))
        return projectSecureAgeStrata(aTuple);

    return projectPlainAgeStrata(aTuple);
}


// Project #2
//     CASE WHEN count(*) > 1 THEN 1 else 0 END AS multisite
Value EnrichTestSupport::projectMultisite(const QueryTuple & aTuple) {
    Value siteCount = aTuple.getField(6).getValue();
    TypeId siteCountType = siteCount.getType(); // can be MPC or plaintext
    Value zero = TypeUtilities::getZero(siteCountType);
    Value one = TypeUtilities::getOne(siteCountType);

    Value condition =  siteCount > one;
    // get from Value::TypeId bool --> int
    if(TypeUtilities::isEncrypted(siteCountType))
        return Value::obliviousIf(condition, one, zero);

    return condition.getBool() ? one : zero;

}

//    CASE WHEN MAX(numerator)=1 ^ COUNT(*) > 1 THEN 1 ELSE 0 END AS numerator_multisite
Value EnrichTestSupport::projectNumeratorMultisite(const QueryTuple & aTuple) {

    Value inNumerator = aTuple.getFieldPtr(5)->getValue();
    Value siteCount = aTuple.getFieldPtr(6)->getValue();
    TypeId siteCountType = siteCount.getType(); // can be MPC or plaintext
    Value zero = TypeUtilities::getZero(siteCountType);
    Value one = TypeUtilities::getOne(siteCountType);

    Value condition =  (siteCount > one) ^ (inNumerator == one);

    // get from Value::TypeId bool --> int
    if(TypeUtilities::isEncrypted(siteCountType))
        return Value::obliviousIf(condition, one, zero);

    return condition.getBool() ? one : zero;

}


// patients(patid int, zip_marker varchar(3), age_days integer, sex varchar(1), ethnicity bool, race int, numerator int default null)
QuerySchema EnrichTestSupport::getPatientSchema() {
    QuerySchema patientSchema(7);
    patientSchema.putField(QueryFieldDesc(0, "patid", "patient", TypeId::INTEGER32));
    patientSchema.putField(QueryFieldDesc(1, "zip_marker", "patient", TypeId::VARCHAR, 3));
    patientSchema.putField(QueryFieldDesc(2, "age_days", "patient", TypeId::INTEGER32));
    patientSchema.putField(QueryFieldDesc(3, "sex", "patient", TypeId::VARCHAR, 1));
    patientSchema.putField(QueryFieldDesc(4, "ethnicity", "patient", TypeId::BOOLEAN));
    patientSchema.putField(QueryFieldDesc(5, "race", "patient", TypeId::INTEGER32));
    // numerator: null = false, 1 = true
    patientSchema.putField(QueryFieldDesc(6, "numerator", "patient", TypeId::INTEGER32));

    return patientSchema;
}

// patient_inclusion(patid int, numerator int, denom_incl int)
QuerySchema EnrichTestSupport::getPatientInclusionSchema() {
    QuerySchema patientInclusionSchema(3);
    patientInclusionSchema.putField(QueryFieldDesc(0, "patid", "patient_inclusion", TypeId::INTEGER32));
    // numerator: null = false, 1 = true
    patientInclusionSchema.putField(QueryFieldDesc(1, "numerator", "patient_inclusion", TypeId::INTEGER32));
    // denom_excl: null = false, 1 = true
    patientInclusionSchema.putField(QueryFieldDesc(2, "denom_incl", "patient_inclusion", TypeId::INTEGER32));

}