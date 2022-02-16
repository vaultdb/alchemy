#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "usage: ./pilot/prod/run-data-partner-nm.sh <test \in (2018-2020, all)>"
    exit
fi

# may need to set up ssh tunnel first with:
#  ssh  -p 3333 -N -i /home/vaultdb/.ssh/vaultdb-nm-key -L 4444:127.0.0.1:4444 vaultdb@vtfsmsmsql01.fsm.northwestern.edu
YEAR=$1
date

./bin/run_data_partner -h 127.0.0.1 -P 4444 --party=1 -d enrich_htn_prod -y $YEAR \
		       -p pilot/secret_shares/alliance/alliance-partial-counts-$YEAR.alice \
		       -l pilot/logs/vaultdb-data-partner-no-semi-join-$YEAR



