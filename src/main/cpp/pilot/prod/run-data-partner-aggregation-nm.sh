#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "usage: ./pilot/prod/run-data-partner-aggregation-nm.sh <test \in (2018-2020, all)>"
    exit
fi

YEAR=$1
# May need to connect to NM first with:
# ssh -p 3333 -f -N -i ~/.ssh/vaultdb-nm-key -L 4444:127.0.0.1:4444 vaultdb@165.124.123.122
./bin/run_data_partner_aggregation 127.0.0.1 4444 1 enrich_htn_prod $YEAR pilot/secret_shares/alliance/alliance-partial-counts-no-dedupe-$YEAR.alice pilot/logs/vaultdb-data-partner-aggregation-$YEAR

