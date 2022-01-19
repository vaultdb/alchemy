#!/bin/bash
if [ "$#" -ne 1 ]; then
    echo "usage: ./pilot/prod/run-data-partner-rush.sh <test \in (2018-2020, all)>"
    exit
fi

YEAR=$1

./bin/run_data_partner -h 127.0.0.1 -P 4444 --party=2 -d enrich_htn_prod -y $YEAR \
		       --rp=pilot/secret_shares/alliance/alliance-partial-counts-$YEAR.bob \
		       --rc=pilot/secret_shares/alliance/alliance-partial-counts-$YEAR.bob 
