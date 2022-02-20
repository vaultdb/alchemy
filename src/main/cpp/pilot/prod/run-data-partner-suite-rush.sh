#!/bin/bash

for TEST in 2018 2018-2019 all
do
    bash pilot/prod/run-data-partner-aggregation-rush.sh $TEST
    bash pilot/prod/run-data-partner-rush.sh $TEST
    bash pilot/prod/run-data-partner-no-semi-join-rush.sh  $TEST
done

for BATCH in 25 50 75 100
do
    
    for TEST in 2018 2018-2019 all
    do
	bash pilot/prod/run-data-partner-batch-rush.sh $TEST $BATCH
    done
done
