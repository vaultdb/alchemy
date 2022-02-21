#!/bin/bash
if [ "$#" -ne 1 ]; then
    echo "usage: ./pilot/prod/run-data-partner-rush.sh <test \in (2018-2020, all)>"
    exit
fi

YEAR=$1

#may need to set up ssh agent with:
#  eval `ssh-agent -s`
# ssh-add ~/.ssh/vaultdb-nm-key


ssh -p 3333 -f -N -o StrictHostKeyChecking=no -i ~/.ssh/vaultdb-nm-key -L 4444:127.0.0.1:4444 vaultdb@165.124.123.122 &

sleep 2

./bin/run_data_partner -h 127.0.0.1 -P 4444 --party=2 -d enrich_htn_prod -y $YEAR \
		       -p pilot/secret_shares/alliance/alliance-partial-counts-$YEAR.bob \
		       -r pilot/secret_shares/alliance/multisite-$YEAR.bob \
		       -l pilot/logs/vaultdb-data-partner-$YEAR -s 
