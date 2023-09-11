#!/bin/bash

executable="./bin/sort_plan_enumeration_test"
PARTY=1

# Get today's date
date=$(date +'%Y-%m-%d')

unioned_dbs=("tpch_unioned_600" "tpch_unioned_1200" "tpch_unioned_2400")
alice_dbs=("tpch_alice_600" "tpch_alice_1200" "tpch_alice_2400")
bob_dbs=("tpch_bob_600" "tpch_bob_1200" "tpch_bob_2400")

filters=("*.tpch_q1" "*.tpch_q3" "*.tpch_q5" "*.tpch_q8" "*.tpch_q9" "*.tpch_q18")

for ((i=0; i<${#unioned_dbs[@]}; i++))
do
    # Create a unique log file for each data size
    log_file="SortPlanEnumerationTest_${unioned_dbs[$i]}_${date}.log"

    for filter in "${filters[@]}"
    do
        echo "Running tests with Bit Packed and filter $filter" >> "$log_file"
        $executable --party=$PARTY --unioned_db=${unioned_dbs[$i]} --alice_db=${alice_dbs[$i]} --bob_db=${bob_dbs[$i]} --filter=$filter >> "$log_file"
    done
done