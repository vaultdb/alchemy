#include "shared_schema.h"

QuerySchema SharedSchema::getInputSchema() {

//CREATE TABLE patient (
//    pat_id integer,
//    study_year integer,
//    zip_marker character varying(3),
//    age_days integer,
//    sex character varying(2),
//    ethnicity character varying(2),
//    race character varying(2),
//    numerator boolean DEFAULT false,
//    denominator boolean DEFAULT true,
//    denom_excl boolean DEFAULT false,
//    site_id integer
//);

    QuerySchema targetSchema(11);
    targetSchema.putField(QueryFieldDesc(0, "pat_id", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(1, "study_year", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(2, "zip_marker", "patient", FieldType::STRING, 3));    
    targetSchema.putField(QueryFieldDesc(3, "age_days", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(4, "sex", "patient", FieldType::STRING, 2));
    targetSchema.putField(QueryFieldDesc(5, "ethnicity", "patient", FieldType::STRING, 2));
    targetSchema.putField(QueryFieldDesc(6, "race", "patient", FieldType::STRING, 2));
    targetSchema.putField(QueryFieldDesc(7, "numerator", "patient", FieldType::BOOL));
    targetSchema.putField(QueryFieldDesc(8, "denominator", "patient", FieldType::BOOL));
    targetSchema.putField(QueryFieldDesc(9, "denom_excl", "patient", FieldType::BOOL));
    targetSchema.putField(QueryFieldDesc(10, "site_id", "patient", FieldType::INT));

    return targetSchema;
}
