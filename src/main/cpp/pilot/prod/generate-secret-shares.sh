#!/bin/bash

mkdir -p pilot/secret_shares/alliance
mkdir -p pilot/secret_shares/batch

for BATCH in 25 50 75 100
  do
    mkdir -p pilot/secret_shares/batch/$BATCH
done

for YEAR in '2018' '2018-2019' 'all'
do
  #multisite patients
  ./bin/secret_share_from_query -D enrich_htn_prod -q pilot/queries/patient.sql -y $YEAR -d  pilot/secret_shares/alliance/multisite-$YEAR -s
  # partial counts
  ./bin/secret_share_from_query -D enrich_htn_prod -q  pilot/queries/partial-count.sql -y $YEAR -d  alliance-partial-counts-$YEAR
  #aggregate-only
  ./bin/secret_share_from_query -D enrich_htn_prod -q pilot/queries/partial-count-no-dedupe.sql -y $YEAR -d   pilot/secret_shares/alliance/alliance-partial-counts-no-dedupe-$YEAR

  for BATCH in 25 50 75 100
  do
     ./bin/secret_share_batch_from_query -D enrich_htn_prod -q pilot/queries/patient.sql -y $YEAR -b $BATCH -d pilot/secret_shares/batch/$BATCH/alliance-$YEAR
  done

done

tar -cvzf alliance-alice-shares.tar.gz  pilot/secret_shares/alliance/*.alice pilot/secret_shares/batch/*/*.alice
tar -cvzf alliance-bob-shares.tar.gz  pilot/secret_shares/alliance/*.bob pilot/secret_shares/batch/*/*.bob

