
if [ "$#" -ne 1 ]; then
    echo "usage: ./pilot/test/load-generated-data.sh <tuple count per host>"
    exit
fi

TUPLE_COUNT=$1
mkdir -p pilot/test/input
mkdir -p pilot/test/output

cmake .  && \ 
make && \
./bin/generate_enrich_data_three_parties pilot/test/input/ $TUPLE_COUNT
./bin/secret_share_csv  pilot/test/input/chi-patient.csv pilot/test/output/chi-patient

dropdb  --if-exists enrich_htn_unioned
createdb enrich_htn_unioned

cd pilot/test/input
psql enrich_htn_unioned < ../load-generated-data.sql

cd ..
pg_dump enrich_htn_unioned > output/enrich_htn_unioned.sql
