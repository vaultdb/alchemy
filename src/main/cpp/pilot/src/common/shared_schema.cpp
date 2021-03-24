#include "shared_schema.h"

QuerySchema SharedSchema::getInputSchema() {
    // CREATE TABLE patient (
//    patid int,
//    zip_marker varchar(3),
//    age_days integer,
//    sex varchar(1),
//    ethnicity bool,
//    race int,
//    numerator int default 0, -- denotes 0 = false, 1 = true
//    denom_excl int default 0 -- denotes 0 = false, 1 = true
//);
    QuerySchema targetSchema(9);
    targetSchema.putField(QueryFieldDesc(0, "patid", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(1, "zip_marker", "patient", FieldType::STRING, 3));
    targetSchema.putField(QueryFieldDesc(2, "age_days", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(3, "sex", "patient", FieldType::STRING, 1));
    targetSchema.putField(QueryFieldDesc(4, "ethnicity", "patient", FieldType::BOOL));
    targetSchema.putField(QueryFieldDesc(5, "race", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(6, "numerator", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(7, "denom_excl", "patient", FieldType::INT));
    targetSchema.putField(QueryFieldDesc(8, "site_id", "patient", FieldType::INT));

    return targetSchema;
}