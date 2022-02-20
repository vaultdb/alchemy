#!/bin/bash

for TEST in  2018-2019 all #2018
do
    bash pilot/prod/run-data-partner-aggregation-nm.sh $TEST
    sleep 2
    bash pilot/prod/run-data-partner-nm.sh $TEST
    sleep 2
    bash pilot/prod/run-data-partner-no-semi-join-nm.sh  $TEST
    sleep 2
done

for BATCH in 25 50 75 100
do
    
    for TEST in 2018 2018-2019 all
    do
	bash pilot/prod/run-data-partner-batch-nm.sh $TEST $BATCH
	sleep 2
    done
done
