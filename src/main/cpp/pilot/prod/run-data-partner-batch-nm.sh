#!/bin/bash

# only "all" available in secret shares right now

if [ "$#" -ne 2]; then
    echo "usage: ./pilot/prod/run-data-partner-nm.sh <test \in (2018-2020, all)> <batch count>"
    exit
fi

YEAR=$1
BATCHES=$2

./bin/run_data_partner_batch -h 127.0.0.1 -P 4444 --party=1 -d enrich_htn_prod -y $YEAR \
		       -r pilot/secret_shares/batch/$BATCHES/alliance-all \
		       -l pilot/logs/vaultdb-data-partner-batch-$YEAR \
		       -s --batch-count=$BATCHES
