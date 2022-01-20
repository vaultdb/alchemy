#!/bin/bash

for TEST in 2018 2019 2020 all
do
    bash pilot/prod/run-data-partner-nm.sh $TEST
    bash pilot/prod/run-data-partner-aggregation-nm.sh $TEST
done

