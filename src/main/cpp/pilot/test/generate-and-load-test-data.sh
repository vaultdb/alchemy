#!/bin/bash -x

if [ "$#" -ne 1 ]; then
    echo "usage: ./pilot/test/load-generated-data.sh <tuple count per host>"
    exit
fi

TUPLE_COUNT=$1
DB_NAME=enrich_htn_unioned_3pc

mkdir -p pilot/test/input
mkdir -p pilot/test/output


#cmake .  
make -j5 

./bin/generate_enrich_data_three_parties pilot/test/input/ $TUPLE_COUNT 


dropdb  --if-exists $DB_NAME
createdb $DB_NAME


psql $DB_NAME <  pilot/test/load-generated-data.sql



pg_dump $DB_NAME > pilot/test/output/$DB_NAME.sql

#whole tables
psql $DB_NAME -t --csv -c   "SELECT  pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl FROM patient WHERE site_id=1 ORDER BY pat_id" >  pilot/test/input/alice-patient.csv
psql $DB_NAME -t  --csv -c   "SELECT  pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl FROM patient WHERE site_id=2 ORDER BY pat_id" >  pilot/test/input/bob-patient.csv
psql $DB_NAME -t  --csv -c   "SELECT  pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl FROM patient WHERE site_id=3 ORDER BY pat_id" >  pilot/test/input/chi-patient.csv

echo "Writing multisite tables!"

#multisite tables
psql $DB_NAME -t --csv -c   "SELECT  pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl FROM patient WHERE site_id=1 AND multisite ORDER BY pat_id" >  pilot/test/input/alice-multisite-patient.csv
psql $DB_NAME -t  --csv -c   "SELECT  pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl FROM patient WHERE site_id=2 AND multisite ORDER BY pat_id" >  pilot/test/input/bob-multisite-patient.csv
psql $DB_NAME -t  --csv -c   "SELECT  pat_id, age_strata, sex, ethnicity, race, numerator, denom_excl FROM patient WHERE site_id=3 AND multisite ORDER BY pat_id" >  pilot/test/input/chi-multisite-patient.csv

# for aggregate-only test
psql $DB_NAME -t --csv < pilot/queries/data-cube-query.sql  > pilot/test/input/chi-patient-aggregate.csv


# multi-site only
./bin/secret_share_from_query enrich_htn_unioned_chi pilot/queries/patient-multisite-all.sql  pilot/test/output/chi-patient-multisite


#setup for semijoin optimization
# TODO: deprecate this file
#./bin/secret_share_partial_counts_no_dedupe enrich_htn_unioned_chi  pilot/secret_shares/tables/chi_counts
./bin/secret_share_from_query enrich_htn_unioned_chi pilot/queries/partial-count-all.sql pilot/secret_shares/tables/chi_partial_counts
./bin/secret_share_from_query enrich_htn_unioned_chi pilot/queries/partial-count-no-dedupe-all.sql pilot/secret_shares/tables/chi_counts
