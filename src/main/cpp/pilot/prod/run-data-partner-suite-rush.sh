#!/bin/bash

for $TEST in 2018 2019 2020 all
do
    ./pilot/prod/run-data-partner-rush.sh $TEST
    ./pilot/prod/run-data-partner-aggregation-rush.sh $TEST
done

