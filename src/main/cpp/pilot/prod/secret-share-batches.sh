#!/bin/bash -x


for B in 10 20 30 40 50
do
    mkdir -p pilot/secret_shares/batch/$B
    for YEAR in 2018 2019 2020 all
    do
	echo 'Secret sharing for batch '$B' in study year '$YEAR
	./bin/secret_share_batch_from_query -D enrich_htn_prod -q pilot/queries/patient.sql  -y $YEAR -b $B -d pilot/secret_shares/batch/$B/alliance-$YEAR
    done
done



cd pilot/secret_shares
tar -cvzf alliance-batch-shares-alice.tar.gz batch/*/*.alice
tar -cvzf alliance-batch-shares-bob.tar.gz batch/*/*.bob
mv alliance-batch-shares-alice.tar.gz alliance-batch-secret-shares-bob.tar.gz ~/
