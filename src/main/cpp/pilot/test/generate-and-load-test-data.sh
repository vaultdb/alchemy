
if [ "$#" -ne 1 ]; then
    echo "usage: ./pilot/test/load-generated-data.sh <tuple count per host>"
    exit
fi

TUPLE_COUNT=$1
mkdir -p pilot/test/input
mkdir -p pilot/test/output

cmake .  
make -j5 

./bin/generate_enrich_data_three_parties pilot/test/input/ $TUPLE_COUNT 


dropdb  --if-exists enrich_htn_unioned
createdb enrich_htn_unioned


psql enrich_htn_unioned <  pilot/test/load-generated-data.sql


pg_dump enrich_htn_unioned > pilot/test/output/enrich_htn_unioned.sql

#whole tables
psql enrich_htn_unioned -t --csv -c   "SELECT DISTINCT pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl FROM patient WHERE site_id=1 ORDER BY pat_id" >  pilot/test/input/alice-patient.csv
psql enrich_htn_unioned -t  --csv -c   "SELECT DISTINCT pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl FROM patient WHERE site_id=2 ORDER BY pat_id" >  pilot/test/input/bob-patient.csv
psql enrich_htn_unioned -t  --csv -c   "SELECT DISTINCT pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl FROM patient WHERE site_id=3 ORDER BY pat_id" >  pilot/test/input/chi-patient.csv

#multisite tables
psql enrich_htn_unioned -t --csv -c   "SELECT DISTINCT pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl FROM patient WHERE site_id=1 AND multisite ORDER BY pat_id" >  pilot/test/input/alice-multisite-patient.csv
psql enrich_htn_unioned -t  --csv -c   "SELECT DISTINCT pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl FROM patient WHERE site_id=2 AND multisite ORDER BY pat_id" >  pilot/test/input/bob-multisite-patient.csv
psql enrich_htn_unioned -t  --csv -c   "SELECT DISTINCT pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl FROM patient WHERE site_id=3 AND multisite ORDER BY pat_id" >  pilot/test/input/chi-multisite-patient.csv


#original table
./bin/secret_share_csv  pilot/test/input/chi-patient.csv pilot/test/output/chi-patient
# multi-site only
./bin/secret_share_csv  pilot/test/input/chi-patient.csv pilot/test/output/chi-patient

./bin/secret_share_partial_counts enrich_htn_unioned pilot/secret_shares/tables/partial_counts_alice 1
./bin/secret_share_partial_counts enrich_htn_unioned pilot/secret_shares/tables/partial_counts_bob 2
./bin/secret_share_partial_counts enrich_htn_unioned pilot/secret_shares/tables/partial_counts_chi 3
