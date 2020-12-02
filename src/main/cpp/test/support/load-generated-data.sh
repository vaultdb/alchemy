
./bin/generate_enrich_data test/support/csv/enrich 100
cd test/support/csv/enrich

cat alice-patient.csv  bob-patient.csv > unioned-patient.csv 
cat alice-patient-exclusion.csv bob-patient-exclusion.csv >  unioned-patient-exclusion.csv 


createdb enrich_htn_unioned
createdb enrich_htn_alice
createdb enrich_htn_bob

psql enrich_htn_unioned < ../../load-generated-data.sql
