#!/bin/bash

# only "all" available in secret shares right now
if [ "$#" -ne 2 ]; then
    echo "usage: ./pilot/prod/run-data-partner-rush.sh <test \in (2018-2020, all)> <batch count>"
    exit
fi

YEAR=$1
BATCHES=$2

#may need to run ssh setup first with:
# ssh -p 3333 -f -N -i ~/.ssh/vaultdb-nm-key -L 4444:127.0.0.1:4444 vaultdb@165.124.123.122

./bin/run_data_partner_batch -h 127.0.0.1 -P 4444 --party=2 -d enrich_htn_prod -y $YEAR \
		       -r pilot/secret_shares/batch/$BATCHES/alliance-all \
		       -l pilot/logs/vaultdb-data-partner-$YEAR-$BATCHES \
		       --batch-count=$BATCHES
