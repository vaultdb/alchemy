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
    QuerySchema targetSchema(8);
    targetSchema.putField(QueryFieldDesc(0, "patid", "patient", TypeId::INTEGER32));
    targetSchema.putField(QueryFieldDesc(1, "zip_marker", "patient", TypeId::VARCHAR, 3));
    targetSchema.putField(QueryFieldDesc(2, "age_days", "patient", TypeId::INTEGER32));
    targetSchema.putField(QueryFieldDesc(3, "sex", "patient", TypeId::VARCHAR, 1));
    targetSchema.putField(QueryFieldDesc(4, "ethnicity", "patient", TypeId::BOOLEAN));
    targetSchema.putField(QueryFieldDesc(5, "race", "patient", TypeId::INTEGER32));
    targetSchema.putField(QueryFieldDesc(6, "numerator", "patient", TypeId::INTEGER32));
    targetSchema.putField(QueryFieldDesc(7, "denom_excl", "patient", TypeId::INTEGER32));
    return targetSchema;
}