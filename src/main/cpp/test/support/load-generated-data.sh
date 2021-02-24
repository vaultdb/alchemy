
if [ "$#" -ne 1 ]; then
    echo "usage: ./load-generated-data.sh <tuple count per host>"
    exit
fi

TUPLE_COUNT=$1

./bin/generate_enrich_data test/support/csv/enrich $TUPLE_COUNT
cd test/support/csv/enrich

cat alice-patient.csv  bob-patient.csv > unioned-patient.csv 
cat alice-patient-exclusion.csv bob-patient-exclusion.csv >  unioned-patient-exclusion.csv 


dropdb enrich_htn_unioned
dropdb enrich_htn_alice
dropdb enrich_htn_bob

createdb enrich_htn_unioned
createdb enrich_htn_alice
createdb enrich_htn_bob

psql enrich_htn_unioned < ../../load-generated-data.sql
