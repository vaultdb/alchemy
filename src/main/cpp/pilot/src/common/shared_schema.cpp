#include "shared_schema.h"

QuerySchema SharedSchema::getInputSchema() {

// omitting study year since we either do all years or one year at a time already
//CREATE TABLE patient (
//    pat_id integer,
//    age_strata char(1),
//    sex character varying(1),
//    ethnicity character varying(1),
//    race character varying(1),
//    numerator boolean DEFAULT false,
//    denom_excl boolean DEFAULT false
//);

    QuerySchema targetSchema(11);
    targetSchema.putField(QueryFieldDesc(0, "pat_id", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(3, "age_days", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(4, "sex", "patient", FieldType::STRING, 1));
    targetSchema.putField(QueryFieldDesc(5, "ethnicity", "patient", FieldType::STRING, 1));
    targetSchema.putField(QueryFieldDesc(6, "race", "patient", FieldType::STRING, 1));
    targetSchema.putField(QueryFieldDesc(7, "numerator", "patient", FieldType::BOOL));
    targetSchema.putField(QueryFieldDesc(9, "denom_excl", "patient", FieldType::BOOL));

    return targetSchema;
}
