#!/bin/bash

for $TEST in 2018 2019 2020 all
do
    ./pilot/prod/run-data-partner-nm.sh $TEST
    ./pilot/prod/run-data-partner-aggregation-nm.sh $TEST
done

