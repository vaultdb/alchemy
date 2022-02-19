#include "shared_schema.h"

// the fields from patient we will read in for experiments
QuerySchema SharedSchema::getInputSchema() {

// omitting study year since we either do all years or one year at a time already
//CREATE TABLE patient (
//    pat_id integer,
//    age_strata char(1),
//    sex char(1),
//    ethnicity char(1),
//    race char(1),
//    numerator boolean DEFAULT false,
//    denom_excl boolean DEFAULT false
//);

    QuerySchema targetSchema(9);
    targetSchema.putField(QueryFieldDesc(0, "study_year", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(1, "pat_id", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(2, "age_strata", "patient", FieldType::STRING, 1));
    targetSchema.putField(QueryFieldDesc(3, "sex", "patient", FieldType::STRING, 1));
    targetSchema.putField(QueryFieldDesc(4, "ethnicity", "patient", FieldType::STRING, 1));
    targetSchema.putField(QueryFieldDesc(5, "race", "patient", FieldType::STRING, 1));
    targetSchema.putField(QueryFieldDesc(6, "numerator", "patient", FieldType::BOOL));
    targetSchema.putField(QueryFieldDesc(7, "denominator", "patient", FieldType::BOOL));
    targetSchema.putField(QueryFieldDesc(8, "denom_excl", "patient", FieldType::BOOL));

    return targetSchema;
}



// the fields from patient we will read in for experiments
QuerySchema SharedSchema::getPatientSchema() {


    // real, stored patient schema at each site
    //CREATE TABLE patient (
//    pat_id integer,
//    study_year int,
//    age_strata char(1),
//    sex char(1),
//    ethnicity char(1),
//    race char(1),
//    numerator boolean DEFAULT false,
//    denom_excl boolean DEFAULT false
//    site_id INT,
//    multisite bool
//);

    QuerySchema targetSchema(10);
    targetSchema.putField(QueryFieldDesc(0, "pat_id", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(1, "study_year", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(2, "age_strata", "patient", FieldType::STRING, 1));
    targetSchema.putField(QueryFieldDesc(3, "sex", "patient", FieldType::STRING, 1));
    targetSchema.putField(QueryFieldDesc(4, "ethnicity", "patient", FieldType::STRING, 1));
    targetSchema.putField(QueryFieldDesc(5, "race", "patient", FieldType::STRING, 1));
    targetSchema.putField(QueryFieldDesc(6, "numerator", "patient", FieldType::BOOL));
    targetSchema.putField(QueryFieldDesc(7, "denom_excl", "patient", FieldType::BOOL));
    targetSchema.putField(QueryFieldDesc(8, "site_id", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(9, "multisite", "patient", FieldType::BOOL));

    return targetSchema;
}

// study_year(0), age_strata (1), sex (2), ethnicity (3) , race (4), numerator_cnt (5), denominator_cnt (6), numerator_multisite_cnt (7), denominator_multisite_cnt (8)
QuerySchema SharedSchema::getPartialCountSchema() {
    QuerySchema targetSchema(9);
    targetSchema.putField(QueryFieldDesc(0, "study_year", "patient", FieldType::INT, 0));
    targetSchema.putField(QueryFieldDesc(1, "age_strata", "patient", FieldType::STRING, 1));
    targetSchema.putField(QueryFieldDesc(2, "sex", "patient", FieldType::STRING, 1));
    targetSchema.putField(QueryFieldDesc(3, "ethnicity", "patient", FieldType::STRING, 1));
    targetSchema.putField(QueryFieldDesc(4, "race", "patient", FieldType::STRING, 1));
    targetSchema.putField(QueryFieldDesc(5, "numerator_cnt", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(6, "denominator_cnt", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(7, "numerator_multisite_cnt", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(8, "denominator_multisite_cnt", "patient", FieldType::INT));

    return targetSchema;
}
