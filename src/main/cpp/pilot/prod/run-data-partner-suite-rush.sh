#!/bin/bash

for TEST in 2018 2019 2020 all
do
    bash pilot/prod/run-data-partner-rush.sh $TEST
    bash pilot/prod/run-data-partner-aggregation-rush.sh $TEST
done

