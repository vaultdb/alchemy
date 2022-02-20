#!/bin/bash

for TEST in 2018 2018-2019 all
do
    bash pilot/prod/run-data-partner-aggregation-nm.sh $TEST
    bash pilot/prod/run-data-partner-nm.sh $TEST
    bash pilot/prod/run-data-partner-no-semi-join-nm.sh  $TEST
done

for BATCH in 25 50 75 100
do
    
    for TEST in 2018 2018-2019 all
    do
	bash pilot/prod/run-data-partner-batch-nm.sh $TEST $BATCH
    done
done
