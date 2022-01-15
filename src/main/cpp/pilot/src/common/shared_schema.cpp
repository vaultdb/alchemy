#include "shared_schema.h"

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

    QuerySchema targetSchema(7);
    targetSchema.putField(QueryFieldDesc(0, "pat_id", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(1, "age_stratum", "patient", FieldType::STRING, 1));
    targetSchema.putField(QueryFieldDesc(2, "sex", "patient", FieldType::STRING, 1));
    targetSchema.putField(QueryFieldDesc(3, "ethnicity", "patient", FieldType::STRING, 1));
    targetSchema.putField(QueryFieldDesc(4, "race", "patient", FieldType::STRING, 1));
    targetSchema.putField(QueryFieldDesc(5, "numerator", "patient", FieldType::BOOL));
    targetSchema.putField(QueryFieldDesc(6, "denom_excl", "patient", FieldType::BOOL));

    return targetSchema;
}
