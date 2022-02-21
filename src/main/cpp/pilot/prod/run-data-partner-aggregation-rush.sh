#!/bin/bash -x

if [ "$#" -ne 1 ]; then
    echo "usage: ./pilot/prod/run-data-partner-aggregation-rush.sh <test \in (2018-2020, all)>"
    exit
fi

YEAR=$1
# May need to connect to NM first with:
#may need to set up ssh agent with:
#  eval `ssh-agent -s`
# ssh-add ~/.ssh/vaultdb-nm-key

ssh -p 3333 -f -N -o StrictHostKeyChecking=no  -i ~/.ssh/vaultdb-nm-key -L 4444:127.0.0.1:4444 vaultdb@165.124.123.122 &
sleep 2

 ./bin/run_data_partner_aggregation 127.0.0.1 4444 2 enrich_htn_prod $YEAR pilot/secret_shares/alliance/alliance-partial-counts-no-dedupe-$YEAR.bob  pilot/logs/vaultdb-data-partner-aggregation-$YEAR


